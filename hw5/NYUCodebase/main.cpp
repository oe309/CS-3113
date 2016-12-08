#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "Entity.hpp"

#define PI 3.141592653589793238
#define FIXED_TIMESTEP 0.166666f
#define MAX_TIMESTEPS 6
#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

SDL_Window* displayWindow;

void Vector::normalize(){
    float len = sqrtf(x*x + y*y);
    x /= len;
    y /= len;
}

bool testSATSeparationForEdge(float edgeX, float edgeY, const std::vector<Vector> &points1, const std::vector<Vector> &points2) {
    float normalX = -edgeY;
    float normalY = edgeX;
    float len = sqrtf(normalX*normalX + normalY*normalY);
    normalX /= len;
    normalY /= len;
    
    std::vector<float> e1Projected;
    std::vector<float> e2Projected;
    
    for(int i=0; i < points1.size(); i++) {
        e1Projected.push_back(points1[i].x * normalX + points1[i].y * normalY);
    }
    for(int i=0; i < points2.size(); i++) {
        e2Projected.push_back(points2[i].x * normalX + points2[i].y * normalY);
    }
    
    std::sort(e1Projected.begin(), e1Projected.end());
    std::sort(e2Projected.begin(), e2Projected.end());
    
    float e1Min = e1Projected[0];
    float e1Max = e1Projected[e1Projected.size()-1];
    float e2Min = e2Projected[0];
    float e2Max = e2Projected[e2Projected.size()-1];
    float e1Width = fabs(e1Max-e1Min);
    float e2Width = fabs(e2Max-e2Min);
    float e1Center = e1Min + (e1Width/2.0);
    float e2Center = e2Min + (e2Width/2.0);
    float dist = fabs(e1Center-e2Center);
    float p = dist - ((e1Width+e2Width)/2.0);
    
    if(p < 0) {
        return true;
    }
    return false;
}

bool checkSATCollision(const std::vector<Vector> &e1Points, const std::vector<Vector> &e2Points) {
    for(int i=0; i < e1Points.size(); i++) {
        float edgeX, edgeY;
        
        if(i == e1Points.size()-1) {
            edgeX = e1Points[0].x - e1Points[i].x;
            edgeY = e1Points[0].y - e1Points[i].y;
        } else {
            edgeX = e1Points[i+1].x - e1Points[i].x;
            edgeY = e1Points[i+1].y - e1Points[i].y;
        }
        
        bool result = testSATSeparationForEdge(edgeX, edgeY, e1Points, e2Points);
        if(!result) {
            return false;
        }
    }
    for(int i=0; i < e2Points.size(); i++) {
        float edgeX, edgeY;
        
        if(i == e2Points.size()-1) {
            edgeX = e2Points[0].x - e2Points[i].x;
            edgeY = e2Points[0].y - e2Points[i].y;
        } else {
            edgeX = e2Points[i+1].x - e2Points[i].x;
            edgeY = e2Points[i+1].y - e2Points[i].y;
        }
        bool result = testSATSeparationForEdge(edgeX, edgeY, e1Points, e2Points);
        if(!result) {
            return false;
        }
    }
    return true;
}

GLuint LoadTexture(const char *image_path) {
    SDL_Surface *surface = IMG_Load(image_path);
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_BGRA,
                 GL_UNSIGNED_BYTE, surface->pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    SDL_FreeSurface(surface);
    return textureID;
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
    
    
    ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
    projectionMatrix.setOrthoProjection(-3.55, 3.55, -2.0f, 2.0f, -1.0f, 1.0f);
    glUseProgram(program.programID);

    GLuint sheet = LoadTexture("sheet.png");
    SheetSprite rock(sheet,224.0f/1024.0f,748.0f/1024.0f,101.0f/1024.0f,84.0f/1024.0f,0.6f);
    SheetSprite ship(sheet,211.0f/1024.0f,941.0f/1024.0f,99.0f/1024.0f,75.0f/1024.0f,1.0f);
    
    Entity player(ship);
    Entity rock1(rock);
    Entity rock2(rock);
    std::vector<Entity>entities;
    rock1.x = -2.5;
    rock1.y = 1.5;
    rock2.y  = -1.5;
    rock2.x=2.5;
    rock1.rotation = PI/2.0f;
    rock2.rotation = PI/3.0f;
    entities.push_back(player);
    entities.push_back(rock1);
    entities.push_back(rock2);
    
    float lastFrameTicks = 0.0f;
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    
    SDL_Event event;
    bool done = false;
    while (!done) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
            else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.scancode == SDL_SCANCODE_UP) {
                    
                    entities[0].velocity_y = 3.0;
                }
                else if(event.key.keysym.scancode == SDL_SCANCODE_DOWN)
                {
                    entities[0].velocity_y = -3.0;
                }
                else if(event.key.keysym.scancode == SDL_SCANCODE_A)
                {
                    entities[0].rotation +=0.2;
                }
                else if(event.key.keysym.scancode == SDL_SCANCODE_D)
                {
                    entities[0].rotation -=0.2;
                }
                else if(event.key.keysym.scancode == SDL_SCANCODE_LEFT)
                {
                    entities[0].velocity_x = -3.0;
                }
                else if(event.key.keysym.scancode == SDL_SCANCODE_RIGHT)
                {
                    entities[0].velocity_x = 3.0;
                }
        }
        }
        glClear(GL_COLOR_BUFFER_BIT);
        
        float ticks = (float)SDL_GetTicks() / 1000.0f;
        float elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;
        
        float fixedElapsed = elapsed;
        
        if(fixedElapsed > FIXED_TIMESTEP * MAX_TIMESTEPS) {
            fixedElapsed = FIXED_TIMESTEP * MAX_TIMESTEPS;
        }
        while (fixedElapsed >= FIXED_TIMESTEP ) {
            fixedElapsed -= FIXED_TIMESTEP;
            player.Update(fixedElapsed);
        }
        
        for(int i =0;i<entities.size();i++)
        {
            entities[i].Update(elapsed);
            entities[i].updateEdges();
        }
        
        for (int i = 0; i < entities.size(); ++i) {
            for (int j = i + 1; j < entities.size(); ++j) {
                int maxChecks = 10;
                while (checkSATCollision(entities[i].edges, entities[j].edges) && maxChecks > 0) {
                    
                    Vector responseVector = Vector(entities[i].x - entities[j].x, entities[i].y - entities[j].y, 0);
                    responseVector.normalize();
                    entities[i].x += responseVector.x * 0.002;
                    entities[i].y += responseVector.y * 0.002;
                    
                    entities[j].x -= responseVector.x * 0.002;
                    entities[j].y -= responseVector.y * 0.002;
                    maxChecks -= 1;
                }
            }
        }
       
        program.setViewMatrix(viewMatrix);
        program.setProjectionMatrix(projectionMatrix);
        
        for(Entity ent:entities)
            ent.Render(&program);

        
        
        
        SDL_GL_SwapWindow(displayWindow);
    }
    
    SDL_Quit();
    return 0;
}
