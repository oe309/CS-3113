#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <vector>
#include "ShaderProgram.h"
#include "LevelEditor.hpp"
#include "Entity.hpp"


#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

SDL_Window* displayWindow;


void worldToTileCoordinates(float worldX, float worldY, int *gridX, int *gridY) {
    *gridX = (int)(worldX / TILE_SIZE);
    *gridY = (int)(-worldY / TILE_SIZE);
}
bool isSolid(int tile)
{
    std::vector<int> solids = {0,1,2,3,4,5,6,7,16,17,18,19,20,35,98,86,100,101};
    for (int i =0; i < solids.size();i++)
    {
        if(tile >0)
            if(tile == solids[i]) return true;
    }
    return false;
}
void collisionX(Entity& entity, unsigned char** levelData)
{
    float right =entity.x+TILE_SIZE/2;
    float left = entity.x-TILE_SIZE/2;
    int x,y;
    worldToTileCoordinates(right, entity.y, &x, &y);

        
        if(isSolid(levelData[y][x])){
            
            if(right > TILE_SIZE*x)
            {
                
                float penetration = fabs(right - TILE_SIZE*x);
                entity.x -= penetration+0.001;
                entity.velocity_x = 0;
                entity.collidedRight = true;
                
            }
}
    worldToTileCoordinates(left, entity.y, &x, &y);
   
        if(isSolid(levelData[y][x])){
            
            if(left > TILE_SIZE*x)
            {
                
                float penetration = fabs(left - TILE_SIZE*x);
                entity.x += penetration+0.001;
                entity.velocity_x = 0;
                entity.collidedLeft = true;
                
            }
        }
}

void collisionY(Entity& entity, unsigned char** levelData)
{
    float top =  entity.y +TILE_SIZE/2;
    float bottom = entity.y - TILE_SIZE/2;
    int x,y;
    worldToTileCoordinates(entity.x, top, &x, &y);
    
        if(isSolid(levelData[y][x])){
            
            if(top*-1 > TILE_SIZE*y)
            {
                
                float penetration = fabs(top + TILE_SIZE*y);
                entity.y -= penetration+0.001;
                entity.velocity_y = 0;
                entity.collidedTop = true;
                
            }
        
    }
    worldToTileCoordinates(entity.x, bottom, &x, &y);
    
        
        if(isSolid(levelData[y][x])){
            
            if(bottom*-1 > TILE_SIZE*y)
            {
                
                float penetration = fabs(bottom + TILE_SIZE*y);
                entity.y += penetration+0.001;
                entity.velocity_y = 0;
                entity.collidedBottom =true;
                
            }
        
    }
    
    

}



GLuint LoadTexture(const char *image_path) {
    SDL_Surface *surface = IMG_Load(image_path);
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, surface->pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    SDL_FreeSurface(surface);
    return textureID;
}

void DrawText(ShaderProgram *program, int fontTexture, std::string text, float size, float spacing) {
    float texture_size = 1.0/16.0f;
    std::vector<float> vertexData;
    std::vector<float> texCoordData;
    for(int i=0; i < text.size(); i++) {
        float texture_x = (float)(((int)text[i]) % 16) / 16.0f;
        float texture_y = (float)(((int)text[i]) / 16) / 16.0f;
        vertexData.insert(vertexData.end(), {
            ((size+spacing) * i) + (-0.5f * size), 0.5f * size,
            ((size+spacing) * i) + (-0.5f * size), -0.5f * size,
            ((size+spacing) * i) + (0.5f * size), 0.5f * size,
            ((size+spacing) * i) + (0.5f * size), -0.5f * size,
            ((size+spacing) * i) + (0.5f * size), 0.5f * size,
            ((size+spacing) * i) + (-0.5f * size), -0.5f * size,
        });
        texCoordData.insert(texCoordData.end(), {
            texture_x, texture_y,
            texture_x, texture_y + texture_size,
            texture_x + texture_size, texture_y,
            texture_x + texture_size, texture_y + texture_size,
            texture_x + texture_size, texture_y,
            texture_x, texture_y + texture_size,
        }); }
    glUseProgram(program->programID);
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
    glEnableVertexAttribArray(program->positionAttribute);
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
    glEnableVertexAttribArray(program->texCoordAttribute);
    glBindTexture(GL_TEXTURE_2D, fontTexture);
    glDrawArrays(GL_TRIANGLES, 0, text.size() * 6);
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}


int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
    glewInit();
#endif
    glViewport(0, 0, 640, 360);
    Matrix projectionMatrix;
    Matrix viewMatrix;
    Matrix modelMatrix;
    std::vector<Entity> entities;
    ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
    projectionMatrix.setOrthoProjection(-3.55, 3.55, -2.0f, 2.0f, -1.0f, 1.0f);
    glUseProgram(program.programID);
    GLuint font = LoadTexture("font1.png");
    GLuint map = LoadTexture("arne_sprites.png");
    LevelEditor game("NYUCodebase.app/Contents/Resources/map.txt");
    SheetSprite playerSprite(map,16,8,98,0.25);
    SheetSprite enemySprite(map, 16,8,86,0.25);
    for(int i=0;i<game.enemies.size();i++)
    {
        game.enemies[i] = Entity();
        game.enemies[i].sprite = enemySprite;
        game.enemies[i].width=TILE_SIZE;
        game.enemies[i].height = TILE_SIZE;
    }
    game.player = Entity();
    game.player.sprite = playerSprite;
    game.player.width =0.25;
    game.player.height = 0.25;
    game.getData();
    game.buildLevel();
    entities.push_back(game.player);
    for(int i=0; i < game.enemies.size();i++)
        entities.push_back(game.enemies[i]);

  
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    float lastFrameTicks = 0.0f;
    SDL_Event event;
    bool done = false;
    while (!done) {
        float ticks = (float)SDL_GetTicks() / 1000.0f;
        float elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        glClearColor(0, 0, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
            else if(event.type == SDL_KEYDOWN)
            {
                if(event.key.keysym.scancode==SDL_SCANCODE_SPACE)
                {
                    if(entities[0].collidedBottom)
                        entities[0].velocity_y = 6.0f;
                }
             

            }
            
            
        }
        
        
        for(int i = 0;i<entities.size();i++)
        {
            entities[i].collidedTop = false;
            entities[i].collidedBottom = false;
            entities[i].collidedLeft = false;
            entities[i].collidedRight = false;
            
        }
        
        float fixedElapsed = elapsed;
        if(fixedElapsed > FIXED_TIMESTEP * MAX_TIMESTEPS) {
            fixedElapsed = FIXED_TIMESTEP * MAX_TIMESTEPS;
        }

        for(int i=0; i < entities.size();i++)
        {
            while (fixedElapsed >= FIXED_TIMESTEP ) {
                fixedElapsed -= FIXED_TIMESTEP;
                entities[i].Update(FIXED_TIMESTEP);
                collisionX(entities[i],game.levelData);
                collisionY(entities[i],game.levelData);
            }
            entities[i].Update(fixedElapsed);
            collisionX(entities[i],game.levelData);
            collisionY(entities[i],game.levelData);
            
        }
        
        for(int i = 0; i < entities.size();i++)
        {
            if(entities[0].collidesWith(&entities[i]))
            {
                entities[i].x = 200;
            }
        }
        
        
        
        if(keys[SDL_SCANCODE_RIGHT])
        {
            entities[0].velocity_x = 1.5;
        }
        else if(keys[SDL_SCANCODE_LEFT])
        {
            entities[0].velocity_x=-1.5;
        }
        
  
        

        viewMatrix.identity();
        viewMatrix.Translate(-entities[0].x-3, -entities[0].y, 0);
        program.setModelMatrix(modelMatrix);
        program.setProjectionMatrix(projectionMatrix);
        program.setViewMatrix(viewMatrix);
        game.drawLevel(&program, map);
        
        for(int i =0; i < entities.size();i++)
        {
            entities[i].Render(&program);
        }
        
      
        
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        
        
        SDL_GL_SwapWindow(displayWindow);
    }
    
    SDL_Quit();
    return 0;
}
