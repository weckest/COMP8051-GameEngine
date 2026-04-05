//
// Created by Miguel on 2026-03-25.
//

#include "AudioManager.h"

#include <iostream>
#include <ostream>

MIX_Track* AudioManager::sfxTrack;
MIX_Track* AudioManager::weaponSfxTrack;
MIX_Track* AudioManager::damageSfxTrack;
MIX_Track* AudioManager::musicTrack;

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
    weaponSfxTrack = MIX_CreateTrack(mixer);
    damageSfxTrack = MIX_CreateTrack(mixer);
    MIX_SetTrackGain(musicTrack, 0.8f);
    MIX_SetTrackGain(sfxTrack, 0.6f);
    MIX_SetTrackGain(weaponSfxTrack, 0.3f);
    MIX_SetTrackGain(damageSfxTrack, 0.5f);
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

void AudioManager::playMusic(const std::string& name)
{
    if (name == currentTrack) return; //playing same track

    if (MIX_SetTrackAudio(musicTrack, audio[name]) == 0) {
        std::cout << "MIX_SetTrackAudio() failed" << std::endl;
        return;
    }

    SDL_PropertiesID props = SDL_CreateProperties();
    SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, -1);

    MIX_PlayTrack(musicTrack, props); //loop endlessly
    std::cout << "Playing Music: " << name << std::endl;

    SDL_DestroyProperties(props);
    currentTrack = name;

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
    //std::cout << "Playing SFX: " << name << std::endl;
    //commented because of spam
}

//to handle different audio levels for the amount of sounds playing
void AudioManager::playWeaponSfx(const std::string& name)
{
    if (MIX_SetTrackAudio(weaponSfxTrack, audio[name]) == 0)
    {
        std::cout << "MIX_SetTrackAudio() failed" << std::endl;
        return;
    }

    MIX_PlayTrack(weaponSfxTrack, 0);
    //std::cout << "Playing SFX: " << name << std::endl;
    //commented because of spam
}

void AudioManager::playDamageSfx(const std::string& name)
{
    if (MIX_SetTrackAudio(damageSfxTrack, audio[name]) == 0)
    {
        std::cout << "MIX_SetTrackAudio() failed" << std::endl;
        return;
    }

    MIX_PlayTrack(damageSfxTrack, 0);
    //std::cout << "Playing SFX: " << name << std::endl;
    //commented because of spam
}

float AudioManager::getVolume(int track)
{
    switch (track)
    {
    case 0:
        return MIX_GetTrackGain(musicTrack);
    case 1:
        return MIX_GetTrackGain(sfxTrack);
    case 2:
        return MIX_GetTrackGain(weaponSfxTrack);
    case 3:
        return MIX_GetTrackGain(damageSfxTrack);
    default:
        std::cout << "MIX_GetTrackGain() failed" << std::endl;
        break;
    }
    return 0.0f;
}

void AudioManager::updateVolume(int track, float volume)
{
    switch (track)
    {
        case 0:
            MIX_SetTrackGain(musicTrack, volume);
            break;
        case 1:
            MIX_SetTrackGain(sfxTrack, volume);
            break;
        case 2:
            MIX_SetTrackGain(weaponSfxTrack, volume);
            break;
        case 3:
            MIX_SetTrackGain(damageSfxTrack, volume);
            break;
        default:
            std::cout << "MIX_SetTrackGain() failed" << std::endl;
            break;
    }
}
