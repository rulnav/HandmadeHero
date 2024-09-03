#include "iostream"
#include <x86intrin.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_events.h"
#include "SDL2/SDL_video.h"
#include "array"
#include "stdint.h"
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_keycode.h>
#include <cstdint>
#include <math.h>
#include "../src/handmade.cpp"

#define Pi32 3.14159265358979f
	
struct SDL_WindowDimension
{
    int width;
    int height;
};

struct SDL_Backbuffer {
    void* pixels;
    SDL_Texture *texture ;
    int width;
    int height;
    int pitch;
} ;
static SDL_Backbuffer globalBuffer;
static bool running = true;

struct SDL_SoundOutput
{
    int samples_per_second;
    int tone_hz;
    int16_t tone_volume;
    uint32_t running_sample_index;
    int wave_period;
    int bytes_per_sample;
    int secondary_buffer_size;
    float t_sine;
    int latency_sample_count;
};

SDL_WindowDimension SDLGetWindowDimension(SDL_Window *window){
    SDL_WindowDimension result;

    SDL_GetWindowSize(window, &result.width, &result.height);

    return(result);
}

static void SDLUpdateWindow(SDL_Backbuffer& buffer, SDL_Renderer *renderer){
    if (SDL_UpdateTexture(buffer.texture, 0,
			  buffer.pixels,
			  buffer.pitch)){
	std::cout<<"error updating texture"<<'\n';
    }
    if (SDL_RenderCopy(renderer,
		       buffer.texture,
		       0,
		       0)){
	std::cout<<"error copying renderer"<<'\n';
    }
    SDL_RenderPresent(renderer);
}

static void SDLStretchTextureToWindow(SDL_Backbuffer& buffer, SDL_Renderer *renderer, int width, int height){
    if(buffer.pixels){
	free(buffer.pixels);
    }
    if(buffer.texture){
       SDL_DestroyTexture(buffer.texture);
    }
    int bytesPerPixel {4};
    buffer = { malloc(width * height * bytesPerPixel),
		    SDL_CreateTexture(renderer,
    //one byte for alpha, followed by 3 byte for red, green and blue, that is the pixel format, or bytesPerPixel
				     SDL_PIXELFORMAT_ARGB8888,
				     SDL_TEXTUREACCESS_STREAMING,
				     width,
				     height),
		    width, height, width * bytesPerPixel,
    };
    if (!buffer.texture) {
	std::cout<<"error creating texture"<<'\n';
	return;
    }
    if(!buffer.pixels) std::cout<<"failed to allocate memory"<<'\n';
}

// static void SDLAudioCallback(void *UserData, Uint8 *AudioData, int Length){
//     // Clear our audio buffer to silence.
//     memset(AudioData, 0, Length);
// }

static void SDLInitAudio(int32_t samples_per_second, int32_t BufferSize){
    SDL_AudioSpec AudioSettings = {0};

    AudioSettings.freq = samples_per_second; //number of samples per second
    AudioSettings.format = AUDIO_S16LSB; //signed 16 bit
    AudioSettings.channels = 2; //1 for mono, 2 for stereo
    AudioSettings.samples = BufferSize; 
    // AudioSettings.callback = &SDLAudioCallback; //SDL will call this function whenever more data is required

    SDL_OpenAudio(&AudioSettings, 0);

    printf("Initialised an Audio device at frequency %d Hz, %d Channels\n",
           AudioSettings.freq, AudioSettings.channels);

    if (AudioSettings.format != AUDIO_S16LSB)
    {
        printf("Oops! We didn't get AUDIO_S16LSB as our sample format!\n");
        SDL_CloseAudio();
    }

    // SDL_PauseAudio(0);
}

void SDLFillSoundBuffer(SDL_SoundOutput *sound_output, int ByteToLock, int bytes_to_write)
{
    int SampleCount = bytes_to_write/sound_output->bytes_per_sample;
    void *AudioBuffer = malloc(bytes_to_write);
    int16_t *SampleOut = (int16_t*)AudioBuffer;
    for(int SampleIndex = 0;
        SampleIndex < SampleCount;
        ++SampleIndex)
    {
        // TODO(casey): Draw this out for people
        float SineValue = sinf(sound_output->t_sine);
        int16_t SampleValue = (int16_t)(SineValue * sound_output->tone_volume);
        *SampleOut++ = SampleValue;
        *SampleOut++ = SampleValue;

        sound_output->t_sine += 2.0f*Pi32*1.0f/(float)sound_output->wave_period;
        ++sound_output->running_sample_index;
    }

    SDL_QueueAudio(1, AudioBuffer, bytes_to_write);

    free(AudioBuffer);
}

static bool HandleEvent(const SDL_Event& event, SDL_Renderer* renderer){
    bool should_quit = false;
    switch (event.type)
    {
	case SDL_QUIT:{
	    std::cout<<"SDL_QUIT"<<'\n';
	    should_quit=true;
	} break;
	case SDL_WINDOWEVENT:{
	    switch(event.window.event)
	    {
	        case SDL_WINDOWEVENT_SIZE_CHANGED:{
		    //TODO: Think about removing this line, RenderCopy already stretches the texture
		    // SDLStretchTextureToWindow(globalBuffer, renderer, event.window.data1, event.window.data2);
	        } break;
	    }
	} break;
	case SDL_KEYDOWN:
	case SDL_KEYUP:
	{
	    SDL_Keycode key_code = event.key.keysym.sym;
	    bool was_down = false;
	    if (event.key.state == SDL_RELEASED)
	    {
		was_down = true;
	    }
	    else if (event.key.repeat != 0)
	    {
		was_down = true;
	    }
	    if (was_down) break;
	    if (key_code == SDLK_w){
		printf("W\n");
	    }
	    if (key_code == SDLK_s){
		printf("S\n");
	    }
	    if (key_code == SDLK_d){
		printf("D\n");
	    }
	    if (key_code == SDLK_a){
		printf("A\n");
	    }
	    if (key_code == SDLK_q){
		printf("Q\n");
	    }
	    if (key_code == SDLK_e){
		printf("E\n");
	    }
	    if (key_code == SDLK_RIGHT){
		printf("RIGHT\n");
	    }
	    if (key_code == SDLK_LEFT){
		printf("LEFT\n");
	    }
	    if (key_code == SDLK_UP){
		printf("UP\n");
	    }
	    if (key_code == SDLK_DOWN){
		printf("DOWN\n");
	    }
	    if (key_code == SDLK_ESCAPE){
		printf("Escape\n");
	    }
	} break;
    }
    return(should_quit);
}

int main(int argc, char *argv[])
{
    const int initial_width = 1200;
    const int initial_height = 720;

    // NOTE: Sound test                                                                                                                                                              
    SDL_SoundOutput sound_output = {};

    sound_output.samples_per_second = 48000;
    sound_output.tone_hz = 256;
    sound_output.tone_volume = 3000;
    sound_output.wave_period = sound_output.samples_per_second / sound_output.tone_hz;
    sound_output.bytes_per_sample = sizeof(int16_t) * 2;
    sound_output.latency_sample_count = sound_output.samples_per_second / 15;

    bool sound_is_playing = false;

    // Open our audio device:                                                                                                                                                        
    SDLInitAudio(sound_output.samples_per_second, sound_output.samples_per_second * sound_output.bytes_per_sample / 60);
    SDLFillSoundBuffer(&sound_output, 0, sound_output.latency_sample_count*sound_output.bytes_per_sample);
    // SDL_PauseAudio(0);


    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_HAPTIC | SDL_INIT_AUDIO) != 0)
    {
	//TODO: SDL_Init didn't work!
	std::cout << SDL_GetError();
	return 1;
    }
    SDL_Window *window;
    window = SDL_CreateWindow("Handmande Hero",
		     SDL_WINDOWPOS_UNDEFINED,
		     SDL_WINDOWPOS_UNDEFINED,
		     initial_width,
		     initial_height,
		     SDL_WINDOW_RESIZABLE);
    if (!window) {
	std::cout<<"Window failed to create"<<'\n';
	return 1;
    }
    SDLInitAudio(48000, 4096);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if (!renderer) {
	std::cout<<"Error creating SDL Renderer.\n";
        return 1;
    }
    // SDL_AudioSpec *audio_spec;
    // SDL_OpenAudio(audio_spec, audio_spec);

    SDLStretchTextureToWindow(globalBuffer, renderer, initial_width, initial_height);
    int XOffset = 0;
    int YOffset = 0;
#ifdef PERFORMANCE_LOG
    uint64_t perf_count_frequency = SDL_GetPerformanceFrequency();
    uint64_t last_counter = SDL_GetPerformanceCounter();
    uint64_t last_cycle_count = _rdtsc();
#endif
    struct game_offscreen_buffer buffer = {};
    while(running)
    {
	SDL_Event Event;
	while(SDL_PollEvent(&Event)){
	    if (HandleEvent(Event, renderer))
	    {
		running= false;
	    }
	}
	buffer.memory = globalBuffer.pixels;
	buffer.width = globalBuffer.width;
	buffer.height = globalBuffer.height;
	buffer.pitch = globalBuffer.pitch;
	GameUpdateAndRender(buffer, XOffset, YOffset);

	int target_queue_bytes = sound_output.latency_sample_count * sound_output.bytes_per_sample;
	int bytes_to_write = target_queue_bytes - SDL_GetQueuedAudioSize(1);
	SDLFillSoundBuffer(&sound_output, 0, bytes_to_write);
	if(!sound_is_playing)
	{
	    SDL_PauseAudio(0);
	    sound_is_playing = true;
	}

	SDLUpdateWindow(globalBuffer, renderer);
	++XOffset;
#ifdef PERFORMANCE_LOG
	uint64_t end_counter = SDL_GetPerformanceCounter();
	uint64_t counter_elapsed = end_counter - last_counter;
	uint64_t end_cycle_count = _rdtsc();
	uint64_t cycles_elapsed = end_cycle_count - last_cycle_count;

	double ms_per_frame = (((1000.0f * (double)counter_elapsed ) / (double)perf_count_frequency));
	double fps = (double)perf_count_frequency / (double)counter_elapsed;
	double mcpf = ((double)cycles_elapsed / (1000.0f * 1000.0f));

	printf("%.02f ms/f, %.02f fps/s, %.02f mc/f\n",ms_per_frame, fps, mcpf);
	last_cycle_count = end_cycle_count;
	last_counter = end_counter;
#endif
    }
    SDL_DestroyTexture(globalBuffer.texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return(0);
}
