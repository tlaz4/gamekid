#include "SDL2/SDL.h"
#include <stdio.h>

#include "joypad.h"

// handle all external events from the device, ie drawing to screen and getting input

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Event event;

void drawScreen(int LCDScreen[160][144]){
	for (int i = 0; i < 144; i++) {
        for (int j = 0; j < 160; j++) {
        	if(LCDScreen[j][i] == 3){
        		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xff);
        	}else if(LCDScreen[j][i] == 2){
        		SDL_SetRenderDrawColor(renderer, 96, 96, 96, 0xff);
        	}else if(LCDScreen[j][i] == 1){
                SDL_SetRenderDrawColor(renderer, 192, 192, 192, 0xff);
            }else{
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0xff);
            }
        	SDL_RenderDrawPoint(renderer, j, i);
        }
    }
    SDL_RenderPresent(renderer);
}

void createWindow(){
	window = SDL_CreateWindow("gamekid", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 160, 144, 0);
	if(window == NULL){
		printf("failed to create window");
	}else{
		renderer = SDL_CreateRenderer(window, -1, 0);
	}
}

int getInput(){
    while(SDL_PollEvent(&event)){
        // check what type of event it is
        if(event.type == SDL_QUIT){
            return 1;
        }

        switch(event.type){
            case SDL_KEYDOWN:
                // Check the SDLKey values
                switch(event.key.keysym.sym){
                    case SDLK_RIGHT:
                        writeToJoypad('r');
                        break;

                    case SDLK_LEFT:
                        writeToJoypad('l');
                        break;

                    case SDLK_UP:
                        writeToJoypad('u');
                        break;

                    case SDLK_DOWN:
                        writeToJoypad('d');
                        break;

                    case SDLK_RETURN:
                        writeToJoypad('n');
                        break;

                    case SDLK_a:
                        writeToJoypad('a');
                        break;
                }
            break;
        }
        switch(event.type){
            case SDL_KEYUP:
                switch(event.key.keysym.sym){
                    case SDLK_RIGHT:
                        writeToJoypad('e');
                        break;

                    case SDLK_LEFT:
                        writeToJoypad('k');
                        break;

                    case SDLK_UP:
                        writeToJoypad('y');
                        break;

                    case SDLK_DOWN:
                        writeToJoypad('s');
                        break;

                    case SDLK_RETURN:
                        writeToJoypad('b');
                        break;

                    case SDLK_a:
                        writeToJoypad('@');
                        break;

                }
            break;
        }
    }
    return 0;
}

