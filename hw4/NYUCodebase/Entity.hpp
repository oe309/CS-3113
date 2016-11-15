//
//  Entity.hpp
//  NYUCodebase
//
//  Created by Ovais Ellahi on 11/7/16.
//  Copyright © 2016 Ivan Safrin. All rights reserved.
//

#ifndef Entity_hpp
#define Entity_hpp


#include <stdio.h>
#include "Shaderprogram.h"
#include "SheetSprite.hpp"
// 60 FPS (1.0f/60.0f)
#define FIXED_TIMESTEP 0.0166666f
#define MAX_TIMESTEPS 6




enum EntityType {Shooter, Key};
class Entity {
public:
    Entity();
    Entity(SheetSprite sprt);
    void Update(float elapsed);
    void Render(ShaderProgram *program);
    bool collidesWith(Entity *entity);
    

    SheetSprite sprite;
    float x;
    float y;
    float width;
    float height;
    float velocity_x;
    float velocity_y;
    float acceleration_x;
    float acceleration_y;
    float friction_x;
    float friction_y;
    bool isStatic;
    EntityType entityType;
    bool collidedTop;
    bool collidedBottom;
    bool collidedLeft;
    bool collidedRight;
};
#endif /* Entity_hpp */
