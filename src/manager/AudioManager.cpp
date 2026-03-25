//
// Created by Miguel on 2026-03-25.
//

#include "AudioManager.h"

#include <iostream>
#include <ostream>

MIX_Track* AudioManager::sfxTrack;
std::unordered_map<std::string, MIX_Audio*> AudioManager::audio;

AudioManager::AudioManager()
{
    if (MIX_Init() == 0)
    {
        std::cout << "MIX_Init() failed" << std::endl;
        return;
    }

    mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
    if (!mixer)
    {
        std::cout << "MIX_CreateMixerDevice() failed" << std::endl;
        return;
    }

    musicTrack = MIX_CreateTrack(mixer);
    sfxTrack = MIX_CreateTrack(mixer);
    MIX_SetTrackGain(musicTrack, 0.8f);
    MIX_SetTrackGain(sfxTrack, 0.8f);
}

void AudioManager::loadAudio(const std::string& name, const char* path) const
{
    if (audio.contains(path))
    {
        return;
    }

    auto audioPtr = MIX_LoadAudio(mixer, path, true);
    if (!audioPtr)
    {
        std::cout << "MIX_LoadAudio() failed" << std::endl;
        return;
    }
    audio.emplace(name, audioPtr);
}

void AudioManager::playMusic(const std::string& name) const
{
    if (MIX_SetTrackAudio(musicTrack, audio[name]) == 0)
    {
        std::cout << "MIX_SetTrackAudio() failed" << std::endl;
        return;
    }
    MIX_PlayTrack(musicTrack, -1); //loop endlessly
    std::cout << "Playing Music" << std::endl;
}

void AudioManager::stopMusic() const
{
    MIX_StopTrack(musicTrack, 0);
}

void AudioManager::playSfx(const std::string& name)
{
    if (MIX_SetTrackAudio(sfxTrack, audio[name]) == 0)
    {
        std::cout << "MIX_SetTrackAudio() failed" << std::endl;
        return;
    }

    MIX_PlayTrack(sfxTrack, 0);
    std::cout << "Playing SFX " << name << std::endl;
}
