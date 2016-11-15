//
//  LevelEditor.hpp
//  NYUCodebase
//
//  Created by Ovais Ellahi on 11/7/16.
//  Copyright © 2016 Ivan Safrin. All rights reserved.
//

#ifndef LevelEditor_hpp
#define LevelEditor_hpp

#define SPRITE_COUNT_X 16
#define SPRITE_COUNT_Y 8
#define TILE_SIZE 0.25f
#define LEVEL_WIDTH 128
#define LEVEL_HEIGHT 32
#define ENEMIES 5


#include <stdio.h>
#include <fstream>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "Entity.hpp"



class LevelEditor{
public:
    LevelEditor(std::string fileName):fileName(fileName){enemies.resize(5);}
    
    void getData();
    bool readHeader(std::ifstream &stream);
    bool readLayerData(std::ifstream &stream);
    bool readEntityData(std::ifstream &stream);
    void placeEntity(std::string type, float x, float y);
    void buildLevel();
    void drawLevel(ShaderProgram* program, GLuint textureID);
    unsigned char** levelData;
    Entity player;
    int enemyIndex= 0;
    std::vector<Entity> enemies;
    std::vector<float> vertexData;
    std::vector<float> texCoordData;
    int mapWidth;
    int mapHeight;
    std::string fileName;
    
};

#endif /* LevelEditor_hpp */


