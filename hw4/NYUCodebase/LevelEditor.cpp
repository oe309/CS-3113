//
//  LevelEditor.cpp
//  NYUCodebase
//
//  Created by Ovais Ellahi on 11/7/16.
//  Copyright © 2016 Ivan Safrin. All rights reserved.
//

#include "LevelEditor.hpp"

using namespace std;

void LevelEditor::drawLevel(ShaderProgram* program, GLuint textureID)
{
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUseProgram(program->programID);
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
    glEnableVertexAttribArray(program->positionAttribute);
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
    glEnableVertexAttribArray(program->texCoordAttribute);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glDrawArrays(GL_TRIANGLES, 0, 6*mapWidth*mapHeight);
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);

}
void LevelEditor::getData()
{
    ifstream infile(fileName);
    string line;
    
    while (getline(infile, line)) {
        if(line == "[header]") {
            if(!readHeader(infile)) {
                return; }
        } else if(line == "[layer]") {
            readLayerData(infile);
        } else if(line == "[ObjectsLayer]") {
            readEntityData(infile);
        } }
    infile.close();
}

void LevelEditor::buildLevel()
{
        for(int y=0; y < mapHeight; y++) {
        for(int x=0; x < mapWidth; x++) {
          
            if(levelData[y][x]!=0){
            float u = (float)(((int)levelData[y][x]) % SPRITE_COUNT_X) / (float) SPRITE_COUNT_X;
            float v = (float)(((int)levelData[y][x]) / SPRITE_COUNT_X) / (float) SPRITE_COUNT_Y;
            float spriteWidth = 1.0f/(float)SPRITE_COUNT_X;
            float spriteHeight = 1.0f/(float)SPRITE_COUNT_Y;
            
            vertexData.insert(vertexData.end(), {
                TILE_SIZE * x, -TILE_SIZE * y,
                TILE_SIZE * x, (-TILE_SIZE * y)-TILE_SIZE,
                (TILE_SIZE * x)+TILE_SIZE, (-TILE_SIZE * y)-TILE_SIZE,
                TILE_SIZE * x, -TILE_SIZE * y,
                (TILE_SIZE * x)+TILE_SIZE, (-TILE_SIZE * y)-TILE_SIZE,
                (TILE_SIZE * x)+TILE_SIZE, -TILE_SIZE * y
            });
            texCoordData.insert(texCoordData.end(), {
                u, v,
                u, v+(spriteHeight),
                u+spriteWidth, v+(spriteHeight),
                u, v,
                u+spriteWidth, v+(spriteHeight),
                u+spriteWidth, v
            });
            
    }
        }
}
}

bool LevelEditor::readHeader(std::ifstream &stream) {
    string line;
    mapWidth = -1;
    mapHeight = -1;
    while(getline(stream, line)) {
        if(line == "") { break; }
        istringstream sStream(line);
        string key,value;
        getline(sStream, key, '=');
        getline(sStream, value);
        if(key == "width") {
            mapWidth = atoi(value.c_str());
        } else if(key == "height"){
            mapHeight = atoi(value.c_str());
        } }
    if(mapWidth == -1 || mapHeight == -1) {
        return false;
    } else { // allocate our map data
        levelData = new unsigned char*[mapHeight];
        for(int i = 0; i < mapHeight; ++i) {
            levelData[i] = new unsigned char[mapWidth];
        }
        return true;
    }
}

bool LevelEditor::readLayerData(std::ifstream &stream) {
    string line;
    while(getline(stream, line)) {
        if(line == "") { break; }
        istringstream sStream(line);
        string key,value;
        getline(sStream, key, '=');
        getline(sStream, value);
        if(key == "data") {
            for(int y=0; y < mapHeight; y++) {
                getline(stream, line);
                istringstream lineStream(line);
                string tile;
                
                for(int x=0; x < mapWidth; x++) {
                    getline(lineStream, tile, ',');
                    unsigned char val =  (unsigned char)atoi(tile.c_str());
                    if(val > 0) {
                        // be careful, the tiles in this format are indexed from 1 not 0
                        levelData[y][x] = val-1;
                    } else {
                        levelData[y][x] = 0;
                    }
                }
            } }
    }
    return true;
}

void LevelEditor::placeEntity(std::string type, float x, float y)
{
    
    if(type =="Shooter")
    {
        player.x = x;
        player.y = y;
        
    }
    else if(type =="Key")
    {
        enemies[enemyIndex%ENEMIES].x =x;
        enemies[enemyIndex%ENEMIES].y=y;
        enemyIndex++;
    }
}
bool LevelEditor::readEntityData(std::ifstream &stream) {
    string line;
    string type;
    while(getline(stream, line)) {
        if(line == "") { break; }
        istringstream sStream(line);
        string key,value;
        getline(sStream, key, '=');
        getline(sStream, value);
        if(key == "type") {
            type = value;
        } else if(key == "location") {
            istringstream lineStream(value);
            string xPosition, yPosition;
            getline(lineStream, xPosition, ',');
            getline(lineStream, yPosition, ',');
            float placeX = atoi(xPosition.c_str())*TILE_SIZE;
            float placeY = atoi(yPosition.c_str())*-TILE_SIZE;
            placeEntity(type, placeX, placeY);
        }
    }
    return true;
}




