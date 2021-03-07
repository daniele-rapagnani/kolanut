#include "kolanut/stats/Stats.h"

#include <cassert>

namespace kola {
namespace stats {

void Stats::init(const Config& config)
{
    this->config = config;

    addLabel(Measure::CPU_TIME, "CPU_TIME");
    addLabel(Measure::FRAME_TIME, "FRAME_TIME");
    addLabel(Measure::GPU_TIME, "GPU_TIME");
    addLabel(Measure::TRIANGLES, "TRIANGLES");
    addLabel(Measure::UPDATE_TIME, "UPDATE_TIME");
    addLabel(Measure::DRAW_TIME, "DRAW_TIME");
    addLabel(Measure::AUDIO_PROC, "AUDIO_PROC");
}

void Stats::enqueueSample(size_t m, double value, bool addToCurrent /* = false */)
{
    this->addSampleTasksMutex.lock();
    this->addSampleTasks.emplace_back(m, value, addToCurrent);
    this->addSampleTasksMutex.unlock();
}

void Stats::processEnqueued()
{
    this->addSampleTasksMutex.lock();
    
    for (const AddSampleTask& t : this->addSampleTasks)
    {
        if (t.addToCurrent)
        {
            addToCurrentSample(t.measure, t.value);
        }
        else
        {
            addSample(t.measure, t.value);
        }
    }

    this->addSampleTasks.clear();

    this->addSampleTasksMutex.unlock();
}

void Stats::addSample(size_t measure, double value)
{
    std::vector<double>& data = this->samples[measure];

    data.push_back(value);

    if (data.size() == getConfig().samplesPerPeriod)
    {
        calcResult(measure);
        reset(measure);
    }
}

void Stats::addToCurrentSample(size_t m, double value)
{
    std::vector<double>& data = this->samples[m];
    
    if (data.empty())
    {
        addSample(m, value);
        return;
    }

    data.back() += value;
}

void Stats::startTimeSample(size_t m)
{
    this->timePoints[m] = std::chrono::high_resolution_clock::now();
}

void Stats::endTimeSample(size_t m)
{
    auto it = this->timePoints.find(m);
    assert(it != this->timePoints.end());

    uint64_t elapsed = 
        std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now() - it->second
        ).count()
    ;

    this->timePoints.erase(it);
    addSample(m, static_cast<double>(elapsed) / 1000.0);
}

Stats::Result Stats::getResult(size_t m) const
{
    if (!hasResult(m))
    {
        return {};
    }

    return this->results.at(m);
}

void Stats::calcResult(size_t measure)
{
    if (this->samples.find(measure) == this->samples.end())
    {
        return;
    }

    const std::vector<double>& samples = this->samples[measure];

    Result& r = this->results[measure];
    r = {};

    r.measure = measure;
    r.samplesCount = samples.size();
    r.label = &getLabel(measure);
    
    for (double d : samples)
    {
        r.total += d;
    }

    r.avg = r.total / r.samplesCount;

    if (getConfig().resultCb)
    {
        getConfig().resultCb(r);
    }
}

} // namespace stats
} // namespace kola