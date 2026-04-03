//
// Created by Miguel on 2026-03-25.
//

#ifndef INC_8051TUTORIAL_AUDIOMANAGER_H
#define INC_8051TUTORIAL_AUDIOMANAGER_H

#include <string>
#include <unordered_map>

#include "SDL3_mixer/SDL_mixer.h"

class AudioManager
{
    MIX_Mixer *mixer = nullptr;
    MIX_Track *musicTrack = nullptr;
    static MIX_Track *sfxTrack;
    static MIX_Track *weaponSfxTrack;
    static MIX_Track *damageSfxTrack;
    static std::unordered_map<std::string, MIX_Audio*> audio;

public:
    AudioManager();

    void loadAudio(const std::string& name, const char* path) const;

    void playMusic(const std::string& name);
    void stopMusic() const;

    static void playSfx(const std::string& name);
    static void playWeaponSfx(const std::string& name);
    static void playDamageSfx(const std::string& name);
private:
    std::string currentTrack;

};

#endif //INC_8051TUTORIAL_AUDIOMANAGER_H