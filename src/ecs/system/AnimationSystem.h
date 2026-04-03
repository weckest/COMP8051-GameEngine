//
// Created by Weckest on 2026-02-18.
//

#ifndef INC_8051TUTORIAL_ANIMATIONSYSTEM_H
#define INC_8051TUTORIAL_ANIMATIONSYSTEM_H
#include <memory>
#include <vector>

#include "AnimationClip.h"
#include "Component.h"
#include "Entity.h"

//State system: deciding which clip to use
//Check if the animation has been switched
//playback system: advances the animation

class AnimationSystem {
public:
    void update(const std::vector<std::unique_ptr<Entity>>& entities, float dt) {
        for (auto& e : entities) {
            if (e->hasComponent<Animation>() && e->hasComponent<Velocity>()) {
                auto& anim = e->getComponent<Animation>();
                auto& velocity = e->getComponent<Velocity>();

                //state system
                std::string newClip;

                if (velocity.direction.x != 0.0f) {
                    newClip = (velocity.direction.x > 0.0f) ? "walk_right" : "walk_left";
                } else if (velocity.direction.x == 0.0f && velocity.direction.y != 0.0f) {
                    //player is only moving north/south, use facing data
                    newClip = (velocity.facingRight) ? "walk_right" : "walk_left";
                } else {
                    newClip = (velocity.facingRight) ? "idle_right" : "idle_left";
                }


                // if (velocity.direction.x > 0.0f) {
                //     newClip = "walk_right";
                // } else if (velocity.direction.x < 0.0f) {
                //     newClip = "walk_left";
                // } else if (velocity.direction.y > 0.0f) {
                //     newClip = "walk_down";
                // } else if (velocity.direction.y < 0.0f) {
                //     newClip = "walk_up";
                // } else {
                //     newClip = "idle_down";
                // }

                //check if the animation has switched
                //if the chosen clip is different from the current one, switch to new clip
                //reset time and frame index
                if (newClip != anim.currentClip) {
                    anim.currentClip = newClip; //switch to new clip
                    anim.time = 0.0f; //reset time to 0
                    anim.currentFrame = 0; //reset frame index to 0
                }

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
                }

            }
        }
    }
};

#endif //INC_8051TUTORIAL_ANIMATIONSYSTEM_H