//
//  Bullet.cpp
//  NYUCodebase
//
//  Created by Ovais Ellahi on 11/30/16.
//  Copyright © 2016 Ivan Safrin. All rights reserved.
//

#include "Bullet.hpp"


Bullet::Bullet(){}

Bullet::Bullet(Entity bullet, float x,float y):bullet(bullet),x(x),y(y)
{
    for(int i =0; i<MAX_BULLETS;i++)
    {
        bullets[i] = Entity(bullet);
        bullets[i].x=-100;
        bullets[i].y = 0;
    }
}


void Bullet::shootBullet() {
    
    
    bullets[index].velocity_x=5.0;
    index++;
    if(index > MAX_BULLETS-1) {
        index = 0;
    }

}

void Bullet::Update(float elapsed)
{
    
    for(int i=0; i < MAX_BULLETS; i++) {
        bullets[i].Update(elapsed);
    }
}

void Bullet::Render(ShaderProgram *program)
{
    for(int i=0;i<MAX_BULLETS;i++)
    {
        bullets[i].Render(program);
    }
}
