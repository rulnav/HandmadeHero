// #ifndef HANDMADE_CPP
// #define HANDMADE_CPP
#include "../headers/handmade.hpp"
#include "stdint.h"

#define Pi32 3.14159265358979f
/*
 * NOTE: Services that the game provides to the platform layer.
*/
// player input, bitmap buffer to use, sound buffer to use
static void GradientRenderer(s_gameOffscreenBuffer &buffer, int blue_offset, int green_offset) {
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

// void FillSoundBuffer(s_gameSoundOutputBuffer *sound_output, int ByteToLock, int bytes_to_write)
// {
//     // int SampleCount = bytes_to_write/sound_output->bytes_per_sample;
//     // void *AudioBuffer = malloc(bytes_to_write);
//     int16_t *SampleOut = (int16_t*)sound_output->samples;
//     for(int SampleIndex = 0;
//         SampleIndex < sound_output->sample_count;
//         ++SampleIndex)
//     {
//         // TODO(casey): Draw this out for people
//         // float SineValue = sinf(sound_output->t_sine);
//         // int16_t SampleValue = (int16_t)(SineValue * sound_output->tone_volume);
//         *SampleOut++ = SampleValue;
//         *SampleOut++ = SampleValue;
//
//         sound_output->t_sine += 2.0f*Pi32*1.0f/(float)sound_output->wave_period;
//         ++sound_output->running_sample_index;
//     }
//
//     SDL_QueueAudio(1, AudioBuffer, bytes_to_write);
//
//     free(AudioBuffer);
// }

static void GameOutputSound(s_gameSoundOutputBuffer &sound_buffer){
    static float t_sine;
    int16_t tone_volume = 3000;
    int tone_Hz = 256;
    int wave_period = sound_buffer.samples_per_second / tone_Hz;
    int16_t *sample_out = sound_buffer.samples;
    for(int sample_index = 0; sample_index < sound_buffer.sample_count; ++sample_index){
	float sine_value = sinf(t_sine);
	int16_t sample_value = (int16_t)(sine_value * tone_volume);
	*sample_out++ = sample_value;
	*sample_out++ = sample_value;
	t_sine += 2.0f*Pi32*1.0f/(float)wave_period;
    }
}

static void GameUpdateAndRender(s_gameOffscreenBuffer &buffer, int blue_offset, int green_offset, s_gameSoundOutputBuffer &sound_buffer)
{
    //TODO(casey): Allow sample offsets here for more robust platorm options
    GameOutputSound(sound_buffer);
    GradientRenderer(buffer, blue_offset, green_offset);
}

/*
 * TODO: Services that the platform layer provides to the game
*/
// #endif 
