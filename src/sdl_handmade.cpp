#include "iostream"
#include "SDL2/SDL.h"
#include "SDL2/SDL_events.h"
#include "SDL2/SDL_video.h"
#include "array"
#include "stdint.h"
	
const int bytesPerPixel = 4;
struct TextureInfo {
    int width;
    int hight;
    int pitch;
    void* pixels;
    SDL_Texture *texture ;
} mainTexture;

void GradientRenderer(TextureInfo& texture, const int& offsetX, const int& offsetY){
    uint8_t* row = (uint8_t *)texture.pixels; 
    *row = 255;
    for (int Y = 0; Y < texture.hight; Y++){
	uint8_t* pixel = (uint8_t *) row; 
	for (int X = 0; X < texture.width; X++){
	    *pixel = (uint8_t)(X+offsetX);  //BLUE
	    ++pixel;

	    *pixel = (uint8_t)(Y+offsetY);  //GREEN
	    ++pixel;

	    *pixel = 0xFF;  //RED
	    ++pixel;

	    *pixel = 0x00; 
	    ++pixel;
	}
	row+=texture.pitch;
    }

}

void UpdateWindow(SDL_Window *window, SDL_Renderer *renderer){
    if (SDL_UpdateTexture(mainTexture.texture, 0,
			  mainTexture.pixels,
			  mainTexture.pitch)){
	std::cout<<"error updating texture"<<'\n';
    }
    if (SDL_RenderCopy(renderer,
		       mainTexture.texture,
		       0,
		       0)){
	std::cout<<"error copying renderer"<<'\n';
    }
    // SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}

void SDLResizeTexture(SDL_Renderer *renderer, const int& width, const int& hight){
    free(mainTexture.pixels);
    mainTexture = {width, hight, width * bytesPerPixel, malloc(width * hight * bytesPerPixel),
		   SDL_CreateTexture(renderer,
				     SDL_PIXELFORMAT_ARGB8888, //one byte for alpha, followed by 3 byte for red, green and blue, that is the pixel format
				     SDL_TEXTUREACCESS_STREAMING,
				     width,
				     hight)
    };
    if (!mainTexture.texture) {
	std::cout<<"error creating texture"<<'\n';
	return;
    }
    // pixels = malloc(width * hight * bytesPerPixel); //multiply by 4 for the four bytes of each pixel
    if(!mainTexture.pixels) std::cout<<"failed to allocate memory"<<'\n';
    // pitch = width * bytesPerPixel;
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
		    SDLResizeTexture(renderer, Event.window.data1, Event.window.data2);
		    // std::cout<<"width: "<<Event.window.data1<<' '<<
			   //     "hight: "<<Event.window.data2<<'\n';
	        } break;
		// case SDL_WINDOWEVENT_EXPOSED:{
		//     // SDL_Window *window = SDL_GetWindowFromID(Event.window.windowID);
		//     // SDL_Renderer *renderer = SDL_GetRenderer(window);
		//     static bool isWhite = true;
		//     if (isWhite == true)
		//     {
		// 	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		// 	isWhite = false;
		//     }
		//     else
		//     {
		// 	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		// 	isWhite = true;
		//     }
		// } break;
	    }
	} break;
    }
    return(shouldQuit);
}

int main(int argc, char *argv[])
{
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
		     640,
		     480,
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
    bool running = true;
    int XOffset = 0;
    while(running)
    {
	SDL_Event Event;
	while(SDL_PollEvent(&Event)){
	    if (HandleEvent(Event, renderer))
	    {
		running= false;
	    }
	}
	GradientRenderer(mainTexture, XOffset, 0);
	UpdateWindow(window, renderer);
	++XOffset;
    }
    // SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Handmade Hero", "This is Handmade Hero", 0);
    SDL_DestroyTexture(mainTexture.texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return(0);
}
