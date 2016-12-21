//
//  Entity.hpp
//  NYUCodebase
//
//  Created by Ovais Ellahi on 11/30/16.
//  Copyright © 2016 Ivan Safrin. All rights reserved.
//

#ifndef Entity_hpp
#define Entity_hpp

#include <stdio.h>

#include "SheetSprite.hpp"

enum EntityType {ENTITY_PLAYER, ENTITY_ENEMY,
    ENTITY_DIAMOND,ENTITY_KEY};
enum EnemyType {ENEMY_NULL, ENEMY_SLUG, ENEMY_SLUG2, ENEMY_FISH, ENEMY_BEE,
    ENEMY_GHOST, ENEMY_SPIDER,ENEMY_BAT, ENEMY_SAW, ENEMY_SNAKE, ENEMY_SNAIL
};
class Entity {
public:
    Entity();
    Entity(SheetSprite sprite);
    void Update(float elapsed);
    void Render(ShaderProgram *program);
    bool collidesWith(Entity *entity);
    SheetSprite sprite;
    void collidesReset();
    Matrix modelMatrix;
    float scale =1;
    float x;
    float y;
    float width;
    float height;
    float velocity_x;
    float velocity_y;
    float acceleration_x;
    float acceleration_y;
    float gravity;
    int health = 30;
    
    int damage = 5;
    float timeAlive=0;
    bool isStatic = true;
    EntityType entityType;
    EnemyType enemyType;
    bool isDead = false;
    bool isActive = false;
    bool collidedTop;
    bool collidedBottom;
    bool collidedLeft;
    bool jmp;
    bool collidedRight;
    int * animation;
    int currInd =0;
};


#endif /* Entity_hpp */
