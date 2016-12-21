#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include "Entity.hpp"
#include "Map.hpp"
#define MAX_BULLETS 30
#define FIXED_TIMESTEP 0.1666f
#define MAX_TIMESTEPS 6

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

bool invWall(int tile)
{
    return tile==200;
}

bool isSolid(int tile)
{
    std::vector<int> solids = {69,68,99,98,480,481,482,483,484,485,486,487,488,489,490,491,492,493,494,510,511,512,513,514,515,516,517,518,519,570,571,572,573,574,575,576,426,427,428,429,430,431,432,425,122,123,124,125,126,127,128,129,130,131,132,133,121,152,153,154,155,156,157,158,159,160,161,162,163,151,191,192,193,190,182,183,184,185,186,187,188,189,181,212,213,214,215,216,217,218,219,211,735,736,737,738,739,740,741,742,734,765,766,767,768,769,770,771,772,764,795,796,797,794,825,824,801,802,800,13,14,43,199};
    for (int i =0; i < solids.size();i++)
    {
        if(tile >0)
            if(tile == solids[i]) return true;
    }
    return false;
}
bool isUnderWater(int tile)
{
    std::vector<int>water = {547,548,549,550,577,578,579,580};
    for(int i=0;i<water.size();i++)
    {
        if(tile>0)
            if(tile==water[i]) return true;
    }
    return false;
}
bool hitSpike(int tile)
{
    std::vector<int>spike = {570,571,572,573,574,575,576,13,14,43};
    for(int i=0; i<spike.size();i++)
    {
        if(tile>0)
            if (tile==spike[i]) return true;
    }
    return false;
}
void collisionX(Entity& entity, int** levelData, int gameW, int gameH)
{
    
    float right =entity.x+entity.width/2;
    float left = entity.x-entity.width/2;
    
    int x,y;
    worldToTileCoordinates(right, entity.y, &x, &y);
    
    if(x>=0 && x<gameW&&y>=0 && y<gameH){
        if(isSolid(levelData[y][x])||(invWall(levelData[y][x])&&entity.entityType==ENTITY_ENEMY)){
            
            if(right > TILE_SIZE*x)
            {
                
                float penetration = fabs(right - TILE_SIZE*x);
                entity.x -= penetration+0.00001;
                if(entity.entityType!=ENTITY_ENEMY)entity.velocity_x = 0;
                
                entity.collidedRight = true;
            
            }
        }
        if(entity.entityType==ENTITY_PLAYER && isUnderWater(levelData[y][x]))
        {
            entity.gravity = -1;
        }
        else if(entity.entityType==ENTITY_PLAYER && !isUnderWater(levelData[y][x]))
        {
            entity.gravity=-5;
        }
    }
    
    worldToTileCoordinates(left, entity.y, &x, &y);
       if(x>=0 && x<gameW&&y>=0 && y<gameH){
    if(isSolid(levelData[y][x])||(invWall(levelData[y][x])&&entity.entityType==ENTITY_ENEMY)){
        
        if(left < TILE_SIZE*x+TILE_SIZE)
        {
            
            float penetration = fabs(left - TILE_SIZE*x-TILE_SIZE);
            entity.x += penetration+0.00001;
            if(entity.entityType!=ENTITY_ENEMY)entity.velocity_x = 0;
            
            entity.collidedLeft = true;
            
        }
    }
       }
    

    
}

void collisionY(Entity& entity, int** levelData, int gameW,int gameH)
{
    
    float top =  entity.y +entity.height/2;
    float bottom = entity.y - entity.height/2;
    int x,y;
    worldToTileCoordinates(entity.x, top, &x, &y);
       if(y>=0 && y<gameH &&x>=0 && x<gameW){
    if(isSolid(levelData[y][x])){
        
        if(top > (-TILE_SIZE*y)-TILE_SIZE)
        {
            
            float penetration = fabs(top + TILE_SIZE*y+TILE_SIZE);
            entity.y -= penetration+0.00001;
            entity.velocity_y = 0;
            entity.collidedTop = true;
            
        }
        
    }
       }
    worldToTileCoordinates(entity.x, bottom, &x, &y);
    
    if(y>=0 && y<gameH&&x>=0 && x<gameW){
    if(isSolid(levelData[y][x])){
        
        if(bottom < -TILE_SIZE*y)
        {
            
            float penetration = fabs(bottom + TILE_SIZE*y);
            entity.y += penetration+0.00001;
            entity.velocity_y = 0;
            entity.collidedBottom =true;
            
        
            if(entity.entityType==ENTITY_PLAYER && hitSpike(levelData[y][x])){
                entity.x = 3*TILE_SIZE;
                entity.y = -20*TILE_SIZE;
                if(entity.health>=10)
                    entity.health-=10;
                else
                    entity.health-=5;
            }
        }
        
    }
    }
    else if(bottom < - TILE_SIZE*gameH&&entity.entityType==ENTITY_PLAYER){
        entity.x = 3*TILE_SIZE;
        entity.y=-20*TILE_SIZE;
        if(entity.health>=10)
            entity.health-=10;
        else
            entity.health-=5;

    }
}


void draw(ShaderProgram *program, int textureID)
{
    
    float vertexCoords[]={-0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5};
    float texCoords[] = {0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0};
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUseProgram(program->programID);
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertexCoords);
    glEnableVertexAttribArray(program->positionAttribute);
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

void numSprite(uint textureID,int num, Entity & entity)
{
    if(num <= 5)
    {
        SheetSprite n(textureID,num+434);
        entity.sprite =n;
    }
    else if (num > 5)
    {
        SheetSprite n(textureID,num+463-6);
        entity.sprite=n;
    }
    
}
void setKeyDiam(uint textureID,int numKey,int numDiam,Entity keyDiam[], const Entity player)
{
    SheetSprite keyim(textureID,404);
    SheetSprite diamIm(textureID,377);
    numSprite(textureID, numKey, keyDiam[1]);
    numSprite(textureID, numDiam, keyDiam[3]);
    keyDiam[0].sprite = keyim;
    keyDiam[2].sprite = diamIm;
    for(int i =0; i < 4;i++)
    {
        keyDiam[i].x =player.x+TILE_SIZE*(i-1);
        keyDiam[i].y =player.y+TILE_SIZE*7;
    }
    
    
}
void setLives(uint textureID,Entity lives[], const Entity player)
{
    int wholeLives = player.health/10;
    int halfLives = (player.health-wholeLives*10)/5;
    int noLives = 3 - (wholeLives+halfLives);
    for(int i =0;i<3;i++)
    {
        if(i<noLives)
        {
            SheetSprite emptyHeart(textureID,375);
            lives[i].y = player.y +TILE_SIZE*7;
            lives[i].x = player.x+TILE_SIZE*(22-i);
            lives[i].sprite=emptyHeart;
        }
        else if (i>=noLives && i<halfLives+noLives)
        {
            SheetSprite halfHeart(textureID,374);
            lives[i].y = player.y +TILE_SIZE*7;
            lives[i].x = player.x+TILE_SIZE*(22-i);
            lives[i].sprite=halfHeart;

        }
        else if (i>=halfLives+ noLives && i<halfLives+noLives+wholeLives)
        {
            SheetSprite fullHeart(textureID,373);
            lives[i].y = player.y +TILE_SIZE*7;
            lives[i].x = player.x+TILE_SIZE*(22-i);
            lives[i].sprite=fullHeart;

        }
    }
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
void shootBullet(uint textureID,const Entity player, std::vector<Entity>&bullets)
{
    SheetSprite bull(textureID,896,30,30,TILE_SIZE/2,TILE_SIZE/2,TILE_SIZE/2);
    Entity newBullet(bull);
    newBullet.x = player.x+newBullet.width/2;
    newBullet.y=player.y;
    newBullet.gravity=0;
    if(player.scale==1) newBullet.velocity_x=10;
    else newBullet.velocity_x=-10;
    bullets.push_back(newBullet);
}
enum GameState { STATE_GAME_TITLE, STATE_GAME_INS, STATE_GAME_LVL1, STATE_GAME_LVL2, STATE_GAME_LVL3, STATE_GAME_WIN, STATE_GAME_LOSE};
GameState state = STATE_GAME_TITLE;
Mix_Chunk *keyS;
Mix_Chunk *jump;
Mix_Chunk *diamondS;
Mix_Chunk *enemydie;
Mix_Chunk *shootS;
Mix_Chunk *gethit;
Mix_Chunk *fallhit;
Mix_Chunk *winlvl;
Mix_Music *win;
Mix_Music *loser;
Mix_Music *titlee;
Mix_Music *lvl1;
Mix_Music *lvl2;
Mix_Music *lvl3;
Mix_Music *transs;


void switchState(GameState newState){
    state = newState;
    if (newState == STATE_GAME_TITLE){
        Mix_PlayMusic(titlee, -1);
        
    }
    else if (newState == STATE_GAME_LVL1){
        Mix_VolumeMusic(50);
        Mix_PlayMusic(lvl1, -1);
    }
    else if (newState == STATE_GAME_LVL2){
        Mix_PlayMusic(lvl2, -1);
    }
    else if (newState == STATE_GAME_LVL3){
        Mix_PlayMusic(lvl3, -1);
    }
    else if (newState == STATE_GAME_LOSE){
        Mix_PlayMusic(loser, -1);
    }
    else if (newState == STATE_GAME_WIN){
        Mix_PlayMusic(win, -1);
    }
}

std::vector<Entity> levelEntities(Map & game, uint textId)
{
    std::vector<Entity>entities;
    
    SheetSprite player(textId,20);
    SheetSprite slug(textId,231);
    SheetSprite bee(textId,355);
    SheetSprite slug2(textId,449);
    SheetSprite diamond(textId,286);
    SheetSprite key(textId,15);
    SheetSprite spider(textId,472);
    SheetSprite ghost(textId,447);
    SheetSprite fish(textId,381);
    SheetSprite bullet(textId,711);
    SheetSprite saw(textId,235);
    SheetSprite snail(textId,415);
    SheetSprite snake(textId,295);
    Entity play(player);
    
    game.player = play;
    for(int i =0;i < game.nK;i++)
    {
        game.keys[i] = Entity(key);
        
    }
    for(int i =0; i<game.nE;i++)
    {
        game.enemies[i] = Entity(bee);
    }
    for(int i =0; i<game.nG;i++)
    {
        game.diamonds[i] = Entity(diamond);
    }
    game.getData();
    game.buildLevel();
    
    entities.push_back(game.player);
    for(int i =0;i < game.nK;i++)
    {
        entities.push_back(game.keys[i]);
        
    }
    for(int i =0; i<game.nE;i++)
    {
        
        if(game.enemies[i].enemyType == ENEMY_SLUG) game.enemies[i].sprite = slug;
        else if(game.enemies[i].enemyType == ENEMY_SLUG2) game.enemies[i].sprite =slug2;
        else if(game.enemies[i].enemyType == ENEMY_GHOST)  game.enemies[i].sprite =ghost;
        else if(game.enemies[i].enemyType == ENEMY_BEE)game.enemies[i].sprite =bee;
        else if(game.enemies[i].enemyType == ENEMY_SPIDER)game.enemies[i].sprite =spider;
        else if(game.enemies[i].enemyType == ENEMY_FISH)game.enemies[i].sprite =fish;
        else if(game.enemies[i].enemyType==ENEMY_SAW)game.enemies[i].sprite=saw;
        else if(game.enemies[i].enemyType==ENEMY_SNAIL)game.enemies[i].sprite=snail;
        else if(game.enemies[i].enemyType==ENEMY_SNAKE)game.enemies[i].sprite=snake;
        
        entities.push_back(game.enemies[i]);
    }
    for(int i =0; i<game.nG;i++)
    {
        entities.push_back(game.diamonds[i]);
    }


    
    return entities;
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
    
    ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
    projectionMatrix.setOrthoProjection(-3.55, 3.55, -2.0f, 2.0f, -1.0f, 1.0f);
    glUseProgram(program.programID);
    
    GLuint map2 = LoadTexture("spritesheet_rgba.png");
    GLuint back2 = LoadTexture("spritesheet2.png");
    GLuint font = LoadTexture("font2.png");
    GLuint title = LoadTexture("mainpage.png");
    GLuint arrowkeys = LoadTexture("arrowkeys.png");
    GLuint obj = LoadTexture("obj.png");
    GLuint gems = LoadTexture("gems.png");
    GLuint losep = LoadTexture("lose1.png");
    GLuint lose = LoadTexture("lose2.png");
    
    std::vector<Entity>entities;
    
    
    SheetSprite origChar(map2, 19, 30, 30, TILE_SIZE, TILE_SIZE, 1.0);
    SheetSprite lvlUP1(map2, 79, 30, 30, TILE_SIZE, TILE_SIZE, 1.0);
    SheetSprite lvlUP3(map2, 109, 30, 30, TILE_SIZE, TILE_SIZE, 1.0);
    SheetSprite lvlUP2(map2, 49, 30, 30, TILE_SIZE, TILE_SIZE, 1.0);
    SheetSprite red(map2, 896, 30, 30, TILE_SIZE, TILE_SIZE, 1.0);
    SheetSprite blue(map2, 897, 30, 30, TILE_SIZE, TILE_SIZE, 1.0);
    SheetSprite yellow(map2, 866, 30, 30, TILE_SIZE, TILE_SIZE, 1.0);
    SheetSprite s1(map2,19,30,30,1,1,1);
    SheetSprite wT(map2,238,30,30,1,1,1);
    SheetSprite wB(map2,268,30,30,1,1,1);
    
    
    
    SheetSprite bullet(map2,711);
    SheetSprite gun1(map2,708);
    Entity gun(gun1);
    
    Entity lives[3];
    Entity keyDiam[4];
    Map game2(RESOURCE_FOLDER"level2.txt",30,30,23,6,5);
    Map game(RESOURCE_FOLDER"level1.txt",30,30,16,6,5);
    Map game3(RESOURCE_FOLDER"level3.txt",30,30,14,6,5);
    Map back(RESOURCE_FOLDER"back2.txt",30,16,0,0,0);
    std::vector<Entity> b2 = levelEntities(back, back2);
    std::vector<Entity> level1ent = levelEntities(game,map2);
    std::vector<Entity> level2ent = levelEntities(game2, map2);
    std::vector<Entity> level3ent = levelEntities(game3, map2);
    
    
    Entity bullets[MAX_BULLETS];
    int bullInd=0;
    for(int i =0; i<MAX_BULLETS;i++)
    {
        bullets[i] = Entity(bullet);
        bullets[i].gravity=0;
        bullets[i].x=-2000;
        bullets[i].y = 0;
        bullets[i].velocity_y=0;
        
    }
    
       const Uint8 *keys = SDL_GetKeyboardState(NULL);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    keyS = Mix_LoadWAV("key.wav");
    jump = Mix_LoadWAV("jump.wav");
    diamondS = Mix_LoadWAV("diamond.wav");
    enemydie = Mix_LoadWAV("enemydie.wav");
    shootS = Mix_LoadWAV("shoot.wav");
    gethit = Mix_LoadWAV("gethit.wav");
    fallhit = Mix_LoadWAV("fallhit.wav");
    winlvl = Mix_LoadWAV("winlvl.wav");
    win = Mix_LoadMUS("win.mp3");
    loser = Mix_LoadMUS("lose.mp3");
    titlee = Mix_LoadMUS("title.mp3");
    lvl1 = Mix_LoadMUS("lvl1.mp3");
    lvl2 = Mix_LoadMUS("lvl2.mp3");
    lvl3 = Mix_LoadMUS("lvl3.mp3");
    transs = Mix_LoadMUS("transs.mp3");
    float tim=0;
    switchState(STATE_GAME_TITLE);
    bool win=false;
    float lose1=0;
    float lose2=0;
    const int runAnimation[] = {28,28,29,29};
    const int slugAnim[] = {229,229,230,230,231,231};
    const int slug2Anim[] = {718,718,449,449};
    const int spiderAnim[] = {470,471,472};
    const int ghostAnim[] = {445,445,445,446,446,446};
    const int beeAnim[] = {354,355};
    const int fishAnim[] ={380,381};
    const int sawAnim[] = {234,234,235,235};
    const int snakeAnim[] ={294,294,295,295};
    const int snailAnim[] ={414,414,415,415};
    const int anim[] = {19,21};
    const int wtAnim[] = {238,239};
    const int wbAnim[] = {268,269};
    float viewX;
    float viewY;
    int entX=0;
    int numKeys, numDiam;
    int entY= 0;
    int currentIndex = 0;
    float animationElapsed=0;
    bool changeLevel= false;
     float lastFrameTicks = 0.0f;
    SDL_Event event;
    bool done = false;
    while (!done) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
                done = true;
            }
            if(state==STATE_GAME_LVL1||state==STATE_GAME_LVL2||state == STATE_GAME_LVL3)
            {
            if(event.type==SDL_KEYDOWN)
            {
                
                if(event.key.keysym.scancode==SDL_SCANCODE_SPACE)
                {
                 
                    if(!bullets[(bullInd-1)%MAX_BULLETS].isActive  || bullInd==0){
                        Mix_PlayChannel(-1, shootS, 0);
                        bullets[bullInd%MAX_BULLETS].isActive=true;
                        bullets[bullInd%MAX_BULLETS].x = gun.x;
                        bullets[bullInd%MAX_BULLETS].y=gun.y;
                        if(entities[0].scale==1)bullets[bullInd%MAX_BULLETS].velocity_x=10.0;
                        else if(entities[0].scale==-1)bullets[bullInd%MAX_BULLETS].velocity_x=-10.0;
                        bullInd++;
                    }
                }
            }
            else if(event.type==SDL_KEYUP)
            {
                entities[0].acceleration_x =0;
                
            }
        }
            
    }
        glClearColor(201.0f/255.0f,242.0f/255.0f,246.0f/255.0f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        float ticks = (float)SDL_GetTicks() / 1000.0f;
        float elapsed = ticks - lastFrameTicks;
        lastFrameTicks = ticks;

        float fixedElapsed = elapsed;
        if(fixedElapsed > FIXED_TIMESTEP * MAX_TIMESTEPS) {
            fixedElapsed = FIXED_TIMESTEP * MAX_TIMESTEPS;
        }
        animationElapsed += elapsed;
        if(animationElapsed > FIXED_TIMESTEP) {
            currentIndex++;
            animationElapsed = 0.0;
        }
        switch (state){
            case(STATE_GAME_TITLE) :
                
                if (keys[SDL_SCANCODE_RETURN]) {
                    switchState(STATE_GAME_INS);
                }
                break;
            case(STATE_GAME_INS) :
                
                if (keys[SDL_SCANCODE_SPACE]){
                    numKeys=game.nK;
                    numDiam=game.nG;
                    entities.clear();
                    entities = level1ent;
                    
                    switchState(STATE_GAME_LVL1);//for now
                }
                break;
                
            case(STATE_GAME_LVL1) :

                if (changeLevel){
                    entities.clear();
                    numKeys=game2.nK;
                    numDiam=game2.nG;
                    changeLevel=false;  
                    entities=level2ent;
                    for(int i=0; i<entities.size();i++)
                    {
                        if(entities[i].entityType==ENTITY_KEY)
                            entities[i].sprite.index +=30;
                        if(entities[i].entityType==ENTITY_DIAMOND)
                            entities[i].sprite.index +=2;
                    }
                    
                    entities[0].damage=5;
                    switchState(STATE_GAME_LVL2);
                }
                if(keys[SDL_SCANCODE_LEFT])
                {
                    
                    entities[0].scale = -1.0;
                    entities[0].acceleration_x = -5.0;
                    entities[0].sprite.index = runAnimation[currentIndex%4];
                }
                else if(keys[SDL_SCANCODE_RIGHT])
                {
                    
                    entities[0].scale = 1.0;
                    entities[0].acceleration_x = 5.0f;
                    entities[0].sprite.index =runAnimation[currentIndex%4];
                }
                else
                    entities[0].sprite.index=20;
                if(entities[0].isDead) switchState(STATE_GAME_LOSE);
                break;
            case(STATE_GAME_LVL2):
                
                if (changeLevel){
                    entities.clear();
                    numKeys=game3.nK;
                    numDiam=game3.nG;
                    changeLevel=false;
                    entities=level3ent;
                    for(int i=0; i<entities.size();i++)
                    {
                        if(entities[i].entityType==ENTITY_KEY)
                            entities[i].sprite.index =44;
                        if(entities[i].entityType==ENTITY_DIAMOND)
                            entities[i].sprite.index +=1;
                    }
                    
                    
                    entities[0].damage=5;
                    switchState(STATE_GAME_LVL3);
                }
                if(keys[SDL_SCANCODE_LEFT])
                {
                    
                    entities[0].scale = -1.0;
                    entities[0].acceleration_x = -5.0;
                    entities[0].sprite.index = runAnimation[currentIndex%4]+30;
                }
                else if(keys[SDL_SCANCODE_RIGHT])
                {
                    
                    entities[0].scale = 1.0;
                    entities[0].acceleration_x = 5.0f;
                    entities[0].sprite.index =runAnimation[currentIndex%4]+30;
                }
                else
                    entities[0].sprite.index=50;
                if(entities[0].isDead) switchState(STATE_GAME_LOSE);
                break;
            case(STATE_GAME_LVL3):
                if(win) switchState(STATE_GAME_WIN);
                if(keys[SDL_SCANCODE_LEFT])
                {
                    
                    entities[0].scale = -1.0;
                    entities[0].acceleration_x = -5.0;
                    entities[0].sprite.index = runAnimation[currentIndex%4]+60;
                }
                else if(keys[SDL_SCANCODE_RIGHT])
                {
                    
                    entities[0].scale = 1.0;
                    entities[0].acceleration_x = 5.0f;
                    entities[0].sprite.index =runAnimation[currentIndex%4]+60;
                }
                else
                    entities[0].sprite.index=80;
                if(entities[0].isDead) switchState(STATE_GAME_LOSE);
                break;
            case(STATE_GAME_WIN) :
                if (keys[SDL_SCANCODE_ESCAPE]||keys[SDL_SCANCODE_Q]){
                    done=true;
                }
                else if (keys[SDL_SCANCODE_R]){
                    level1ent = levelEntities(game,map2);
                    level2ent = levelEntities(game2, map2);
                    level3ent = levelEntities(game3, map2);

                    switchState(STATE_GAME_TITLE);
                }
                break;
            case(STATE_GAME_LOSE) :
                if (keys[SDL_SCANCODE_ESCAPE]||keys[SDL_SCANCODE_Q]){
                    done=true;
                }
                else if(keys[SDL_SCANCODE_R]){
                
                    level1ent = levelEntities(game,map2);
                    level2ent = levelEntities(game2, map2);
                    level3ent = levelEntities(game3, map2);
                    switchState(STATE_GAME_TITLE);
                }
                break;
                
        }
        
        
        switch (state) {
            case STATE_GAME_TITLE:
                
                
                modelMatrix.identity();
                modelMatrix.Translate(-1.5, 1.5, 0.0);
                program.setModelMatrix(modelMatrix);
                DrawText(&program, font, "THE HERO", 0.4f, 0.0f);
                
                modelMatrix.identity();
                modelMatrix.Translate(2.75, 0.175, 0.0);
                program.setModelMatrix(modelMatrix);
                DrawText(&program, font, "Hit", 0.175f, 0.0f);
                
                modelMatrix.identity();
                modelMatrix.Translate(2.6, 0.0, 0.0);
                program.setModelMatrix(modelMatrix);
                DrawText(&program, font, "Enter", 0.175f, 0.0f);
                
                modelMatrix.identity();
                modelMatrix.Translate(2.85, -0.175, 0.0);
                program.setModelMatrix(modelMatrix);
                DrawText(&program, font, "To", 0.175f, 0.0f);
                
                modelMatrix.identity();
                modelMatrix.Translate(2.4, -0.35, 0.0);
                program.setModelMatrix(modelMatrix);
                DrawText(&program, font, "Play...", 0.175f, 0.0f);
                
                modelMatrix.identity();
                modelMatrix.Scale(4.5, 3.2, 1.0);
                modelMatrix.Translate(0.0, -0.2, 0.0);
                program.setModelMatrix(modelMatrix);
                draw(&program, title);
                
                
                break;
            case STATE_GAME_INS:
                modelMatrix.identity();
                modelMatrix.Translate(-1.0, 1.9, 0.0);
                program.setModelMatrix(modelMatrix);
                DrawText(&program, font, "How to Play", 0.2f, 0.0f);
                
                modelMatrix.identity();
                modelMatrix.Translate(-1.8, 1.4, 0.0);
                program.setModelMatrix(modelMatrix);
                DrawText(&program, font, "Use the arrow keys", 0.2f, 0.0f);
                
                modelMatrix.identity();
                modelMatrix.Translate(-1.8, 1.2, 0.0);
                program.setModelMatrix(modelMatrix);
                DrawText(&program, font, "to move around", 0.2f, 0.0f);
                
                modelMatrix.identity();
                modelMatrix.Translate(-1.8, 0.8, 0.0);
                program.setModelMatrix(modelMatrix);
                DrawText(&program, font, "Hit space to shoot", 0.2f, 0.0f);
                
                modelMatrix.identity();
                modelMatrix.Translate(-1.8, 0.4, 0.0);
                program.setModelMatrix(modelMatrix);
                DrawText(&program, font, "Collect all the keys", 0.2f, 0.0f);
                
                modelMatrix.identity();
                modelMatrix.Translate(-1.8, 0.0, 0.0);
                program.setModelMatrix(modelMatrix);
                DrawText(&program, font, "Some enemies are harder", 0.2f, 0.0f);
                
                modelMatrix.identity();
                modelMatrix.Translate(-1.8, -0.2, 0.0);
                program.setModelMatrix(modelMatrix);
                DrawText(&program, font, "to kill than others", 0.2f, 0.0f);
                
                modelMatrix.identity();
                modelMatrix.Translate(-1.8, -0.6, 0.0);
                program.setModelMatrix(modelMatrix);
                DrawText(&program, font, "Increase your shooting", 0.2f, 0.0f);
                
                modelMatrix.identity();
                modelMatrix.Translate(-1.8, -0.8, 0.0);
                program.setModelMatrix(modelMatrix);
                DrawText(&program, font, "damage by collecting gems", 0.2f, 0.0f);
                
                modelMatrix.identity();
                modelMatrix.Translate(-1.8, -1.4, 0.0);
                program.setModelMatrix(modelMatrix);
                DrawText(&program, font, "Hit SPACE to Start Playing...", 0.15f, 0.0f);
                
                modelMatrix.identity();
                modelMatrix.Scale(0.8, 0.8, 1.0);
                modelMatrix.Translate(-3.5, 1.6, 0.0);
                program.setModelMatrix(modelMatrix);
                draw(&program, arrowkeys);
                
                
                modelMatrix.identity();
                modelMatrix.Scale(1.35, 1.35, 1.0);
                modelMatrix.Translate(-2.0, 0.0, 0.0);
                program.setModelMatrix(modelMatrix);
                draw(&program, obj);
                
                
                
                
                modelMatrix.identity();
                
                modelMatrix.Translate(-2.75, -1.2, 0.0);
                program.setModelMatrix(modelMatrix);
                draw(&program, gems);
                break;
            
            case STATE_GAME_LVL1:
                if(fixedElapsed > FIXED_TIMESTEP * MAX_TIMESTEPS) {
                    fixedElapsed = FIXED_TIMESTEP * MAX_TIMESTEPS;
                }
                
        for(int i=0; i<entities.size();i++)
        {
            entities[i].collidesReset();
            if(entities[i].entityType==ENTITY_ENEMY)
            {
                if(entities[i].enemyType == ENEMY_BEE)
                    entities[i].sprite.index = beeAnim[currentIndex%2];
                else if(entities[i].enemyType == ENEMY_FISH)
                    entities[i].sprite.index = fishAnim[currentIndex%2];
                else if(entities[i].enemyType == ENEMY_SLUG)
                    entities[i].sprite.index = slugAnim[currentIndex%6];
                else if(entities[i].enemyType == ENEMY_SLUG2)
                    entities[i].sprite.index = slug2Anim[currentIndex%4];
                else if(entities[i].enemyType == ENEMY_SPIDER)
                    entities[i].sprite.index = spiderAnim[currentIndex%3];
                else if(entities[i].enemyType==ENEMY_GHOST)
                    entities[i].sprite.index = ghostAnim[currentIndex%6];
                else if(entities[i].enemyType==ENEMY_SNAIL)
                    entities[i].sprite.index = snailAnim[currentIndex%4];
                else if(entities[i].enemyType==ENEMY_SNAKE)
                    entities[i].sprite.index = snakeAnim[currentIndex%4];
                else if(entities[i].enemyType==ENEMY_SAW)
                    entities[i].sprite.index = sawAnim[currentIndex%4];
                    
            }
            
        }
               

        for(int i=0; i < entities.size();i++)
        {
            
            
            while (fixedElapsed >= FIXED_TIMESTEP ) {
                fixedElapsed -= FIXED_TIMESTEP;
                entities[i].Update(FIXED_TIMESTEP);
                collisionX(entities[i],game.levelData,game.mapWidth,game.mapHeight);
                collisionY(entities[i],game.levelData,game.mapWidth,game.mapHeight);
                
                
            }
            entities[i].Update(fixedElapsed);
            collisionX(entities[i],game.levelData,game.mapWidth,game.mapHeight);
            collisionY(entities[i],game.levelData,game.mapWidth,game.mapHeight);
            
            
        }
                if(keys[SDL_SCANCODE_UP])
                {
                    
                    if(entities[0].collidedBottom)
                    {    Mix_PlayChannel(-1, jump, 0);
                        entities[0].velocity_y = 4.0f;
                    }
                }
                

        for(int i =0;i<MAX_BULLETS;i++)
        {
            if(bullets[i].isActive){
                bullets[i].collidesReset();
                bullets[i].Update(fixedElapsed);
                collisionX(bullets[i], game.levelData,game.mapWidth,game.mapHeight);
                collisionY(bullets[i], game.levelData,game.mapWidth,game.mapHeight);
            }
            
        }
        
        setLives(map2, lives, entities[0]);
        setKeyDiam(map2,numKeys,numDiam,keyDiam, entities[0]);
        
        
        gun.x = entities[0].x+(entities[0].scale)*entities[0].width/2;
        gun.y=entities[0].y;
        gun.scale=entities[0].scale;
                for(int i=0;i<MAX_BULLETS;i++)
        {
            if(bullets[i].collidedTop||bullets[i].collidedBottom||bullets[i].collidedRight||bullets[i].collidedLeft) bullets[i].isActive=false;
            if(bullets[i].timeAlive>0.4)
            {
                bullets[i].timeAlive=0;
                bullets[i].isActive=false;
            }
            
                
        }
        for(int i=0; i<MAX_BULLETS;i++)
        {
            if(bullets[i].isActive){
                for(int j=0;j<entities.size();j++)
                {
                    if(entities[j].entityType==ENTITY_ENEMY)
                    {
                       if(bullets[i].collidesWith(&entities[j])&&!entities[j].isDead)
                       {
                           Mix_PlayChannel(-1, enemydie, 0);
                           bullets[i].isActive=false;
                           entities[j].health-=entities[0].damage;
                           if(entities[j].health<=0){
                               Mix_PlayChannel(-1, enemydie, 0);
                               entities[j].isDead=true;
                           }
                       }
                    }
                }
            }
        }
        
        
                
        for(int i =1; i < entities.size();i++)
        {
            if(entities[i].velocity_x>=0.0) entities[i].scale=-1.0;
            else if(entities[i].velocity_x<0.0)entities[i].scale=1.0;
            if((entities[i].collidedRight||entities[i].collidedLeft)){
                entities[i].velocity_x*=-1;
                entities[i].acceleration_x*=-1;
            }
            if(entities[0].collidesWith(&entities[i])&& !entities[i].isDead)
            {
                if(entities[i].entityType==ENTITY_ENEMY&& !entities[i].isActive){
                    
                    Mix_PlayChannel(-1, gethit, 0);
                    entities[0].health-=5;
                    entities[i].isActive =true;
                }
                else if(entities[i].entityType==ENTITY_KEY)
                {
                    Mix_PlayChannel(-1, keyS, 0);
                    numKeys-=1;
                    entities[i].isDead=true;
                }
                else if(entities[i].entityType==ENTITY_DIAMOND)
                {
                    Mix_PlayChannel(-1, diamondS, 0);
                    entities[i].isDead=true;
                    entities[0].damage+=5;
                    numDiam-=1;
                }
            }
            else if(!entities[0].collidesWith(&entities[i])&&!entities[i].isDead)
                entities[i].isActive=false;
        
        }
        
        if(entities[0].health<=0)
            entities[0].isDead=true;
        if(entities[0].x+TILE_SIZE/2 >= TILE_SIZE*(game.mapWidth-2) && numKeys==0)
            changeLevel=true;
        
        viewMatrix.identity();
        viewMatrix.Translate(-entities[0].x-2.75, -entities[0].y, 0);
        program.setViewMatrix(viewMatrix);
        program.setModelMatrix(modelMatrix);
        
            
        game.drawLevel(&program, map2);
        
       
        
        for(Entity k : entities){
            if(!k.isDead){
                k.Render(&program);
            }
        }
        for(Entity b:bullets)
        {
            if(b.isActive)
                b.Render(&program);;
        }
        
        for(Entity live : lives)
            live.Render(&program);
        for(Entity key: keyDiam)
            key.Render(&program);
        gun.Render(&program);
                
                break;
            case STATE_GAME_LVL2:
                if(fixedElapsed > FIXED_TIMESTEP * MAX_TIMESTEPS) {
                    fixedElapsed = FIXED_TIMESTEP * MAX_TIMESTEPS;
                }
                for(int i=0; i<entities.size();i++)
                {
                    entities[i].collidesReset();
                    if(entities[i].entityType==ENTITY_ENEMY)
                    {
                        if(entities[i].enemyType == ENEMY_BEE)
                            entities[i].sprite.index = beeAnim[currentIndex%2];
                        else if(entities[i].enemyType == ENEMY_FISH)
                            entities[i].sprite.index = fishAnim[currentIndex%2];
                        else if(entities[i].enemyType == ENEMY_SLUG)
                            entities[i].sprite.index = slugAnim[currentIndex%6];
                        else if(entities[i].enemyType == ENEMY_SLUG2)
                            entities[i].sprite.index = slug2Anim[currentIndex%4];
                        else if(entities[i].enemyType == ENEMY_SPIDER)
                            entities[i].sprite.index = spiderAnim[currentIndex%3];
                        else if(entities[i].enemyType==ENEMY_GHOST)
                            entities[i].sprite.index = ghostAnim[currentIndex%6];
                        else if(entities[i].enemyType==ENEMY_SNAIL)
                            entities[i].sprite.index = snailAnim[currentIndex%4];
                        else if(entities[i].enemyType==ENEMY_SNAKE)
                            entities[i].sprite.index = snakeAnim[currentIndex%4];
                        else if(entities[i].enemyType==ENEMY_SAW)
                            entities[i].sprite.index = sawAnim[currentIndex%4];
                    }
                }
                for(int i=0; i < entities.size();i++)
                {
                    
                    
                    while (fixedElapsed >= FIXED_TIMESTEP ) {
                        fixedElapsed -= FIXED_TIMESTEP;
                        entities[i].Update(FIXED_TIMESTEP);
                        collisionX(entities[i],game2.levelData,game2.mapWidth,game2.mapHeight);
                        collisionY(entities[i],game2.levelData,game2.mapWidth,game2.mapHeight);
                        
                        
                    }
                    entities[i].Update(fixedElapsed);
                    collisionX(entities[i],game2.levelData,game2.mapWidth,game2.mapHeight);
                    collisionY(entities[i],game2.levelData,game2.mapWidth,game2.mapHeight);
                    
                    
                }
                if(keys[SDL_SCANCODE_UP])
                {
                    
                    if(entities[0].collidedBottom)
                    {    Mix_PlayChannel(-1, jump, 0);
                        entities[0].velocity_y = 4.0f;
                    }
                }
                

                    
                for(int i =0;i<MAX_BULLETS;i++)
                {
                    if(bullets[i].isActive){
                        bullets[i].collidesReset();
                        bullets[i].Update(fixedElapsed);
                        collisionX(bullets[i], game2.levelData,game2.mapWidth,game2.mapHeight);
                        collisionY(bullets[i], game2.levelData,game2.mapWidth,game2.mapHeight);
                    }
                    
                }
                
                
                setLives(map2, lives, entities[0]);
                setKeyDiam(map2, numKeys,numDiam, keyDiam, entities[0]);
                keyDiam[0].sprite.index +=2;
                keyDiam[2].sprite.index +=2;
                gun.x = entities[0].x+(entities[0].scale)*entities[0].width/2;
                gun.y=entities[0].y;
                gun.scale=entities[0].scale;
                                for(int i=0;i<MAX_BULLETS;i++)
                {
                    if(bullets[i].collidedTop||bullets[i].collidedBottom||bullets[i].collidedRight||bullets[i].collidedLeft) bullets[i].isActive=false;
                    if(bullets[i].timeAlive>0.4)
                    {
                        bullets[i].timeAlive=0;
                        bullets[i].isActive=false;
                    }
                    
                    
                }
                
                for(int i=0; i<MAX_BULLETS;i++)
                {
                    if(bullets[i].isActive){
                        for(int j=0;j<entities.size();j++)
                        {
                            if(entities[j].entityType==ENTITY_ENEMY)
                            {
                                if(bullets[i].collidesWith(&entities[j])&&!entities[j].isDead)
                                {
                                    Mix_PlayChannel(-1, enemydie, 0);
                                    bullets[i].isActive=false;
                                    entities[j].health-=entities[0].damage;
                                    if(entities[j].health<=0){
                                        Mix_PlayChannel(-1, enemydie, 0);
                                        entities[j].isDead=true;
                                    }
                                }
                            }
                        }
                    }
                }
                
                for(int i =1; i < entities.size();i++)
                {
                    if(entities[i].velocity_x>=0.0) entities[i].scale=-1.0;
                    else if(entities[i].velocity_x<0.0)entities[i].scale=1.0;
                    if((entities[i].collidedRight||entities[i].collidedLeft)){
                        entities[i].velocity_x*=-1;
                        entities[i].acceleration_x*=-1;
                    }
                    if(entities[0].collidesWith(&entities[i])&& !entities[i].isDead)
                    {
                        if(entities[i].entityType==ENTITY_ENEMY&& !entities[i].isActive){
                            
                            Mix_PlayChannel(-1, gethit, 0);
                            entities[0].health-=5;
                            entities[i].isActive =true;
                        }
                        else if(entities[i].entityType==ENTITY_KEY)
                        {
                            Mix_PlayChannel(-1, keyS, 0);
                            numKeys-=1;
                            entities[i].isDead=true;
                        }
                        else if(entities[i].entityType==ENTITY_DIAMOND)
                        {
                            Mix_PlayChannel(-1, diamondS, 0);
                            entities[i].isDead=true;
                            entities[0].damage+=5;
                            numDiam-=1;
                        }
                    }
                    else if(!entities[0].collidesWith(&entities[i])&&!entities[i].isDead)
                        entities[i].isActive=false;
                    
                }
                
                if(entities[0].health<=0)
                    entities[0].isDead=true;
                if(entities[0].x >= TILE_SIZE*(game2.mapWidth-2)&& numKeys==0)
                    changeLevel=true;
                
                viewMatrix.identity();
                viewMatrix.Translate(-entities[0].x-2.75, -entities[0].y, 0);
                program.setViewMatrix(viewMatrix);
                program.setModelMatrix(modelMatrix);
                
                
                game2.drawLevel(&program, map2);
                
                
                
                for(Entity k : entities){
                    if(!k.isDead){
                        k.Render(&program);
                    }
                }
                for(Entity b:bullets)
                {
                    if(b.isActive)
                        b.Render(&program);;
                }
                
                for(Entity live : lives)
                    live.Render(&program);
                for(Entity key: keyDiam)
                    key.Render(&program);
                gun.Render(&program);
                
                break;
            case STATE_GAME_LVL3:
                if(fixedElapsed > FIXED_TIMESTEP * MAX_TIMESTEPS) {
                    fixedElapsed = FIXED_TIMESTEP * MAX_TIMESTEPS;
                }
                glClearColor((135.0f / 255.0f), (149.0f / 255.0f), (150.0f / 255.0f), 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);
                for(int i=0; i<entities.size();i++)
                {
                    entities[i].collidesReset();
                    if(entities[i].entityType==ENTITY_ENEMY)
                    {
                        if(entities[i].enemyType == ENEMY_BEE)
                            entities[i].sprite.index = beeAnim[currentIndex%2];
                        else if(entities[i].enemyType == ENEMY_FISH)
                            entities[i].sprite.index = fishAnim[currentIndex%2];
                        else if(entities[i].enemyType == ENEMY_SLUG)
                            entities[i].sprite.index = slugAnim[currentIndex%6];
                        else if(entities[i].enemyType == ENEMY_SLUG2)
                            entities[i].sprite.index = slug2Anim[currentIndex%4];
                        else if(entities[i].enemyType == ENEMY_SPIDER)
                            entities[i].sprite.index = spiderAnim[currentIndex%3];
                        else if(entities[i].enemyType==ENEMY_GHOST)
                            entities[i].sprite.index = ghostAnim[currentIndex%6];
                        else if(entities[i].enemyType==ENEMY_SNAIL)
                            entities[i].sprite.index = snailAnim[currentIndex%4];
                        else if(entities[i].enemyType==ENEMY_SNAKE)
                            entities[i].sprite.index = snakeAnim[currentIndex%4];
                        else if(entities[i].enemyType==ENEMY_SAW)
                            entities[i].sprite.index = sawAnim[currentIndex%4];
                    }
                }
                for(int i=0; i < entities.size();i++)
                {
                    
                    
                    while (fixedElapsed >= FIXED_TIMESTEP ) {
                        fixedElapsed -= FIXED_TIMESTEP;
                        entities[i].Update(FIXED_TIMESTEP);
                        collisionX(entities[i],game3.levelData,game3.mapWidth,game3.mapHeight);
                        collisionY(entities[i],game3.levelData,game3.mapWidth,game3.mapHeight);
                        
                        
                    }
                    entities[i].Update(fixedElapsed);
                    collisionX(entities[i],game3.levelData,game3.mapWidth,game3.mapHeight);
                    collisionY(entities[i],game3.levelData,game3.mapWidth,game3.mapHeight);
                    
                    
                }
                if(keys[SDL_SCANCODE_UP])
                {
                    
                    if(entities[0].collidedBottom)
                    {    Mix_PlayChannel(-1, jump, 0);
                        entities[0].velocity_y = 4.0f;
                    }
                }
                

                for(int i =0;i<MAX_BULLETS;i++)
                {
                    if(bullets[i].isActive){
                        bullets[i].collidesReset();
                        bullets[i].Update(fixedElapsed);
                        collisionX(bullets[i], game3.levelData,game3.mapWidth,game3.mapHeight);
                        collisionY(bullets[i], game3.levelData,game3.mapWidth,game3.mapHeight);
                    }
                    
                }
                
                
                setLives(map2, lives, entities[0]);
                setKeyDiam(map2, numKeys,numDiam, keyDiam, entities[0]);
                gun.x = entities[0].x+(entities[0].scale)*entities[0].width/2;
                keyDiam[0].sprite.index +=1;
                keyDiam[2].sprite.index +=1;
                gun.y=entities[0].y;
                gun.scale=entities[0].scale;
                for(int i=0;i<MAX_BULLETS;i++)
                {
                    if(bullets[i].collidedTop||bullets[i].collidedBottom||bullets[i].collidedRight||bullets[i].collidedLeft) bullets[i].isActive=false;
                    if(bullets[i].timeAlive>0.4)
                    {
                        bullets[i].timeAlive=0;
                        bullets[i].isActive=false;
                    }
                    
                    
                }
                
                for(int i=0; i<MAX_BULLETS;i++)
                {
                    if(bullets[i].isActive){
                        for(int j=0;j<entities.size();j++)
                        {
                            if(entities[j].entityType==ENTITY_ENEMY)
                            {
                                if(bullets[i].collidesWith(&entities[j])&&!entities[j].isDead)
                                {
                                    Mix_PlayChannel(-1, enemydie, 0);
                                    bullets[i].isActive=false;
                                    entities[j].health-=entities[0].damage;
                                    if(entities[j].health==0){
                                        Mix_PlayChannel(-1, enemydie, 0);
                                        entities[j].isDead=true;
                                    }
                                }
                            }
                        }
                    }
                }
                
                for(int i =1; i < entities.size();i++)
                {
                    if(entities[i].velocity_x>=0.0) entities[i].scale=-1.0;
                    else if(entities[i].velocity_x<0.0)entities[i].scale=1.0;
                    if((entities[i].collidedRight||entities[i].collidedLeft)){
                        entities[i].velocity_x*=-1;
                        entities[i].acceleration_x*=-1;
                    }
                    if(entities[0].collidesWith(&entities[i])&& !entities[i].isDead)
                    {
                        if(entities[i].entityType==ENTITY_ENEMY&& !entities[i].isActive){
                            
                            Mix_PlayChannel(-1, gethit, 0);
                            entities[0].health-=5;
                            entities[i].isActive =true;
                        }
                        else if(entities[i].entityType==ENTITY_KEY)
                        {
                            Mix_PlayChannel(-1, keyS, 0);
                            numKeys-=1;
                            entities[i].isDead=true;
                        }
                        else if(entities[i].entityType==ENTITY_DIAMOND)
                        {
                            Mix_PlayChannel(-1, diamondS, 0);
                            entities[i].isDead=true;
                            numDiam-=1;
                            entities[0].damage+=5;
                        }
                    }
                    else if(!entities[0].collidesWith(&entities[i])&&!entities[i].isDead)
                        entities[i].isActive=false;
                    
                }
                
                if(entities[0].health<=0)
                    entities[0].isDead=true;
                if(entities[0].x >= TILE_SIZE*(game3.mapWidth-2)&&numKeys==0)
                    win=true;
                
                viewMatrix.identity();
                
                viewMatrix.Translate(-entities[0].x-2.75, -entities[0].y, 0);
                program.setViewMatrix(viewMatrix);
                program.setModelMatrix(modelMatrix);
                
                
                game3.drawLevel(&program, map2);
                
                
                
                for(Entity k : entities){
                    if(!k.isDead){
                        k.Render(&program);
                    }
                }
                for(Entity b:bullets)
                {
                    if(b.isActive)
                        b.Render(&program);;
                }
                
                for(Entity live : lives)
                    live.Render(&program);
                for(Entity key: keyDiam)
                    key.Render(&program);
                gun.Render(&program);
                
                break;

            case STATE_GAME_WIN:
                glClearColor(0,0,0,0);
                glClear(GL_COLOR_BUFFER_BIT);
                
                modelMatrix.identity();
                modelMatrix.Translate(-3.335, 1.7, 0.0);
                program.setModelMatrix(modelMatrix);
                DrawText(&program, font, "CONGRATS!! YOU WIN!!", 0.35f, 0.0f);
                
                modelMatrix.identity();
                modelMatrix.Translate(-2.2, -1.7, 0.0);
                program.setModelMatrix(modelMatrix);
                DrawText(&program, font, "HIT R TO GO TO MAIN MENU", 0.2f, 0.0f);
                
                s1.index = anim[currentIndex%2];
                wT.index = wtAnim[currentIndex%2];
                wB.index = wbAnim[currentIndex%2];
                modelMatrix.identity();
                program.setModelMatrix(modelMatrix);
                s1.Draw(&program);
                modelMatrix.identity();
                modelMatrix.Translate(-2.0, 0.5, 0);
                program.setModelMatrix(modelMatrix);
                wT.Draw(&program);
                modelMatrix.identity();
                modelMatrix.Translate(-2.0, -0.5, 0);
                program.setModelMatrix(modelMatrix);
                wB.Draw(&program);
                modelMatrix.identity();
                modelMatrix.Translate(2.0, 0.5, 0);
                program.setModelMatrix(modelMatrix);
                wT.Draw(&program);
                modelMatrix.identity();
                modelMatrix.Translate(2.0, -0.5, 0);
                program.setModelMatrix(modelMatrix);
                wB.Draw(&program);

                
                break;
                
            case STATE_GAME_LOSE:
                glClearColor((201.0f / 255.0f), (242.0f / 255.0f), (246.0f / 255.0f), 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);
                
                modelMatrix.identity();
                modelMatrix.Translate(-1.6, 1.7, 0.0);
                program.setModelMatrix(modelMatrix);
                DrawText(&program, font, "YOU LOSE!!", 0.4f, 0.0f);
                
                modelMatrix.identity();
                modelMatrix.Translate(-2.2, -1.7, 0.0);
                program.setModelMatrix(modelMatrix);
                DrawText(&program, font, "HIT R TO GO TO MAIN MENU", 0.2f, 0.0f);
                
                modelMatrix.identity();
                modelMatrix.Scale(1.8, 1.6, 1.0);
                modelMatrix.Translate(0.0, 0.0, 0.0);
                program.setModelMatrix(modelMatrix);
                
                draw(&program, losep);
                
                modelMatrix.identity();
                modelMatrix.Scale(0.5, 0.5, 1.0);
                lose1 += 5.0 * elapsed;
                modelMatrix.Translate(0.0, 2.25, 0.0);
                modelMatrix.Rotate(lose1);
                program.setModelMatrix(modelMatrix);
                
                draw(&program, lose);
                
                modelMatrix.identity();
                modelMatrix.Scale(0.5, 0.5, 1.0);
                lose2 -= 5.0 * elapsed;
                modelMatrix.Translate(0.0, -2.25, 0.0);
                modelMatrix.Rotate(lose2);
                program.setModelMatrix(modelMatrix);
                
                draw(&program, lose);
                
                break;
        }
        viewMatrix.identity();
        modelMatrix.identity();
        program.setModelMatrix(modelMatrix);
        program.setProjectionMatrix(projectionMatrix);
        program.setViewMatrix(viewMatrix);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        SDL_GL_SwapWindow(displayWindow);
        
        
    }
    
    SDL_Quit();
    return 0;
}
