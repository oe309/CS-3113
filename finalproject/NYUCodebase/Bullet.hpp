//
//  Bullet.hpp
//  NYUCodebase
//
//  Created by Ovais Ellahi on 11/30/16.
//  Copyright © 2016 Ivan Safrin. All rights reserved.
//

#ifndef Bullet_hpp
#define Bullet_hpp
#define MAX_BULLETS 30

#include <stdio.h>
#include "Entity.hpp"

class Bullet
{
public:
    Bullet();
    Bullet(Entity bullet,float x,float y);
    Entity bullet;
    Entity bullets[MAX_BULLETS];
    int index=0;
    float x;
    float y;
    
    void shootBullet();
    
    void Update(float elapsed);
    void Render(ShaderProgram*program);
    
};


#endif /* Bullet_hpp */
