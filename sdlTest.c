#include "SDL2/SDL.h"
#include <stdio.h>
#include <unistd.h>

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

void drawScreen(){
	for (int i = 0; i < 144; i++) {
        for (int j = 0; j < 166; j++) {
        	
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 0xff);
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

int main(){
    createWindow();
    drawScreen();
    sleep(10);

}