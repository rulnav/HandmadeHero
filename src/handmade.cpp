// #ifndef HANDMADE_CPP
// #define HANDMADE_CPP
#include "../headers/handmade.hpp"
#include "stdint.h"
/*
 * NOTE: Services that the game provides to the platform layer.
*/
// player input, bitmap buffer to use, sound buffer to use
static void GradientRenderer(game_offscreen_buffer &buffer, int blue_offset, int green_offset) {
    uint8_t* row = (uint8_t *)buffer.memory; 
    for (int Y = 0; Y < buffer.height; Y++){
	uint32_t* pixel = (uint32_t *) row; 
	for (int X = 0; X < buffer.width; X++){
	    uint8_t BLUE = (X+blue_offset);
	    uint8_t GREEN = (Y+green_offset);
	    uint8_t RED = 0xFF;
	    *pixel = (RED<<(8*2)) | (GREEN<<8) | (BLUE);
	    ++pixel;
	}
	row+=buffer.pitch;
    }
}

static void GameUpdateAndRender(game_offscreen_buffer &buffer, int blue_offset, int green_offset)
{
    // int blue_offset = 0;
    // int green_offset = 0;
    GradientRenderer(buffer, blue_offset, green_offset);
}

/*
 * TODO: Services that the platform layer provides to the game
*/
// #endif 
