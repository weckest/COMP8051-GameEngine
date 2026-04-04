//
// Created by Miguel on 2026-03-25.
//

#ifndef INC_8051TUTORIAL_AUDIOMANAGER_H
#define INC_8051TUTORIAL_AUDIOMANAGER_H

#include <string>
#include <unordered_map>
#include <vector>

#include "SDL3_mixer/SDL_mixer.h"

class AudioManager
{
    MIX_Mixer *mixer = nullptr;
    static MIX_Track *musicTrack;
    static MIX_Track *sfxTrack;
    static MIX_Track *weaponSfxTrack;
    static MIX_Track *damageSfxTrack;
    static std::unordered_map<std::string, MIX_Audio*> audio;

    std::vector<float> trackVolumes;

public:
    AudioManager();

    void loadAudio(const std::string& name, const char* path) const;

    void playMusic(const std::string& name);
    void stopMusic() const;

    static void playSfx(const std::string& name);
    static void playWeaponSfx(const std::string& name);
    static void playDamageSfx(const std::string& name);

    static void getVolume(int track);
    static void updateVolume(int track, float volume);
private:
    std::string currentTrack;

};

#endif //INC_8051TUTORIAL_AUDIOMANAGER_H