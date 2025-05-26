#include <numeric>

#include <navigine/navigation_core/geometry.h>
#include <navigine/navigation_core/navigation_settings.h>
#include <navigine/navigation_core/math/math.h>

#include "graph_particle_filter.h"
#include "likelihood_model.h"

namespace navigine::navigation_core {

namespace {

const int SAMPLE_SIZE_MULTIPLIER = 10;

NavigationGraph::Edge getClosestEdge(
    const NavigationGraph& graph,
    const XYPoint& point)
{
    NavigationGraph::Edge closestEdge;
    double minDist = std::numeric_limits<double>::max();

    for(const auto& e : graph.edges) {
        const auto src = graph.getVertex(e.src);
        const auto dst = graph.getVertex(e.dst);

        double dist = SegmentPointDist(XYSegment{src.point, dst.point}, point);
        if (dist < minDist) {
            minDist = dist;
            closestEdge = e;
        }
    }

    return closestEdge;
}

double getAngle(
    const NavigationGraph::Vertex& src,
    const NavigationGraph::Vertex& dst)
{
    double dx = dst.point.x - src.point.x;
    double dy = dst.point.y - src.point.y;
    return math::atan2(dy, dx);
}

void sampleParticleOnEdge(
    const NavigationGraph::Vertex& src,
    const NavigationGraph::Vertex& dst,
    const NavigationGraph::Edge&,
    GraphParticle& particle,
    Rng* rng)
{
    const auto percent = rng->uniform(0, 1);
    const auto isMoveForward = rng->binary(0.5);

    // NB: leaving particle weight unchanged
    particle.point.x = (1.0 - percent) * src.point.x + percent * dst.point.x;
    particle.point.y = (1.0 - percent) * src.point.y + percent * dst.point.y;
    particle.angle = isMoveForward ? getAngle(src, dst) : getAngle(dst, src);
    particle.fromVertex = isMoveForward ? src.id : dst.id;
    particle.toVertex = isMoveForward ? dst.id : src.id;
    particle.percent = percent;
}

void sampleParticleOnGraph(
    const NavigationGraph& graph,
    GraphParticle& particle,
    Rng* rng)
{
    double totalLength = std::accumulate(graph.edges.begin(), graph.edges.end(), 0,
        [](double sum, const NavigationGraph::Edge& e) {
            return sum + e.len;
        });

    double randomLen = rng->uniform(0, totalLength);

    for(const auto& edge : graph.edges) {
        if (randomLen > edge.len) {
            randomLen -= edge.len;
            continue;
        }
        return sampleParticleOnEdge(
            graph.getVertex(edge.src),
            graph.getVertex(edge.dst),
            edge, particle, rng);
    }

    // Empty graph => do nothing
}

// Get uniformly random edge adjacent to the specified vertex
NavigationGraph::Edge getRandomAdjacentEdge(
    const NavigationGraph::Vertex& v,
    const NavigationGraph& graph,
    Rng* rng)
{
    const auto edges = graph.getAdjacentEdges(v.id);
    if (edges.empty()) {
        return NavigationGraph::Edge{};
    }

    const int index = rng->discrete(0, edges.size() - 1);
    return edges[index];
}

//TODO mutate direction not only for edges!!!
void moveParticle(const NavigationGraph& graph, GraphParticle& p, double dist, Rng* rng)
{
    const auto maxIters = 10;

    for(size_t iter = 0; iter < maxIters; ++iter) {
        const auto src = graph.getVertex(p.fromVertex);
        const auto dst = graph.getVertex(p.toVertex);

        double edgeLen = GetDist(src.point, dst.point);
        double passedDist = edgeLen * p.percent;
        double remainingDist = edgeLen - passedDist;

        if (dist < remainingDist) {
            p.percent = (passedDist + dist) / edgeLen;
            p.point.x = src.point.x * (1.0 - p.percent) + dst.point.x * p.percent;
            p.point.y = src.point.y * (1.0 - p.percent) + dst.point.y * p.percent;
            p.angle = getAngle(src, dst);
            break;
        }

        dist -= remainingDist;

        auto nextEdge = getRandomAdjacentEdge(dst, graph, rng);

        // Decrease probability of the particle to turn back
        if (nextEdge.id == p.edgeId) {
            nextEdge = getRandomAdjacentEdge(dst, graph, rng);
        }

        p.percent = 0;
        p.edgeId = nextEdge.id;
        p.fromVertex = nextEdge.src == p.toVertex ? nextEdge.src : nextEdge.dst;
        p.toVertex = nextEdge.src == p.toVertex ? nextEdge.dst : nextEdge.src;
        p.angle = getAngle(
            graph.getVertex(p.fromVertex),
            graph.getVertex(p.toVertex));
    }
}

} // namespace

///
/// GraphParticleFilter
///

GraphParticleFilter::GraphParticleFilter(
    const std::shared_ptr<const Level>& level,
    const std::shared_ptr<Rng>& rng,
    const PositionEstimatorSettings::ParticleFilter& pfSettings)
    : graph_(createNavigationGraph(level->graph()))
    , numParticles_(pfSettings.numParticles.value_or(PositionEstimatorSettings::ParticleFilter::DEFAULT_NUM_PARTICLES))
    , numParticlesToMutate_(pfSettings.numParticlesToMutateRate.value_or(PositionEstimatorSettings::ParticleFilter::DEFAULT_NUM_PARTICLES_TO_MUTATE_RATE) * numParticles_)
    , numParticlesAroundClosestTxs_(pfSettings.numParticlesAroundClosestTxsRate.value_or(PositionEstimatorSettings::ParticleFilter::DEFAULT_NUM_PARTICLES_AROUND_CLOSEST_TXS_RATE) * numParticles_)
    , numParticlesAliveThreshold_(pfSettings.numParticlesAliveThresholdRate.value_or(PositionEstimatorSettings::ParticleFilter::DEFAULT_NUM_PARTICLES_ALIVE_THRESHOLD_RATE) * numParticles_)
    , resamplingThreshold_(pfSettings.resamplingThresholdRate.value_or(PositionEstimatorSettings::ParticleFilter::DEFAULT_RESAMPLING_THRESHOLD_RATE) * numParticles_)
    , minDeviation_(pfSettings.minDeviation.value_or(PositionEstimatorSettings::ParticleFilter::DEFAULT_MIN_DEVIATION))
    , level_(level)
    , rng_(rng)
    , likelihood_(createLikelihood(level_.get(), pfSettings))
    , particles_(numParticles_)
    , initialized_(false)
{}

RadioMeasurementsData GraphParticleFilter::getRadioMeasurements() const
{
    return likelihood_->getRadioMeasurements();
}

LocationMeasurementsData GraphParticleFilter::getLocationMeasurements() const
{
    return likelihood_->getLocationMeasurements();
}

std::vector<GeoParticle> GraphParticleFilter::getParticles() const
{
    if (!initialized_) {
        return {};
    }

    return convertParticles(particles_, level_->bindingPoint());
}

std::optional<SolutionPF> GraphParticleFilter::getSolution() const
{
    if (!initialized_) {
        return std::nullopt;
    }

    return getSolutionFromParticles(particles_, minDeviation_);
}

bool GraphParticleFilter::performEstimation(
    const RadioMeasurementsData& radioMsrs,
    const LocationMeasurementsData& locMsrs,
    const MotionInfo& motionInfo)
{
    likelihood_->setMeasurements(radioMsrs, locMsrs);

    if (initialized_) {
        move(motionInfo);
    }

    if (!initialized_ || calculateAliveParticles(particles_) < numParticlesAliveThreshold_) {
        if (likelihood_->hasSufficientMeasurementsForInit()) {
            init();
            return true;
        }
        return false;
    }

    bool updated = false;

    if (likelihood_->hasSufficientMeasurementsForUpdate()) {
        if (likelihood_->hasSufficientMeasurementsForMutation()) {
            mutate();
        }

        update();
        updated = true;
    }

    resample();
    return updated;
}

void GraphParticleFilter::reset()
{
    if (!initialized_) {
        return;
    }

    initialized_ = false;

    for(auto& p : particles_) {
        p.weight = 0.0;
    }
}

void GraphParticleFilter::init()
{
    particles_ = getSampleParticles(particles_.size());
    updateParticlesWeights(particles_, likelihood_.get());
    initialized_ = true;
}

void GraphParticleFilter::update()
{
    if (!initialized_) {
        return;
    }

    updateParticlesWeights(particles_, likelihood_.get());
}

void GraphParticleFilter::mutate()
{
    if (!initialized_) {
        return;
    }

    const auto sampleParticles = getSampleParticles(numParticlesToMutate_);

    for(const auto& p : sampleParticles) {
        auto index = rng_->discrete(0, particles_.size() - 1);
        particles_[index] = p;
    }

    const auto samplingCenters = likelihood_->calculateSamplingCenters();
    if (samplingCenters.empty()) {
        return;
    }

    const auto numParticlesAroundBestPoint = numParticlesAroundClosestTxs_ / samplingCenters.size();

    for(const auto& bestPoint : samplingCenters) {
        const auto e = getClosestEdge(graph_, bestPoint);

        for(size_t i = 0; i < numParticlesAroundBestPoint; i++) {
            auto index = rng_->discrete(0, particles_.size() - 1);
            auto& particle = particles_[index];
            const auto src = graph_.getVertex(e.src);
            const auto dst = graph_.getVertex(e.dst);
            sampleParticleOnEdge(src, dst, e, particle, rng_.get());

            // Reinitializing particle's weight
            particle.weight = 1.0 / particles_.size();
        }
    }
}

void GraphParticleFilter::resample()
{
    if (!initialized_) {
        return;
    }

    if (needResampling(particles_, resamplingThreshold_)) {
        resampleParticles(particles_, rng_.get());
    }
}

void GraphParticleFilter::move(const MotionInfo& motionInfo)
{
    if (!initialized_) {
        return;
    }

    for(auto& p : particles_) {
        //consider deviation as mean
        double dist = std::fabs(rng_->normal(0, motionInfo.distanceDeviation));
        moveParticle(graph_, p, dist, rng_.get());
    }
}

std::vector<GraphParticle> GraphParticleFilter::getSampleParticles(size_t sampleSize) const
{
    std::vector<GraphParticle> sampleParticles(sampleSize * SAMPLE_SIZE_MULTIPLIER);
    for(auto& particle : sampleParticles) {
        sampleParticleOnGraph(graph_, particle, rng_.get());
        particle.weight = likelihood_->getLogWeight(particle.point);
    }

    std::stable_sort(sampleParticles.begin(), sampleParticles.end(),
        [](const auto& p1, const auto& p2) {
            return p1.weight > p2.weight;
        });

    sampleParticles.resize(sampleSize);

    for(auto& p : sampleParticles) {
        p.weight = 1.0 / particles_.size();
    }

    return sampleParticles;
}

} // namespace navigine::navigation_core
