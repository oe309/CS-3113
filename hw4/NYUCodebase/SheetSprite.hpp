//
//  SheetSprite.hpp
//  NYUCodebase
//
//  Created by Ovais Ellahi on 11/7/16.
//  Copyright © 2016 Ivan Safrin. All rights reserved.
//

#ifndef SheetSprite_hpp
#define SheetSprite_hpp

#include <stdio.h>
#include "ShaderProgram.h"


class SheetSprite {
public:
    SheetSprite(){}
    SheetSprite(unsigned int textureID, int spriteCountX, int spriteCountY, int index, float
                size);
    void Draw(ShaderProgram *program);
    float size;
    
    unsigned int textureID;
    float u;
    float v;
    float width;
    float height;
    
};

#endif /* SheetSprite_hpp */
