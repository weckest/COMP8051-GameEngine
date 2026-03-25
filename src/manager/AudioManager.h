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
    static std::unordered_map<std::string, MIX_Audio*> audio;

public:
    AudioManager();

    void loadAudio(const std::string& name, const char* path) const;

    void playMusic(const std::string& name) const;
    void stopMusic() const;

    static void playSfx(const std::string& name);
};

#endif //INC_8051TUTORIAL_AUDIOMANAGER_H