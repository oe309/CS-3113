//
//  SheetSprite.hpp
//  NYUCodebase
//
//  Created by Ovais Ellahi on 12/1/16.
//  Copyright © 2016 Ivan Safrin. All rights reserved.
//

#ifndef SheetSprite_hpp
#define SheetSprite_hpp

#include <stdio.h>
#include "ShaderProgram.h"

class SheetSprite {
public:
    SheetSprite();
    SheetSprite(uint textureID, int index, int spriteCountX=30, int spriteCountY=30,float width=0.25, float height=0.25, float size=0.25);
    void Draw(ShaderProgram *program);
    float size;
    unsigned int textureID;
    float u;
    float v;
    float width;
    int index;
    float height;
    int spriteCountY;
    int spriteCountX;
    
    
};


#endif /* SheetSprite_hpp */
