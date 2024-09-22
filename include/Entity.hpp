#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include "math.hpp"

class Entity {
public:
    Entity(Vector2f p_pos, SDL_Texture* p_text, int frameCount = 1, int p_frameWidth = 128, int p_frameHeight = 128);

    Vector2f& getPos() { return pos; }
    SDL_Texture* getText() { return text; }
    SDL_Rect getCurrentFrame() { return currentFrame; }

    int getFrameCount() const { return frameCount; }
    int getCurrentFrameIndex() const { return currentFrameIndex; }

    // Animation functions
    void updateAnimation(float deltaTime);
    void setAnimationSpeed(float speed) { animationSpeed = speed; }
    void resetAnimation();
    void setFrameCount(int count);
    bool animationFinished() const;
    SDL_Rect getBoundingBox() const;

    // New functions
    void setFrameSize(int width, int height);  // Set frame size for animations
    void setAnimationRow(int row);             // Set animation row (e.g., for different states like idle, walk)
    void setTexture(SDL_Texture* newTexture);  // Set new texture, useful for switching animations (e.g., boss hurt)
  void setState(const std::string& state);  // New method to set state
private:
    Vector2f pos;
    SDL_Rect currentFrame;
    SDL_Texture* text;
    std::string currentState;  // New field to track current state

    int frameCount;
    int currentFrameIndex;
    float animationSpeed;
    float timeSinceLastFrame;

    // Added fields for better frame control
    int frameWidth;
    int frameHeight;
    int animationRow;
};
