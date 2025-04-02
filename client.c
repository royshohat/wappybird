#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <math.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_timer.h>

#include "entities.h"

#define HEIGHT 540
#define WIDTH 960
#define FPS 60
#define BIRDWIDTH HEIGHT/12
#define BIRDHEIGHT HEIGHT/15


SDL_Window* gWindow = NULL;
SDL_Surface* gScreenSurface = NULL;

bool init(); 
bool make_window();
void loop(entArr* entArray);
void draw(entArr* entArray);
void update(entArr* entArray);
void init_entArrs(entArr *entArray);
void end(); // clean up


int main(int argc, char** argv){
    //init();
    entArr entArray;
    entArray.birdArrSize=2;
    init_entArrs(&entArray);
    bird* birds = entArray.birdArr;
 
    bool res = make_window();
    if (!res){
        perror("Error creating a windows\n");
    }

    loop(&entArray);
    free(entArray.birdArr);
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

 void init_entArrs(entArr* entArray){

    //birds part
    entArray->birdArr = malloc(sizeof(bird) *entArray->birdArrSize);
    
    void* pos = entArray->birdArr;
    int x;
    
    for (int i = 0; i < entArray->birdArrSize; i++) {
        x = WIDTH/10 + i*( ((8*WIDTH/10)-BIRDWIDTH) / (entArray->birdArrSize - 1) );
        SDL_Rect rect = {x ,HEIGHT/3, BIRDWIDTH, BIRDHEIGHT};

        bird b = {i+1, 3, i % 2 == 0 ? RIGHT : LEFT, 0, rect, 0, rect.y};
        *((bird*) pos) = b;
        pos += sizeof(bird);
    }


 }

bool make_window(){
    gWindow = SDL_CreateWindow("Hello Test!", WIDTH, HEIGHT, 0);
    gScreenSurface = SDL_GetWindowSurface( gWindow );
    return (gWindow != NULL);
}

void loop(entArr* entArray) {
    bool running = true;
    // SDL_Rect dvd_rect = {100,100,150,75};
    // ------>
    // |    SDL
    // v
    while(running){
        SDL_Event e;
        SDL_zero(e);
        while( SDL_PollEvent( &e ) ) {
            if( e.type == SDL_EVENT_QUIT ) {
            running = false;
            }
        }
        draw(entArray);
        update(entArray);
        SDL_Delay(1000/FPS); // casts to int
    }
    
}
void draw(entArr* entArray){
    SDL_FillSurfaceRect(gScreenSurface, NULL, SDL_MapSurfaceRGB(gScreenSurface, 0xFF, 0xFF, 0xFF));
    for (int i = 0; i < entArray->birdArrSize; i++) {
        bird b = entArray->birdArr[i];
        SDL_FillSurfaceRect(gScreenSurface, &b.rect, SDL_MapSurfaceRGB(gScreenSurface, 0xFF, 0x00, 0x00));
    }
}

void update(entArr* entArray){ 
    // update
    SDL_UpdateWindowSurface( gWindow );
    
    for(int i=0; i<entArray->birdArrSize; i++){
        // update velocity
        entArray->birdArr[i].vy+=(G*0.5/FPS);
        entArray->birdArr[i].subPixY += entArray->birdArr[i].vy; 
        // update location from sub-pixel
        entArray->birdArr[i].rect.y = round(entArray->birdArr[i].subPixY);
    }

}

void end() {
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;
    gScreenSurface = NULL;
    SDL_Quit();
}