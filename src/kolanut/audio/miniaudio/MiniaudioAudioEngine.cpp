#include "kolanut/core/Logging.h"
#include "kolanut/core/DIContainer.h"
#include "kolanut/audio/miniaudio/MiniaudioAudioEngine.h"
#include "kolanut/filesystem/FilesystemEngine.h"

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

#include <algorithm>
#include <limits>

namespace kola {
namespace audio {

namespace {

void dataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    static std::vector<int16_t> tempBuffer = {};

    auto mae = reinterpret_cast<MiniaudioAudioEngine*>(pDevice->pUserData);
    auto playingSounds = mae->getPlayingSounds();

    mae->getPlayingMutex().lock();

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

    for (MiniaudioAudioEngine::PlayingSound& ps : playingSounds)
    {
        if (ps.finished)
        {
            continue;
        }
        
        uint64_t framesRead = ma_decoder_read_pcm_frames(&ps.decoder, tempBuffer.data(), frameCount);
        ps.finished = framesRead < frameCount;

        if (framesRead > 0)
        {
            int16_t* samples = reinterpret_cast<int16_t*>(pOutput);

            for (
                size_t i = 0; 
                i < framesRead * MiniaudioAudioEngine::NUM_CHANNELS; 
                i += MiniaudioAudioEngine::NUM_CHANNELS
            )
            {
                samples[i] += tempBuffer[i] / ps.gainDiv[1];
                samples[i + 1] += tempBuffer[i + 1] / ps.gainDiv[0];
            }
        }
    }

    mae->getPlayingMutex().unlock();
}

} // namespace

void MiniaudioAudioEngine::PlayingSound::setGain(float gain, float pan)
{
    assert(NUM_CHANNELS == 2);

    // Back to 0 hard left, 1 hard right
    pan = (pan + 1.0f) / 2.0f;

    float gainL = std::max(
        std::numeric_limits<float>::epsilon(),
        (gain * (1.0f - pan))
    );

    float gainR = std::max(
        std::numeric_limits<float>::epsilon(),
        (gain * pan)
    );

    this->gainDiv[0] = std::max(
        static_cast<int16_t>(1), 
        static_cast<int16_t>(round(1.0f / gainL))
    );

    this->gainDiv[1] = std::max(
        static_cast<int16_t>(1), 
        static_cast<int16_t>(round(1.0f / gainR))
    );
}

MiniaudioAudioEngine::~MiniaudioAudioEngine()
{
    knM_logDebug("Stopping Miniaudio engine");

    ma_device_stop(&this->device);

    for (PlayingSound& ps : this->playing)
    {
        ma_decoder_uninit(&ps.decoder);
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

    std::shared_ptr<MiniaudioSound> sound = std::make_shared<MiniaudioSound>();

    if (!di::get<filesystem::FilesystemEngine>()->getFileContent(fileName, sound->data))
    {
        knM_logError("Can't find sound: " << fileName);
        return {};
    }

    sound->fileName = fileName;

    return sound;
}

void MiniaudioAudioEngine::playSound(
    std::shared_ptr<Sound> sample,
    float gain /* = 1.0f */,
    float pan /* = 0.0f */
)
{
    if (!sample)
    {
        return;
    }

    getPlayingMutex().lock();
    
    std::remove_if(this->playing.begin(), this->playing.end(), [] (PlayingSound& ps) {
        return ps.finished;
    });

    this->playing.emplace_back();

    PlayingSound& ps = this->playing.back();
    ps.sound = std::static_pointer_cast<MiniaudioSound>(sample);
    ps.setGain(gain, pan);

    ma_result result = ma_decoder_init_memory(
        ps.sound->data.data(), 
        ps.sound->data.size(), 
        nullptr, 
        &ps.decoder
    );

    if (result != MA_SUCCESS)
    {
        this->playing.pop_back();
        knM_logError("Can't decode sound: " << ps.sound->fileName);
    }

    getPlayingMutex().unlock();
}

} // namespace audio
} // namespace kola