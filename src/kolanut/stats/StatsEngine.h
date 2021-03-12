#pragma once

#include "kolanut/core/Types.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <chrono>

namespace kola {
namespace stats {

class StatsEngine
{
public:
    enum Measure
    {
        NONE = 0,
        FRAME_TIME,
        CPU_TIME,
        GPU_TIME,
        TRIANGLES,
        UPDATE_TIME,
        DRAW_TIME,
        AUDIO_PROC,
        BATCHED_DRAWS,
        DRAW_CALLS,
        MAX_BUILTIN
    };

    struct Result
    {
        size_t measure = {};
        double total = 0.0;
        double avg = 0.0;
        size_t samplesCount = 0;
        const std::string* label = {};

        operator bool() const
        {
            return measure != Measure::NONE;
        }
    };

    struct Config
    {
        size_t samplesPerPeriod = 60;
        std::function<void(const Result&)> resultCb = {};
    };

public:
    virtual void init(const Config& config) = 0;

    virtual void startTimeSample(size_t m) = 0;
    virtual void endTimeSample(size_t m) = 0;

    virtual void addSample(size_t m, double value) = 0;
    virtual void addToCurrentSample(size_t m, double value) = 0;
    virtual void enqueueSample(size_t m, double value, bool addToCurrent = false) = 0;

    virtual void processEnqueued() = 0;

    virtual bool hasResult(size_t m) const = 0;

    virtual Result getResult(size_t m) const = 0;

    virtual const std::string& getLabel(size_t m) const = 0;

    virtual void addLabel(size_t m, const std::string& label) = 0;
    virtual void reset() = 0;

    virtual void reset(size_t m) = 0;

    virtual const Config& getConfig() const = 0;
};

} // namespace stats
} // namespace kola