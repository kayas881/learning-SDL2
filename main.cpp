#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>
#include "include/RenderWindow.hpp"
#include "include/Entity.hpp"
#include "include/Utils.hpp"
#include <math.h>

std::string bossState = "idle";  // Initialize with "idle"

bool checkCollision(const SDL_Rect& a, const SDL_Rect& b) {
    return SDL_HasIntersection(&a, &b);
}

float calculateDistance(const Vector2f& pos1, const Vector2f& pos2) {
    float dx = pos2.x - pos1.x;
    float dy = pos2.y - pos1.y;
    return sqrt(dx * dx + dy * dy);
}

Vector2f calculateDirection(const Vector2f& from, const Vector2f& to) {
    Vector2f direction = { to.x - from.x, to.y - from.y };
    float length = sqrt(direction.x * direction.x + direction.y * direction.y);
    if (length != 0) {
        direction.x /= length;
        direction.y /= length;
    }
    return direction;
}

int main(int argc, char *args[]) {
    if (SDL_Init(SDL_INIT_VIDEO) > 0) {
        std::cout << "HEY... SDL_INIT HAS FAILED. SDL_ERROR: " << SDL_GetError() << std::endl;
    }

    if (!(IMG_Init(IMG_INIT_PNG))) {
        std::cout << "HEY... IMG_Init HAS FAILED. SDL_ERROR: " << SDL_GetError() << std::endl;
    }

    RenderWindow window("Creamy Kayas v1.0", 1280, 720);

    SDL_Texture *pathTexture = window.loadTexture("res/gfx/2.png");
    SDL_Texture *skyTexture = window.loadTexture("res/gfx/1.png");
    SDL_Texture *idleTexture = window.loadTexture("res/gfx/Idle.png");
    SDL_Texture *walkTexture = window.loadTexture("res/gfx/Walk.png");
    SDL_Texture *runTexture = window.loadTexture("res/gfx/Run.png");
    SDL_Texture *walkLeftTexture = window.loadTexture("res/gfx/Walk_flipped_reversed.png");
    SDL_Texture *runLeftTexture = window.loadTexture("res/gfx/Run_flipped_reversed.png");
    SDL_Texture *idleLeftTexture = window.loadTexture("res/gfx/IdleLeft.png");
    SDL_Texture *attackTexture = window.loadTexture("res/gfx/Attack.png");
    SDL_Texture *leftAttackTexture = window.loadTexture("res/gfx/Reversed_Attack.png");
    SDL_Texture *bossIdleTexture = window.loadTexture("res/gfx/Enemy/Idle.png");
    SDL_Texture *bossHurtTexture = window.loadTexture("res/gfx/Enemy/Hurt.png");
    SDL_Texture *bossWalkTexture = window.loadTexture("res/gfx/Enemy/Walk.png"); // Walking animation
    SDL_Texture *bossWalkLeftTexture = window.loadTexture("res/gfx/Enemy/WalkLeft.png"); // Walking animation
    SDL_Texture *bossAttackTexture = window.loadTexture("res/gfx/Enemy/Attack_1.png"); // Attack animation
    SDL_Texture *bossAttackPoisonTexture = window.loadTexture("res/gfx/Enemy/Poison.png"); // Attack animation
    SDL_Texture *BossAttackPoisonCloudAnimation = window.loadTexture("res/gfx/Enemy/PoisonCloud.png"); // Attack animation

    Entity pathEntity(Vector2f(0, 0), pathTexture);
    Entity idleCharacter(Vector2f(100, 100), idleTexture, 5, 128, 128); // 5 frames for idle
    Entity idleLeftCharacter(Vector2f(100, 100), idleLeftTexture, 5, 128, 128); // 5 frames for idle
    Entity walkCharacter(Vector2f(100, 100), walkTexture, 9, 128, 128); // 9 frames for walk
    Entity runCharacter(Vector2f(100, 100), runTexture, 10, 128, 128);
    Entity walkLeftCharacter(Vector2f(100, 100), walkLeftTexture, 9, 128, 128); // 9 frames for walk
    Entity runLeftCharacter(Vector2f(100, 100), runLeftTexture, 10, 128, 128);
    Entity attackCharacter(Vector2f(100, 100), attackTexture, 5, 128, 128);
    Entity leftAttackCharacter(Vector2f(100, 100), leftAttackTexture, 5, 128, 128);
    Entity bossEntity(Vector2f(50, 30), bossIdleTexture, 5, 128, 128); // Set frame size to 128x128

    bool gameRunning = true;
    SDL_Event event;

    float walkSpeed = 0.3f;
    float runSpeed = 0.6f;
    float bossSpeed = 0.2f; // Adjust the speed of the boss
    bool moveUp = false, moveDown = false, moveLeft = false, moveRight = false;
    bool isMoving = false;
    bool isRunning = false;
    bool isAttacking = false;
    bool attackFinished = true;
    bool bossHurt = false;
    float bossHurtDuration = 0.5f;
    float bossHurtTime = 0.0f;
    bool facingLeft = false; // Track the direction the character is facing

    const float timeStep = 0.01f;
    float accumulator = 0.01f;
    float currentTime = utils::hireTimeInSeconds();
    float attackRange = 50.0f;  // You can tweak this value based on your game
    float stopChaseDistance = 115.0f;
    float bossAttackRange = 30.0f; // Boss attack range

    while (gameRunning) {
        int startTick = SDL_GetTicks();
        float newTime = utils::hireTimeInSeconds();
        float frameTime = newTime - currentTime;
        currentTime = newTime;
        accumulator += frameTime;

        while (accumulator >= timeStep) {
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    gameRunning = false;
                }

                if (event.type == SDL_KEYDOWN) {
                    switch (event.key.keysym.sym) {
                        case SDLK_w: moveUp = true; break;
                        case SDLK_s: moveDown = true; break;
                        case SDLK_a: moveLeft = true; facingLeft = true; break;
                        case SDLK_d: moveRight = true; facingLeft = false; break;
                        case SDLK_LSHIFT: isRunning = true; break;
                        case SDLK_SPACE:
                            if (!isAttacking) {
                                isAttacking = true;
                                if (facingLeft) {
                                    leftAttackCharacter.resetAnimation();
                                    leftAttackCharacter.setAnimationSpeed(0.10f);
                                } else {
                                    attackCharacter.resetAnimation();
                                    attackCharacter.setAnimationSpeed(0.10f);
                                }
                            }
                            break;
                    }
                }

                if (event.type == SDL_KEYUP) {
                    switch (event.key.keysym.sym) {
                        case SDLK_w: moveUp = false; break;
                        case SDLK_s: moveDown = false; break;
                        case SDLK_a: moveLeft = false; break;
                        case SDLK_d: moveRight = false; break;
                        case SDLK_LSHIFT: isRunning = false; break;
                    }
                }
            }

            isMoving = moveUp || moveDown || moveLeft || moveRight;
            float currentSpeed = isRunning ? runSpeed : walkSpeed;

            if (!isAttacking) {
                if (moveUp) idleCharacter.getPos().y -= currentSpeed;
                if (moveDown) idleCharacter.getPos().y += currentSpeed;
                if (moveLeft) idleCharacter.getPos().x -= currentSpeed;
                if (moveRight) idleCharacter.getPos().x += currentSpeed;
            }

            // Boss chase logic
            Vector2f bossDirection = calculateDirection(bossEntity.getPos(), idleCharacter.getPos());
            float distanceToBoss = calculateDistance(idleCharacter.getPos(), bossEntity.getPos());

            if (distanceToBoss <= bossAttackRange) {
                // Boss is attacking
                if (bossState != "attacking") {
                    bossEntity.setTexture(bossAttackTexture);  // Only set texture if switching to attack state
                    bossEntity.setFrameCount(6);               // 6 frames for attack
                    bossEntity.setFrameSize(128, 128);         // Set the correct frame size
                    bossEntity.setAnimationSpeed(0.20f);       // Slower animation speed
                    bossEntity.resetAnimation();               // Reset animation for smooth transition
                    bossState = "attacking";                   // Update the state to attacking
                }
            } else if (distanceToBoss > stopChaseDistance) {
                // Boss is walking towards the character
                if (bossDirection.x < 0) { // Moving left
                    if (bossState != "walking_left") {
                        bossEntity.setTexture(bossWalkLeftTexture);  // Only set texture if switching to walk left state
                        bossEntity.setFrameCount(9);                 // 9 frames for walking
                        bossEntity.setFrameSize(128, 128);           // Set the correct frame size
                        bossEntity.setAnimationSpeed(0.15f);         // Slower animation speed
                        bossEntity.resetAnimation();                 // Reset animation for smooth transition
                        bossState = "walking_left";                  // Update the state to walking left
                    }
                } else { // Moving right
                    if (bossState != "walking_right") {
                        bossEntity.setTexture(bossWalkTexture);      // Only set texture if switching to walk right state
                        bossEntity.setFrameCount(9);                 // 9 frames for walking
                        bossEntity.setFrameSize(128, 128);           // Set the correct frame size
                        bossEntity.setAnimationSpeed(0.15f);         // Slower animation speed
                        bossEntity.resetAnimation();                 // Reset animation for smooth transition
                        bossState = "walking_right";                 // Update the state to walking right
                    }
                }

                bossEntity.getPos().x += bossDirection.x * bossSpeed;
                bossEntity.getPos().y += bossDirection.y * bossSpeed;

            } else {
                // Boss is idle
                if (bossState != "idle") {
                    bossEntity.setTexture(bossIdleTexture);  // Only set texture if switching to idle state
                    bossEntity.setFrameCount(5);             // 5 frames for idle
                    bossEntity.setFrameSize(128, 128);       // Set the correct frame size
                    bossEntity.setAnimationSpeed(0.25f);     // Slower animation speed
                    bossEntity.resetAnimation();             // Reset animation for smooth transition
                    bossState = "idle";                      // Update the state to idle
                }
            }

            // Update boss animation in either state
            bossEntity.updateAnimation(frameTime);

            // Check if character attack hits the boss
            if (isAttacking && distanceToBoss <= attackRange && checkCollision(attackCharacter.getBoundingBox(), bossEntity.getBoundingBox())) {
                bossHurt = true;
                bossEntity.setTexture(bossHurtTexture);
                bossEntity.setFrameSize(128, 128);
                bossEntity.setAnimationRow(0);
                bossEntity.setAnimationSpeed(0.20f);  // Slower animation speed
                bossEntity.resetAnimation();
                bossHurtTime = 0.0f;
            }

            // Boss hurt state logic
            if (bossHurt) {
                bossHurtTime += frameTime;
                bossEntity.updateAnimation(frameTime);

                if (bossHurtTime >= bossHurtDuration) {
                    bossHurt = false;
                    bossEntity.setTexture(bossIdleTexture);
                    bossEntity.setFrameSize(128, 128);
                    bossEntity.resetAnimation();
                }
            } else {
                bossEntity.updateAnimation(frameTime);
            }

            if (isAttacking) {
                if (facingLeft) {
                    leftAttackCharacter.getPos() = idleCharacter.getPos();
                    leftAttackCharacter.updateAnimation(frameTime);
                    if (leftAttackCharacter.animationFinished()) {
                        isAttacking = false;
                    }
                } else {
                    attackCharacter.getPos() = idleCharacter.getPos();
                    attackCharacter.updateAnimation(frameTime);
                    if (attackCharacter.animationFinished()) {
                        isAttacking = false;
                    }
                }
            } else if (isMoving && isRunning) {
                if (moveLeft) {
                    runLeftCharacter.getPos() = idleCharacter.getPos();
                    runLeftCharacter.updateAnimation(frameTime);
                } else {
                    runCharacter.getPos() = idleCharacter.getPos();
                    runCharacter.updateAnimation(frameTime);
                }
            } else if (isMoving) {
                if (moveLeft) {
                    walkLeftCharacter.getPos() = idleCharacter.getPos();
                    walkLeftCharacter.updateAnimation(frameTime);
                } else {
                    walkCharacter.getPos() = idleCharacter.getPos();
                    walkCharacter.updateAnimation(frameTime);
                }
            } else {
                if (facingLeft) {
                    idleLeftCharacter.getPos() = idleCharacter.getPos();
                    idleLeftCharacter.updateAnimation(frameTime);
                } else {
                    idleCharacter.updateAnimation(frameTime);
                }
            }

            accumulator -= timeStep;
        }

        window.clear();
        window.renderSky(skyTexture);
        window.renderPath(pathEntity);

        window.render(bossEntity);

        if (isAttacking) {
            if (facingLeft) {
                window.render(leftAttackCharacter);
            } else {
                window.render(attackCharacter);
            }
        } else if (isMoving && isRunning) {
            if (moveLeft) {
                window.render(runLeftCharacter);
            } else {
                window.render(runCharacter);
            }
        } else if (isMoving) {
            if (moveLeft) {
                window.render(walkLeftCharacter);
            } else {
                window.render(walkCharacter);
            }
        } else {
            if (facingLeft) {
                window.render(idleLeftCharacter);
            } else {
                window.render(idleCharacter);
            }
        }

        window.display();

        int frameTicks = SDL_GetTicks() - startTick;
        if (frameTicks < 100 / window.getRefreshRate()) {
            SDL_Delay(100 / window.getRefreshRate());
        }
    }

    window.cleanUp();
    SDL_Quit();

    return 0;
}