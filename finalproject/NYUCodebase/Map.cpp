//
//  Map.cpp
//  NYUCodebase
//
//  Created by Ovais Ellahi on 11/30/16.
//  Copyright © 2016 Ivan Safrin. All rights reserved.
//

#include "Map.hpp"


using namespace std;


Map::Map(std::string fileName, int sprite_count_x, int sprite_count_y, int nE, int nK, int nG):fileName(fileName),SPRITE_COUNT_X(sprite_count_x),SPRITE_COUNT_Y(sprite_count_y),nE(nE),nK(nK),nG(nG)
{
    enemies.resize(nE);
    keys.resize(nK);
    diamonds.resize(nG);
}
Map::Map(const Map& obj)
{
    levelData = obj.levelData;
    nG = obj.nG;
    nK = obj.nK;
    nE = obj.nE;
    vertexData =obj.vertexData;
    texCoordData = obj.texCoordData;
    player = obj.player;
    mapWidth =obj.mapWidth;
    mapHeight=obj.mapHeight;
    
}
void Map::drawLevel(ShaderProgram* program, GLuint textureID)
{
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUseProgram(program->programID);
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
    glEnableVertexAttribArray(program->positionAttribute);
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
    glEnableVertexAttribArray(program->texCoordAttribute);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glDrawArrays(GL_TRIANGLES, 0, 6*counter);
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
    
}
void Map::getData()
{
    ifstream infile(fileName);
    string line;
    
    while (getline(infile, line)) {
        if(line == "[header]") {
            if(!readHeader(infile)) {
                return; }
        } else if(line == "[layer]") {
            readLayerData(infile);
        } else if(line == "[Object Layer 1]") {
            readEntityData(infile);
        } }
    infile.close();
}

void Map::buildLevel()
{
    
    for(int y=0; y < mapHeight; y++) {
        for(int x=0; x < mapWidth; x++) {
            
            if(levelData[y][x]!=0){
                counter++;
                float u = ((float)(((int)levelData[y][x]) % SPRITE_COUNT_X) / (float) SPRITE_COUNT_X);
                float v = ((float)(((int)levelData[y][x]) / SPRITE_COUNT_X) / (float) SPRITE_COUNT_Y);
                float spriteWidth = 1.0f/(float)SPRITE_COUNT_X-2.0/688.0;
                float spriteHeight = 1.0f/(float)SPRITE_COUNT_Y-2.0/688.0;
                
          
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

bool Map::readHeader(std::ifstream &stream) {
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
        levelData = new int*[mapHeight];
        for(int i = 0; i < mapHeight; ++i) {
            levelData[i] = new int[mapWidth];
        }
        return true;
    }
}

bool Map::readLayerData(std::ifstream &stream) {
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
                    int val =  (int)atoi(tile.c_str());
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

void getType(const string type, Entity*ent)
{
    if(type=="Slug"){
        ent->enemyType = ENEMY_SLUG;
        ent->acceleration_x=0.75;
    }
    else if(type=="Slug2"){
        ent->enemyType = ENEMY_SLUG2;
        ent->health=15;
        ent->velocity_x = 0;
        ent->acceleration_x = 0;
    }
    else if(type=="Ghost"){
        ent->enemyType = ENEMY_GHOST;
        ent->acceleration_x=1.0;
    }
    
    else if(type == "Bee"){
        ent->enemyType = ENEMY_BEE;
        ent->acceleration_x=2.0;
        ent->gravity=0;
    }
    else if(type=="Spider"){
        ent->enemyType = ENEMY_SPIDER;
        ent->acceleration_x=2.0;
    }
    else if(type =="Fish" ){
        ent->enemyType = ENEMY_FISH;
        ent->y=0.25/2;
        ent->gravity=0;
        ent->acceleration_x=1.5;
    }
    else if(type=="Bat"){
        ent->enemyType = ENEMY_BAT;
        ent->acceleration_x=3.0;
        ent->gravity=0;
    }
    else if(type=="Snail"){
        ent->enemyType = ENEMY_SNAIL;
        ent->acceleration_x=3.0;
        
    }
    else if(type=="Snake")
    {
        ent->enemyType = ENEMY_SNAKE;
        ent->health=15;
        ent->acceleration_x=3.0;
    }
    else if(type=="Saw")
    {
        ent->enemyType = ENEMY_SAW;
        ent->acceleration_x=3.0;
    }


}

void Map::placeEntity(std::string type, float x, float y)
{
    if(type=="Player"){
        player.x = x;
        player.y = y;
        player.entityType = ENTITY_PLAYER;
        
      
    }
    else if(type=="Key")
    {
        keys[kInd%nK].x = x+TILE_SIZE/2;
        keys[kInd%nK].y = y;
        
        keys[gInd%nG].isStatic=true;
        
        keys[kInd%nK].entityType = ENTITY_KEY;
        kInd++;
    }
    else if(type=="Diamond")
    {
        diamonds[gInd%nG].x = x+TILE_SIZE/2;
        diamonds[gInd%nG].y=y;
        
        diamonds[gInd%nG].isStatic=true;
        diamonds[gInd%nG].entityType=ENTITY_DIAMOND;
        gInd++;
    }
    else
    {
        getType(type, &enemies[eInd%nE]);
        enemies[eInd%nE].x=x+TILE_SIZE/2;
        enemies[eInd%nE].y+=y;
        enemies[eInd%nE].entityType=ENTITY_ENEMY;
        eInd++;
    }
    
}
bool Map::readEntityData(std::ifstream &stream) {
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

