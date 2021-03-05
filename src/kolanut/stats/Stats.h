#pragma once

#include "kolanut/core/Types.h"
#include "kolanut/stats/StatsEngine.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>

namespace kola {
namespace stats {

class Stats : public StatsEngine
{
private:
    using SamplesDb = std::unordered_map<size_t, std::vector<double>>;

public:
    void init(const Config& config) override;

    void startTimeSample(size_t m) override;
    void endTimeSample(size_t m) override;

    void addSample(size_t m, double value) override;
    void addToCurrentSample(size_t m, double value) override;

    bool hasResult(size_t m) const override
    {
        return 
            this->results.find(m) != this->results.end() 
            && this->results.at(m)
        ;
    }

    Result getResult(size_t m) const override;

    const std::string& getLabel(size_t m) const override
    {   
        static const std::string emptyLabel = "";

        if (this->measuresLabels.find(m) == this->measuresLabels.end())
        {
            return emptyLabel;
        }

        return this->measuresLabels.at(m);
    }

    void addLabel(size_t m, const std::string& label) override
    {
        this->measuresLabels[m] = label;
    }

    void reset() override
    { this->samples.clear(); }

    void reset(size_t m) override
    { this->samples[m].clear(); }

    const Config& getConfig() const override
    { return this->config; }

protected:
    void calcResult(size_t measure);

private:
    Config config = {};
    SamplesDb samples = {};
    
    std::unordered_map<
        size_t, 
        std::chrono::time_point<std::chrono::high_resolution_clock>
    > timePoints = {};

    std::unordered_map<size_t, std::string> measuresLabels = {};
    std::unordered_map<size_t, Result> results = {};
};

} // namespace stats
} // namespace kola