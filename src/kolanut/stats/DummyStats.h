#pragma once

#include "kolanut/core/Types.h"
#include "kolanut/stats/StatsEngine.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>

namespace kola {
namespace stats {

class DummyStats : public StatsEngine
{
public:
    void init(const Config& config) override {}

    void startTimeSample(size_t m) override {}
    void endTimeSample(size_t m) override {}

    void addSample(size_t m, double value) override {}
    void addToCurrentSample(size_t m, double value) override {}

    bool hasResult(size_t m) const override
    { return false; }

    Result getResult(size_t m) const override 
    { return {}; }

    const std::string& getLabel(size_t m) const override
    {   
        static const std::string emptyLabel = "";
        return emptyLabel;
    }

    void addLabel(size_t m, const std::string& label) override {}
    void reset() override {}
    void reset(size_t m) override {}

    const Config& getConfig() const override
    { 
        static const Config emptyConf = {};
        return emptyConf; 
    }
};

} // namespace stats
} // namespace kola