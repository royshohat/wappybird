#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#define HEIGHT 540
#define WIDTH 960

SDL_Window* gWindow = NULL;

bool init();
bool make_window();
void loop();

int main(int argc, char** argv){
    init();
    bool res = make_window();
    if (!res){
        perror("Error creating a windows\n");
    }
    sleep(1);
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
    return (gWindow != NULL);
}

void loop() {
    while(!0){

    }
}