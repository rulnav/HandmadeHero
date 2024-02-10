#include "iostream"
#include "SDL2/SDL.h"
#include "SDL2/SDL_events.h"
#include "SDL2/SDL_video.h"
#include "array"
#include "stdint.h"
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_keycode.h>
#include <cstdint>
	
struct sdl_window_dimension
{
    int width;
    int height;
};

struct SDL_Backbuffer {
    void* pixels;
    SDL_Texture *texture ;
    int width;
    int hight;
    int pitch;
} ;
static SDL_Backbuffer globalBuffer;
static bool running = true;

sdl_window_dimension SDLGetWindowDimension(SDL_Window *window){
    sdl_window_dimension result;

    SDL_GetWindowSize(window, &result.width, &result.height);

    return(result);
}

static void GradientRenderer(const SDL_Backbuffer& texture, int offsetX, int offsetY){
    uint8_t* row = (uint8_t *)texture.pixels; 
    for (int Y = 0; Y < texture.hight; Y++){
	uint32_t* pixel = (uint32_t *) row; 
	for (int X = 0; X < texture.width; X++){
	    uint8_t BLUE = (X+offsetX);
	    uint8_t GREEN = (Y+offsetY);
	    uint8_t RED = 0xFF;
	    *pixel = (RED<<(8*2)) | (GREEN<<8) | (BLUE);
	    ++pixel;
	}
	row+=texture.pitch;
    }

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

static void SDLStretchTextureToWindow(SDL_Backbuffer& buffer, SDL_Renderer *renderer, int width, int hight){
    if(buffer.pixels){
	free(buffer.pixels);
    }
    if(buffer.texture){
       SDL_DestroyTexture(buffer.texture);
    }
    int bytesPerPixel {4};
    buffer = { malloc(width * hight * bytesPerPixel),
		    SDL_CreateTexture(renderer,
    //one byte for alpha, followed by 3 byte for red, green and blue, that is the pixel format, or bytesPerPixel
				     SDL_PIXELFORMAT_ARGB8888,
				     SDL_TEXTUREACCESS_STREAMING,
				     width,
				     hight),
		    width, hight, width * bytesPerPixel,
    };
    if (!buffer.texture) {
	std::cout<<"error creating texture"<<'\n';
	return;
    }
    if(!buffer.pixels) std::cout<<"failed to allocate memory"<<'\n';
}

static void SDLAudioCallback(void *UserData, Uint8 *AudioData, int Length){
    // Clear our audio buffer to silence.
    memset(AudioData, 0, Length);
}

static void SDLInitAudio(int32_t SamplesPerSecond, int32_t BufferSize){
    SDL_AudioSpec AudioSettings = {0};

    AudioSettings.freq = SamplesPerSecond;
    AudioSettings.format = AUDIO_S16LSB;
    AudioSettings.channels = 2;
    AudioSettings.samples = BufferSize;
    AudioSettings.callback = &SDLAudioCallback;

    SDL_OpenAudio(&AudioSettings, 0);

    printf("Initialised an Audio device at frequency %d Hz, %d Channels\n",
           AudioSettings.freq, AudioSettings.channels);

    if (AudioSettings.format != AUDIO_S16LSB)
    {
        printf("Oops! We didn't get AUDIO_S16LSB as our sample format!\n");
        SDL_CloseAudio();
    }

    SDL_PauseAudio(0);
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
    const int initial_hight = 720;
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
		     initial_hight,
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

    SDLStretchTextureToWindow(globalBuffer, renderer, initial_width, initial_hight);
    int XOffset = 0;
    int YOffset = 0;
    while(running)
    {
	SDL_Event Event;
	while(SDL_PollEvent(&Event)){
	    if (HandleEvent(Event, renderer))
	    {
		running= false;
	    }
	}
	GradientRenderer(globalBuffer, XOffset, YOffset );
	SDLUpdateWindow(globalBuffer, renderer);
	++XOffset;
    }
    SDL_DestroyTexture(globalBuffer.texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return(0);
}
