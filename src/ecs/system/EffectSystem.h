//
// Created by Weckest on 2026-02-27.
//

#ifndef INC_8051TUTORIAL_EFFECTSYSTEM_H
#define INC_8051TUTORIAL_EFFECTSYSTEM_H
#include <memory>
#include <vector>

#include "Component.h"
#include "Entity.h"

class EffectSystem {
public:
    void update(std::vector<std::unique_ptr<Entity>>& entities, float dt) {
        for (auto& e : entities) {
            if (e->hasComponent<Animation>() && e->hasComponent<Transform>() && e->hasComponent<EffectTag>()) {
                auto& anim = e->getComponent<Animation>();
                auto& transform = e->getComponent<Transform>();

                //playback system: advance the animation
                float animFrameSpeed = anim.speed; //how long each animation frame should last
                auto clip = anim.clips[anim.currentClip]; //retrieve the frame data from the current clip
                //advance time
                anim.time += dt; //every game loop frame

                //once enough time has passed
                if (anim.time >= animFrameSpeed) {
                    anim.time -= animFrameSpeed; //subtract animFrameSpeed (e.g. 0.1f) so the extra time isn't lost

                    std::size_t totalAnimationFrames = clip.frameIndices.size();

                    //advance to the next animation frame (currentAnimationFrame + 1)
                    //wrap around % so it loops when reaching the end of the clip
                    anim.currentFrame = (anim.currentFrame + 1) % totalAnimationFrames;

                    if (anim.currentFrame == 0) {
                        e->destroy();
                    }
                }
            }
        }
    }
};

#endif //INC_8051TUTORIAL_EFFECTSYSTEM_H