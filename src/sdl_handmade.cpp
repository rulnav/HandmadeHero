#include "iostream"
#include "SDL2/SDL.h"
#include "SDL2/SDL_events.h"
#include "SDL2/SDL_video.h"
#include "array"
	
void SDLResizeTexture(SDL_Renderer *renderer, const int& width, const int& height){
    SDL_Texture *texture = SDL_CreateTexture(renderer,
					     SDL_PIXELFORMAT_ARGB8888, //one byte for alpha, followed by 3 byte for red, green and blue, that is the pixel format
					     SDL_TEXTUREACCESS_STREAMING,
					     width,
					     height);
    if (!texture) {
	std::cout<<"error creating texture"<<'\n';
	return;
    }
    void *pixels = malloc(width * height * 4); //multiply by 4 for the four bytes of each pixel
    if (SDL_UpdateTexture(texture,
			  0,
			  pixels,
			  width * 4)){
	std::cout<<"error updating texture"<<'\n';
    }
    if (SDL_RenderCopy(renderer,
		       texture,
		       0,
		       0)){
	std::cout<<"error copying renderer"<<'\n';
    }
    SDL_DestroyTexture(texture);
    free(pixels);
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
    for(;;)
    {
	SDL_Event Event;
	SDL_WaitEvent(&Event);
	if (HandleEvent(Event, renderer))
	{
	    break;
	}
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);
    }
    // SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Handmade Hero", "This is Handmade Hero", 0);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return(0);
}
