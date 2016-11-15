//
//  SheetSprite.cpp
//  NYUCodebase
//
//  Created by Ovais Ellahi on 11/7/16.
//  Copyright © 2016 Ivan Safrin. All rights reserved.
//

#include "SheetSprite.hpp"

SheetSprite:: SheetSprite(unsigned int textureID, int spriteCountX, int spriteCountY, int index, float size):textureID(textureID),size(size)
{
    u = (float)((index) % spriteCountX) / (float) spriteCountX;
    v = (float)((index) / spriteCountX) / (float) spriteCountY;
    width = 1.0f/(float)spriteCountX;
    height = 1.0f/(float)spriteCountY;

}

void SheetSprite::Draw(ShaderProgram *program) {
    glBindTexture(GL_TEXTURE_2D, textureID);
    GLfloat texCoords[] = {
        u, v+height,
        u+width, v,
        u, v,
        u+width, v,
        u, v+height,
        u+width, v+height
    };
    
    float vertices[] = {
        -0.5f * size, -0.5f * size,
        0.5f * size , 0.5f * size,
        -0.5f * size , 0.5f * size,
        0.5f * size , 0.5f * size,
        -0.5f * size , -0.5f * size ,
        0.5f * size , -0.5f * size};
    // draw our arrays
    glUseProgram(program->programID);
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}
