//
//  SheetSprite.hpp
//  NYUCodebase
//
//  Created by Ovais Ellahi on 12/4/16.
//  Copyright © 2016 Ivan Safrin. All rights reserved.
//

#ifndef SheetSprite_hpp
#define SheetSprite_hpp

#include <stdio.h>

#endif /* SheetSprite_hpp */


#include "ShaderProgram.h"


class SheetSprite {
public:
    SheetSprite();
    SheetSprite(unsigned int textureID, float u, float v, float width, float height, float
                size);
    
    
    void Draw(ShaderProgram *program);
    float size;
    unsigned int textureID;
    float u;
    float v;
    float width;
    float height;
};


