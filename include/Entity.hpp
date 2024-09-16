#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <iostream>
#include "math.hpp"

class Entity{

    public: 
    Entity(Vector2f p_pos, SDL_Texture* p_text);

   Vector2f& getPos(){
    return pos;
   }
    SDL_Texture* getText();
    SDL_Rect getCurrentFrame();

    private:
   Vector2f pos;
    SDL_Rect currentFrame;
    SDL_Texture* text;



};
