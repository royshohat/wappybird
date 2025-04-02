#include <SDL3/SDL_rect.h>
#define G 9.817

typedef enum  {
    LEFT = -1,
    RIGHT = 1
} direction;

typedef struct{
    int id; 
    int hearts;
    direction d;
    int coolDown; //milisecs until you can shot
    SDL_Rect rect; // hitbox
    int vy;
} bird;

typedef struct{
    bird* birdArr;
    size_t birdArrSize;
    //more to come
}entArr;
