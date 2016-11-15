//
//  Entity.cpp
//  NYUCodebase
//
//  Created by Ovais Ellahi on 11/7/16.
//  Copyright © 2016 Ivan Safrin. All rights reserved.
//

#include "Entity.hpp"

float lerp(float v0, float v1, float t) {
    return (1.0-t)*v0 + t*v1;
}

Entity::Entity():x(0),y(0),velocity_x(0),velocity_y(0),width(0),height(0),friction_x(2.0),friction_y(2.0){}

Entity::Entity(SheetSprite sprite):sprite(sprite)
{
    width = 0.25;
    height=0.25;
}

void Entity::Render(ShaderProgram* program)
{
    Matrix modelMatrix;
    
    modelMatrix.Translate(x, y, 0);
    program->setModelMatrix(modelMatrix);
    sprite.Draw(program);
}


void Entity::Update(float elapsed)
{
    
    collidedTop = false;
    collidedBottom = false;
    collidedLeft = false;
    collidedRight = false;
    
    velocity_x = lerp(velocity_x, 0.0f, FIXED_TIMESTEP * friction_x);
    velocity_y = lerp(velocity_y, 0.0f, FIXED_TIMESTEP * friction_y);
    velocity_x += acceleration_x * FIXED_TIMESTEP;
    velocity_y += acceleration_y * FIXED_TIMESTEP;
    y += velocity_y * FIXED_TIMESTEP;
    x += velocity_x * FIXED_TIMESTEP;
    velocity_y += -5 * elapsed;
    
}


bool Entity::collidesWith(Entity* object)
{
    
    
    
    if (x + width / 2 <= object->x - object->width / 2 || x - width / 2 >= object->x + object->width / 2 ||
        y + height / 2 <= object->y - object->height / 2 || y - height / 2 >= object->y + object->y / 2)
    {
        return false;
    }
    else
        return true;
}
