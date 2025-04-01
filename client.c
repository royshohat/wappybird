#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#define HEIGHT 540
#define WIDTH 960

SDL_Window* gWindow = NULL;
SDL_Surface* gScreenSurface = NULL;

bool init(); 
bool make_window();
void loop();
void end(); // clean up

int main(int argc, char** argv){
    init();
    bool res = make_window();
    if (!res){
        perror("Error creating a windows\n");
    }

    loop();
    end();
    return 0;
}

bool init(){
    bool success = SDL_Init(SDL_INIT_VIDEO);
    if( !success ) {
       SDL_Log("Initialization failed! Cuz: %s\n", SDL_GetError());
    }
    return success;
 }

bool make_window(){
    gWindow = SDL_CreateWindow("Hello Test!", WIDTH, HEIGHT, 0);
    gScreenSurface = SDL_GetWindowSurface( gWindow );
    return (gWindow != NULL);
}

void loop() {
    bool running = true;
    while(running){
        SDL_Event e;
        SDL_zero(e);
        while( SDL_PollEvent( &e ) ) {
            if( e.type == SDL_EVENT_QUIT ) {
            running = false;
            }
        }
        SDL_FillSurfaceRect(gScreenSurface, NULL, SDL_MapSurfaceRGB(gScreenSurface, 0xFF, 0xFF, 0xFF));
        SDL_UpdateWindowSurface( gWindow );
    }
    
}

void end() {
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;
    gScreenSurface = NULL;
    SDL_Quit();
}