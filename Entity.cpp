#include "include/Entity.hpp"

Entity::Entity(Vector2f p_pos, SDL_Texture* p_text, int frameCount, int p_frameWidth, int p_frameHeight)
    : pos(p_pos), text(p_text), frameCount(frameCount), currentFrameIndex(0), animationSpeed(0.1f), 
      timeSinceLastFrame(0), frameWidth(p_frameWidth), frameHeight(p_frameHeight), animationRow(0)
{
    currentFrame.x = 0;
    currentFrame.y = 0;
    currentFrame.w = frameWidth;  // Set width based on frameWidth
    currentFrame.h = frameHeight; // Set height based on frameHeight
}

void Entity::updateAnimation(float deltaTime) {
    timeSinceLastFrame += deltaTime;
    if (timeSinceLastFrame >= animationSpeed) {
        currentFrameIndex = (currentFrameIndex + 1) % frameCount;
        currentFrame.x = currentFrameIndex * frameWidth;  // Move horizontally across the sprite sheet
        currentFrame.y = animationRow * frameHeight;       // Stay in the same row
        timeSinceLastFrame = 0;
    }
}

bool Entity::animationFinished() const {
    return currentFrameIndex == (frameCount - 1);
}

void Entity::resetAnimation() {
    currentFrameIndex = 0;
    currentFrame.x = 0;
    timeSinceLastFrame = 0;
}

void Entity::setFrameSize(int width, int height) {
    frameWidth = width;
    frameHeight = height;
    currentFrame.w = frameWidth;
    currentFrame.h = frameHeight;
}

void Entity::setAnimationRow(int row) {
    animationRow = row;
}

void Entity::setTexture(SDL_Texture* newTexture) {
    text = newTexture;
    resetAnimation();  // Reset animation when switching textures
}

SDL_Rect Entity::getBoundingBox() const {
    return { static_cast<int>(pos.x), static_cast<int>(pos.y), frameWidth, frameHeight };
}


void Entity::setFrameCount(int count) {
    frameCount = count;
}
