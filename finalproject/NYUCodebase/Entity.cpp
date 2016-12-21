//
//  Entity.cpp
//  NYUCodebase
//
//  Created by Ovais Ellahi on 11/30/16.
//  Copyright © 2016 Ivan Safrin. All rights reserved.
//

#include "Entity.hpp"

float lerp(float v0, float v1, float t) {
    return (1.0-t)*v0 + t*v1;
}


Entity::Entity():width(0),height(0),velocity_x(0),velocity_y(0),acceleration_x(0),acceleration_y(0),gravity(-5.0)
{
    collidesReset();
}

Entity::Entity(SheetSprite sprite):sprite(sprite),velocity_x(0),velocity_y(0),acceleration_x(0),acceleration_y(0),gravity(-5.0)

{
    collidesReset();
    width = sprite.width;
    height=sprite.height;
    
}


void Entity::Update(float elapsed)
{
    timeAlive+=elapsed;
    currInd++;
    velocity_x = lerp(velocity_x, 0.0f, elapsed * 3.0);
    velocity_y = lerp(velocity_y, 0.0f, elapsed * 1.0);
    velocity_x += acceleration_x * elapsed;
    velocity_y += acceleration_y * elapsed;
    y += velocity_y * elapsed;
    x += velocity_x * elapsed;
    velocity_y += gravity * elapsed;

}

void Entity::Render(ShaderProgram* program)
{
    modelMatrix.identity();
    modelMatrix.Translate(x, y, 0);
    modelMatrix.Scale(scale, 1.0,1.0);
    program->setModelMatrix(modelMatrix);
    sprite.Draw(program);
}
void Entity::collidesReset()
{
    collidedLeft = false;
    collidedRight = false;
    collidedTop = false;
    collidedBottom = false;
    
}
bool Entity::collidesWith(Entity * entity)
{
    float top = y + height/2.0f;
    float bot = y - height/2.0f;
    float left = x-width/2.0f;
    float right = x +width/2.0f;
    float entTop = entity->y+entity->height/2.0f;
    float entBot = entity->y-entity->height/2.0f;
    float entLeft = entity->x-entity->width/2.0f;
    float entRight =entity->x+entity->width/2.0f;
    float pen;
    if(top>entBot && bot< entTop)
    {
        if(bot<entBot)
        {
            
        }
        else if(bot > entBot)
        {
            
        }
    
    }
    if(right>entLeft && left< entRight)
    {
        if(left<entLeft)
        {
            
        }
        else if(left>entLeft)
        {
            
        }
    }
    
    return (top>entBot && bot< entTop)&&(right>entLeft && left< entRight);
    
}
