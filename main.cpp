#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>
#include "include/RenderWindow.hpp"
#include "include/Entity.hpp"
#include "include/Utils.hpp"

int main(int argc, char *args[])
{
    if (SDL_Init(SDL_INIT_VIDEO) > 0)
    {
        std::cout << "HEY... SDL_INIT HAS FAILED. SDL_ERROR: " << SDL_GetError() << std::endl;
    }

    if (!(IMG_Init(IMG_INIT_PNG)))
        std::cout << "HEY... IMG_Init HAS FAILED. SDL_ERROR: " << SDL_GetError() << std::endl;

    RenderWindow window("Creamy Kayas v1.0", 1280, 720);

    SDL_Texture *grassTexture = window.loadTexture("res/gfx/ground_grass_1.png");

    Entity grassEntity(Vector2f(0, 0), grassTexture);

    bool gameRunning = true;
    SDL_Event event;

    // Movement variables
    float moveSpeed = 0.3f;
    bool moveUp = false, moveDown = false, moveLeft = false, moveRight = false;

    const float timeStep = 0.01f;
    float acuumulator = 0.01f;
    float currentTime = utils::hireTimeInSeconds();

    while (gameRunning)
    {
        int startTick = SDL_GetTicks();
        float newTime = utils::hireTimeInSeconds();
        float frameTime = newTime - currentTime;
        currentTime = newTime;
        acuumulator += frameTime;

        while (acuumulator >= timeStep)
        {
            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_QUIT)
                    gameRunning = false;

                // Handle keydown events
                if (event.type == SDL_KEYDOWN)
                {
                    switch (event.key.keysym.sym)
                    {
                        case SDLK_w: moveUp = true; break;
                        case SDLK_s: moveDown = true; break;
                        case SDLK_a: moveLeft = true; break;
                        case SDLK_d: moveRight = true; break;
                    }
                }

                // Handle keyup events
                if (event.type == SDL_KEYUP)
                {
                    switch (event.key.keysym.sym)
                    {
                        case SDLK_w: moveUp = false; break;
                        case SDLK_s: moveDown = false; break;
                        case SDLK_a: moveLeft = false; break;
                        case SDLK_d: moveRight = false; break;
                    }
                }
            }

            // Update entity position based on the movement flags
            if (moveUp) grassEntity.getPos().y -= moveSpeed;
            if (moveDown) grassEntity.getPos().y += moveSpeed;
            if (moveLeft) grassEntity.getPos().x -= moveSpeed;
            if (moveRight) grassEntity.getPos().x += moveSpeed;

            acuumulator -= timeStep;
        }

        window.clear();
        window.render(grassEntity); // Render the entity at the new position
        window.display();

        int frameTicks = SDL_GetTicks() - startTick;
        if (frameTicks < 100 / window.getRefreshRate())
        {
            SDL_Delay(100 / window.getRefreshRate());
        }
    }

    window.cleanUp();
    SDL_Quit();

    return 0;
}
