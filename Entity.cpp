#include "include/Entity.hpp"
#include "include/math.hpp"

Entity::Entity(Vector2f p_pos, SDL_Texture* p_text)
:pos(p_pos), text(p_text)
{

    currentFrame.x = 0;
    currentFrame.y = 0;
    currentFrame.w = 32;
    currentFrame.h = 32;

    

}



SDL_Texture* Entity::getText(){
    return text;
}

SDL_Rect Entity::getCurrentFrame(){
    return currentFrame;
}