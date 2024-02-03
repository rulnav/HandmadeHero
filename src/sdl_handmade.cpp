#include "iostream"
#include "SDL2/SDL.h"
#include "SDL2/SDL_events.h"
#include "SDL2/SDL_video.h"
#include "array"
#include "stdint.h"
	
struct SDL_Backbuffer {
    void* pixels;
    SDL_Texture *texture ;
    int width;
    int hight;
    int pitch;
    int bytesPerPixel {4};
} ;
static SDL_Backbuffer globalBuffer;

void GradientRenderer(const SDL_Backbuffer& texture, int offsetX, int offsetY){
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

void UpdateWindow(SDL_Backbuffer& buffer, SDL_Renderer *renderer){
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
    // SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}

void SDLStretchTextureToWindow(SDL_Backbuffer& buffer, SDL_Renderer *renderer, int width, int hight){
    free(buffer.pixels);
    buffer = { malloc(width * hight * buffer.bytesPerPixel),
		    SDL_CreateTexture(renderer,
				     SDL_PIXELFORMAT_ARGB8888, //one byte for alpha, followed by 3 byte for red, green and blue, that is the pixel format
				     SDL_TEXTUREACCESS_STREAMING,
				     width,
				     hight),
		    width, hight, width * buffer.bytesPerPixel,
    };
    if (!buffer.texture) {
	std::cout<<"error creating texture"<<'\n';
	return;
    }
    // pixels = malloc(width * hight * buffer.bytesPerPixel); //multiply by 4 for the four bytes of each pixel
    if(!buffer.pixels) std::cout<<"failed to allocate memory"<<'\n';
    // pitch = width * buffer.bytesPerPixel;
}

bool HandleEvent(const SDL_Event& Event, SDL_Renderer* renderer)
{
    bool shouldQuit = false;
    switch (Event.type)
    {
	case SDL_QUIT:{
	    std::cout<<"SDL_QUIT"<<'\n';
	    shouldQuit=true;
	} break;
	case SDL_WINDOWEVENT:{
	    switch(Event.window.event)
	    {
	        case SDL_WINDOWEVENT_SIZE_CHANGED:{
		    SDLStretchTextureToWindow(globalBuffer, renderer, Event.window.data1, Event.window.data2);
	        } break;
	    }
	} break;
    }
    return(shouldQuit);
}

int main(int argc, char *argv[])
{
    const int initial_width = 640;
    const int initial_hight = 480;
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
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
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if (!renderer) {
	std::cout<<"Error creating SDL Renderer.\n";
        return 1;
    }
    SDLStretchTextureToWindow(globalBuffer, renderer, initial_width, initial_hight);
    bool running = true;
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
	UpdateWindow(globalBuffer, renderer);
	++XOffset;
    }
    SDL_DestroyTexture(globalBuffer.texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return(0);
}
