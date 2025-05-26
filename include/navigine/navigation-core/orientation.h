#pragma once

namespace navigine::navigation_core {

// rotate sequence: heading->pitch->roll
struct Orientation
{
    // Heading, angle of rotation about the -Z axis (in radians).
    // This value represents the angle between the device's Y
    // axis and the magnetic north pole. When facing north, this
    // angle is 0, when facing south, this angle is pi.
    // Likewise, when facing east, this angle is pi/2, and
    // when facing west, this angle is -pi/2. The range of
    // values is [-pi, pi].
    double heading = 0.0;

    // Pitch, angle of rotation about the X axis (in radians).
    // This value represents the angle between a plane parallel
    // to the device's screen and a plane parallel to the ground.
    // Assuming that the bottom edge of the device faces the
    // user and that the screen is face-up, tilting the top edge
    // of the device toward the ground creates a positive pitch
    // angle. The range of values is -pi/2 to pi/2.
    double pitch = 0.0;

    // Roll, angle of rotation about the Y axis (in radians).
    // This value represents the angle between a plane perpendicular
    // to the device's screen and a plane perpendicular to the
    // ground. Assuming that the bottom edge of the device faces
    // the user and that the screen is face-up, tilting the left
    // edge of the device toward the ground creates a positive
    // roll angle. The range of values is -pi to pi.
    double roll = 0.0;
};

} // namespace navigine::navigation_core