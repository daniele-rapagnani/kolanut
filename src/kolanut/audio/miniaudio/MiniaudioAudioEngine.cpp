#include "kolanut/core/Logging.h"
#include "kolanut/core/DIContainer.h"
#include "kolanut/audio/miniaudio/MiniaudioAudioEngine.h"
#include "kolanut/filesystem/FilesystemEngine.h"

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

namespace kola {
namespace audio {

namespace {

void dataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    auto mae = reinterpret_cast<MiniaudioAudioEngine*>(pDevice->pUserData);
    
    if (mae->getPlayingSounds().empty())
    {
        return;
    }

    auto s = mae->getPlayingSounds().back();
    
    if (ma_decoder_read_pcm_frames(&s->decoder, pOutput, frameCount) < frameCount)
    {
        //mae->getPlayingSounds().pop_back();
    }
}

} // namespace

bool MiniaudioAudioEngine::init(const Config& config)
{
    knM_logDebug("Initializing Miniaudio");
    
    ma_device_config deviceConfig = {};

    deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format = ma_format_s16;
    deviceConfig.playback.channels = 2;
    deviceConfig.sampleRate = 44100;
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

    ma_result result = ma_decoder_init_memory(
        sound->data.data(), 
        sound->data.size(), 
        nullptr, 
        &sound->decoder
    );

    if (result != MA_SUCCESS)
    {
        knM_logError("Can't decode sound: " << fileName);
        return {};
    }

    return sound;
}

void MiniaudioAudioEngine::playSound(std::shared_ptr<Sound> sample)
{
    if (!sample)
    {
        return;
    }

    auto s = std::static_pointer_cast<MiniaudioSound>(sample);
    ma_decoder_seek_to_pcm_frame(&s->decoder, 0);

    this->playing.push_back(s);
}

} // namespace audio
} // namespace kola