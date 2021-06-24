/** trilateration.cpp
 *
 * Copyright (c) 2021 Navigine.
 *
 */
#include "trilateration.h"
#include <Eigen/Dense>

namespace navigine {
namespace navigation_core {

RadioMeasurementsData getIntersectedMeasurements(const Level &level, const RadioMeasurementsData& radioMsr)
{
    RadioMeasurementsData newMsr = radioMsr;
    RadioMeasurementsData badMsr;

    // Sorting distances in ascending order
    std::sort(newMsr.begin(), newMsr.end(),
              [](const RadioMeasurementData& lhs, const RadioMeasurementData& rhs)
              {
                return lhs.distance < rhs.distance;
              });

    // Method for determining the Access Point that fails
    for (const auto& msr1 : newMsr)
    {
      if (std::find_if(badMsr.begin(), badMsr.end(),
                      [msr1](const RadioMeasurementData& msr) 
                      {
                        return msr1.id == msr.id;
                      }) == badMsr.end())
      {
        Transmitter<XYZPoint> tx1 = level.transmitter(msr1.id);
        const double x1 = tx1.point.x;
        const double y1 = tx1.point.y;
        const double z1 = tx1.point.z;
        for (const auto& msr2 : newMsr)
        {
          Transmitter<XYZPoint> tx2 = level.transmitter(msr2.id);
          const double x2 = tx2.point.x;
          const double y2 = tx2.point.y;
          const double z2 = tx2.point.z;
          const double dx = x2 - x1;
          const double dy = y2 - y1;
          const double dz = z2 - z1;
          const double delta = std::sqrt(dx * dx + dy * dy + dz * dz);
          if (delta > (msr1.distance + msr2.distance))
            badMsr.push_back(msr2);
        }
      }
    }

    // Update measurement vector with bad Access Points
    for (const auto& msr : badMsr)
    {
      newMsr.erase(std::remove_if(newMsr.begin(), newMsr.end(),
                                [msr](const RadioMeasurementData& elem)
                                {
                                  return msr.id == elem.id;
                                } ),newMsr.end());
    }

    return newMsr;
}

boost::optional<double> Trilateration::calculateAltitude(const Level& level, const RadioMeasurementsData& radioMeasurements)
{
    const RadioMeasurementsData levelMsrs = getLevelRadioMeasurements(level, radioMeasurements);
    const RadioMeasurementsData radioMsrs = getIntersectedMeasurements(level, levelMsrs);

    if (radioMsrs.size() < 4)
        return boost::none;

    // Define the matrix that we are going to use
    size_t count = radioMsrs.size();
    size_t rows = count - 1;
    Eigen::MatrixXd m(rows, 3);
    Eigen::VectorXd b(rows);

    // Fill in matrix according to the equations
    size_t row = 0;
    double x1, x2, y1, y2, z1, z2, r1, r2;

    Transmitter<XYZPoint> tx1 = level.transmitter(radioMsrs[0].id);
    x1 = tx1.point.x, y1 = tx1.point.y, z1 = tx1.point.z;
    r1 = radioMsrs[0].distance;
    for (size_t i = 1; i < count; ++i)
    {
      Transmitter<XYZPoint> tx2 = level.transmitter(radioMsrs[i].id);
      x2 = tx2.point.x, y2 = tx2.point.y, z2 = tx2.point.z;
      r2 = radioMsrs[i].distance;
      m(row, 0) = x2 - x1;
      m(row, 1) = y2 - y1;
      m(row, 2) = z2 - z1;
      b(row) = (pow(r1, 2) - pow(r2, 2) + (pow((x2 - x1), 2) + pow((y2 - y1), 2) + pow((z2 - z1), 2))) / 2;
      row++;
    }
    Eigen::Vector3d rawLocation = m.bdcSvd(Eigen::ComputeThinU|Eigen::ComputeThinV).solve(b);
    double x = rawLocation[0] + x1;
    double y = rawLocation[1] + y1;
    double sum1 = 0.0;
    // double sum2 = 0.0;
    bool nanFlag = true;
    for (const auto& msr : radioMsrs)
    {
      Transmitter<XYZPoint> tx = level.transmitter(msr.id);
      double deltaZ = std::sqrt(pow(msr.distance, 2) - pow((x - tx.point.x), 2 - pow((y - tx.point.y), 2)));
      if (!std::isnan(deltaZ))
      {
        nanFlag = false;
        sum1 += (tx.point.z + deltaZ) / msr.distance; //if the tracked position is below all beacons, then use the sign '-', else: '+'
        //in theory it should also be calculated: sum2 += 1.0 / msr.distance, but in practice gives worse results
      }
    }

    if (nanFlag)
      return boost::none;
    else
      return sum1; //sum1 / sum2;
}

} } // namespace navigine::navigation_core
