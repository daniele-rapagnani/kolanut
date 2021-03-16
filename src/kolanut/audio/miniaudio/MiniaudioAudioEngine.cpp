#include "kolanut/core/Logging.h"
#include "kolanut/core/DIContainer.h"
#include "kolanut/audio/miniaudio/MiniaudioAudioEngine.h"
#include "kolanut/audio/miniaudio/MiniaudioSoundInstance.h"
#include "kolanut/filesystem/FilesystemEngine.h"
#include "kolanut/stats/StatsEngine.h"

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

#include <algorithm>
#include <chrono>
#include <limits>

#include <immintrin.h>
#include <emmintrin.h>

namespace kola {
namespace audio {

namespace {

class MiniaudioSoundInstanceEX : public MiniaudioSoundInstance
{
public:
    void setFinished(bool finished)
    {
        MiniaudioSoundInstance::setFinished(finished);
    }
};

inline float sampleToFloat(int16_t s)
{
    return static_cast<float>(s) / static_cast<float>(std::numeric_limits<int16_t>::max());
}

inline int16_t sampleToInt16(float sample)
{
    return (sample * std::numeric_limits<int16_t>::max());
}

void dataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    static std::vector<int16_t> tempBuffer = {};

    auto mae = reinterpret_cast<MiniaudioAudioEngine*>(pDevice->pUserData);
    auto playingSounds = mae->getPlayingSounds();

    mae->getPlayingMutex().lock();

    auto startTime = std::chrono::high_resolution_clock::now();

    if (playingSounds.empty())
    {
        mae->getPlayingMutex().unlock();
        return;
    }

    size_t samplesCount = frameCount * MiniaudioAudioEngine::NUM_CHANNELS;

    if (tempBuffer.size() < samplesCount)
    {
        tempBuffer.resize(samplesCount);
    }

    for (auto& si : playingSounds)
    {
        auto ps = std::static_pointer_cast<MiniaudioSoundInstanceEX>(si);

        if (!ps || !ps->isPlaying())
        {
            continue;
        }

        if (ps->shouldRestart())
        {
            ma_decoder_seek_to_pcm_frame(ps->getDecoder(), 0);
            ps->setRestart(false);
        }
        
        uint64_t framesRead = ma_decoder_read_pcm_frames(ps->getDecoder(), tempBuffer.data(), frameCount);
        
        if (framesRead < frameCount)
        {
            if (ps->isLooping())
            {
                ps->restart();
            }
            else
            {
                ps->setFinished(true);
            }
        }
        
        if (framesRead > 0)
        {
            int16_t* samples = reinterpret_cast<int16_t*>(pOutput);
            const auto& gains = ps->getGains();

            for (
                size_t i = 0;
                i < framesRead * MiniaudioAudioEngine::NUM_CHANNELS; 
                i += MiniaudioAudioEngine::NUM_CHANNELS
            )
            {
                samples[i] += sampleToInt16(sampleToFloat(tempBuffer[i]) * gains[1]);
                samples[i + 1] += sampleToInt16(sampleToFloat(tempBuffer[i + 1]) * gains[0]);
            }
        }
    }

    mae->getPlayingMutex().unlock();

    uint64_t elapsed = 
        std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now() - startTime
        ).count()
    ;

    di::get<stats::StatsEngine>()->enqueueSample(
        stats::StatsEngine::Measure::AUDIO_PROC,
        elapsed / 1000.0f,
        true
    );
}

} // namespace

MiniaudioAudioEngine::~MiniaudioAudioEngine()
{
    knM_logDebug("Stopping Miniaudio engine");

    ma_device_stop(&this->device);

    for (auto ps : this->playing)
    {
        ma_decoder_uninit(
            std::static_pointer_cast<MiniaudioSoundInstance>(ps)->getDecoder()
        );
    }

    this->playing.clear();

    ma_device_uninit(&this->device);
}

bool MiniaudioAudioEngine::init(const Config& config)
{
    knM_logDebug("Initializing Miniaudio");
    
    ma_device_config deviceConfig = {};

    deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format = ma_format_s16;
    deviceConfig.playback.channels = NUM_CHANNELS;
    deviceConfig.sampleRate = SAMPLE_RATE;
    deviceConfig.dataCallback = dataCallback;
    deviceConfig.pUserData = this;

    if (ma_device_init(NULL, &deviceConfig, &this->device) != MA_SUCCESS) 
    {
        knM_logError("Can't initialize playback device");
        return false;
    }

    if (ma_device_start(&this->device) != MA_SUCCESS) 
    {
        knM_logError("Failed to start playback device.");
        return false;
    }

    return true;
}

std::shared_ptr<Sound> MiniaudioAudioEngine::loadSound(const std::string& fileName)
{
    knM_logDebug("Loading sound file: " << fileName);

    std::shared_ptr<MiniaudioSound> sound = std::make_shared<MiniaudioSound>(fileName);

    if (!di::get<filesystem::FilesystemEngine>()->getFileContent(fileName, sound->getData()))
    {
        knM_logError("Can't find sound: " << fileName);
        return {};
    }

    return sound;
}

std::shared_ptr<SoundInstance> MiniaudioAudioEngine::playSound(
    std::shared_ptr<Sound> sample,
    float gain /* = 1.0f */,
    float pan /* = 0.0f */
)
{
    if (!sample)
    {
        return nullptr;
    }

    getPlayingMutex().lock();
    
    std::remove_if(this->playing.begin(), this->playing.end(), [] (std::shared_ptr<SoundInstance>& ps) {
        return !ps || ps->isFinished();
    });

    auto ps = std::make_shared<MiniaudioSoundInstance>();
    ps->setFinished(false);
    ps->setSound(sample);
    ps->setGain(gain);
    ps->setPan(pan);

    ma_decoder_config dc = {};
    dc.channels = NUM_CHANNELS;
    dc.format = ma_format_s16;

    auto miniSound = std::static_pointer_cast<MiniaudioSound>(sample);

    ma_result result = ma_decoder_init_memory(
        miniSound->getData().data(), 
        miniSound->getData().size(), 
        &dc, 
        ps->getDecoder()
    );

    if (result != MA_SUCCESS)
    {
        knM_logError("Can't decode sound: " << sample->getFilename());
    }

    auto si = std::static_pointer_cast<SoundInstance>(ps);
    this->playing.push_back(si);

    getPlayingMutex().unlock();

    return si;
}

void MiniaudioAudioEngine::setMasterVolume(float volume)
{
    ma_device_set_master_volume(&this->device, volume);
}

float MiniaudioAudioEngine::getMasterVolume()
{
    float volume = {};
    ma_device_get_master_volume(&this->device, &volume);

    return volume;
}

} // namespace audio
} // namespace kola