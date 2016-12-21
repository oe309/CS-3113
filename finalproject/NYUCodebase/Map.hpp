//
//  Map.hpp
//  NYUCodebase
//
//  Created by Ovais Ellahi on 11/30/16.
//  Copyright © 2016 Ivan Safrin. All rights reserved.
//

#ifndef Map_hpp
#define Map_hpp

#define TILE_SIZE 0.25f


#include <stdio.h>
#include <fstream>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "Entity.hpp"


class GameEntities
{
public:
    GameEntities(int numEnemies, int numKeys, int numGems);
    int numEnemies;
    int numKeys;
    int numGems;
    int enemyInd;
    int keyInd;
    int gemInd;
    std::vector<Entity> enemies;
    std::vector<Entity> keys;
    std::vector<Entity> gems;
    Entity player;
    
    
};

class Map{
public:
    Map(std::string fileName, int sprite_count_x, int sprite_count_y,int nK, int nE, int nG);
    Map(const Map& obj);
    void getData();
    bool readHeader(std::ifstream &stream);
    bool readLayerData(std::ifstream &stream);
    bool readEntityData(std::ifstream &stream);
    void placeEntity(std::string type, float x, float y);
    void buildLevel();
    void drawLevel(ShaderProgram* program, GLuint textureID);
    int** levelData;
    std::vector<float> vertexData;
    std::vector<float> texCoordData;
    int mapWidth;
    int mapHeight;
    int SPRITE_COUNT_X;
    int SPRITE_COUNT_Y;
    std::vector<Entity> enemies;
    std::vector<Entity> diamonds;
    std::vector<Entity> keys;
    Entity player;
    int nE;
    int nK;
    int nG;
    int counter=0;
    int eInd = 0;
    int kInd = 0;
    int gInd = 0;
    std::string fileName;

};


#endif /* Map_hpp */
