//
//  Entity.hpp
//  NYUCodebase
//
//  Created by Ovais Ellahi on 12/4/16.
//  Copyright © 2016 Ivan Safrin. All rights reserved.
//

#ifndef Entity_hpp
#define Entity_hpp

#include <stdio.h>
#include<vector>

#include "SheetSprite.hpp"

enum EntityType {ENTITY_PLAYER, ENTITY_ENEMY,
    ENTITY_DIAMOND,ENTITY_KEY};
class Vector{
public:
    Vector():x(0),y(0),z(0){};
    Vector(float x, float y, float z) :x(x), y(y), z(z){}
    void normalize();
    float x;
    float y;
    float z;
};
class Entity {
public:
    Entity();
    Entity(SheetSprite sprite);
    void Update(float elapsed);
    void Render(ShaderProgram *program);
    bool collidesWith(Entity *entity);
    void updateEdges();
    SheetSprite sprite;
    float x;
    float y;
    float width;
    float height;
    float velocity_x;
    float velocity_y;
    float acceleration_x;
    float acceleration_y;
    float gravity;
    float rotation = 0;
    bool isStatic;
    Matrix modelMatrix;
    EntityType entityType;
    bool collidedTop;
    bool collidedBottom;
    bool collidedLeft;
    bool collidedRight;
    std::vector<Vector> edges;
};


#endif /* Entity_hpp */
