//
//  Entity.cpp
//  NYUCodebase
//
//  Created by Ovais Ellahi on 12/4/16.
//  Copyright © 2016 Ivan Safrin. All rights reserved.
//

#include "Entity.hpp"

float lerp(float v0, float v1, float t) {
    return (1.0-t)*v0 + t*v1;
}


Entity::Entity():x(0),y(0),width(0),height(0),velocity_x(0),velocity_y(0),gravity(-5.0),acceleration_x(0),acceleration_y(0)
{
    collidedLeft = false;
    collidedRight = false;
    collidedTop = false;
    collidedBottom = false;
}

Entity::Entity(SheetSprite sprite):x(0),y(0),velocity_x(0),velocity_y(0),gravity(-5.0),acceleration_x(0),acceleration_y(0),sprite(sprite)
{
    width = sprite.width/sprite.height*sprite.size;
    height = sprite.size;
    edges.resize(4);
    std::vector<Vector>p2;

    
    
    collidedLeft = false;
    collidedRight = false;
    collidedTop = false;
    collidedBottom = false;
}
void Entity::updateEdges()
{
    edges[0].x = x+width/2.0f;
    edges[0].y= y-height/2.0f;
    edges[1].x = x-width/2.0f;
    edges[1].y= y+height/2.0f;
    edges[2].x = x+width/2.0f;
    edges[2].y= y+height/2.0f;
    edges[3].x = x-width/2.0f;
    edges[3].y= y-height/2.0f;

}
void Entity::Update(float elapsed)
{
    collidedLeft = false;
    collidedRight = false;
    collidedTop = false;
    collidedBottom = false;
    velocity_x = lerp(velocity_x, 0.0f, elapsed * 3.0);
    velocity_y = lerp(velocity_y, 0.0f, elapsed * 3.0);
    velocity_x += acceleration_x * elapsed;
    velocity_y += acceleration_y * elapsed;
    y += velocity_y * elapsed;
    x += velocity_x * elapsed;
    

}

void Entity::Render(ShaderProgram *program)
{
    modelMatrix.identity();
    modelMatrix.Translate(x, y, 0);
    modelMatrix.Rotate(rotation);
    program->setModelMatrix(modelMatrix);
    sprite.Draw(program);
    
}

bool Entity::collidesWith(Entity *entity)
{
    float top = y + height/2.0f;
    float bot = y - height/2.0f;
    float left = x-width/2.0f;
    float right = x +width/2.0f;
    float entTop = entity->y+entity->height/2.0f;
    float entBot = entity->y-entity->height/2.0f;
    float entLeft = entity->x-entity->width/2.0f;
    float entRight =entity->x+entity->width/2.0f;
    
    if(top>entBot && bot< entTop)
    {
        if(bot<entBot)
        {
            collidedTop=true;
        }
        if(bot > entBot)
        {
            collidedBottom=true;
        }
        
    }
    if(right>entLeft && left< entRight)
    {
        if(left<entLeft)
        {
            collidedRight=true;
        }
        if(left>entLeft)
        {
            collidedLeft=true;
        }
    }
    
    return (collidedTop||collidedBottom || collidedLeft || collidedRight);
}
