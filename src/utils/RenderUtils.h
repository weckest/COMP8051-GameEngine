//
// Created by BCIT on 2026-03-11.
//

#ifndef INC_8051TUTORIAL_RENDERUTILS_H
#define INC_8051TUTORIAL_RENDERUTILS_H
#include <SDL3/SDL_rect.h>

class RenderUtils {
public:
	//scaling from center plot
	static SDL_FRect getScaledDest(const SDL_FRect& dst, float scale) {

		return SDL_FRect{
			dst.x + dst.w *  (1-scale)/2 ,
			dst.y + dst.h *  (1-scale)/2 ,
			dst.w * scale,
			dst.h * scale
		};
	}
};

#endif //INC_8051TUTORIAL_RENDERUTILS_H