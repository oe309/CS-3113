#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "ShaderProgram.h";
#include "Matrix.h";
#include "SheetSprite.h";
#include <vector>
#include "Entity.h";
#include <SDL_mixer.h>


#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

SDL_Window* displayWindow;
Matrix projectionMatrix;
Matrix modelMatrix;
Matrix viewMatrix;

GLuint LoadTexture(const char *image_path, GLenum format = GL_RGBA){
	SDL_Surface *surface = IMG_Load(image_path);

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, format, surface->w, surface->h, 0, format, GL_UNSIGNED_BYTE, surface->pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	SDL_FreeSurface(surface);

	return textureID;
}

void DrawText(ShaderProgram *program, int fontTexture, std::string text, float size, float spacing) {
	float texture_size = 1.0 / 16.0f;
	std::vector<float> vertexData;
	std::vector<float> texCoordData;
	for (int i = 0; i < text.size(); i++) {
		float texture_x = (float)(((int)text[i]) % 16) / 16.0f;
		float texture_y = (float)(((int)text[i]) / 16) / 16.0f;
		vertexData.insert(vertexData.end(), {
			((size + spacing) * i) + (-0.5f * size), 0.5f * size,
			((size + spacing) * i) + (-0.5f * size), -0.5f * size,
			((size + spacing) * i) + (0.5f * size), 0.5f * size,
			((size + spacing) * i) + (0.5f * size), -0.5f * size,
			((size + spacing) * i) + (0.5f * size), 0.5f * size,
			((size + spacing) * i) + (-0.5f * size), -0.5f * size,
		});
		texCoordData.insert(texCoordData.end(), {
			texture_x, texture_y,
			texture_x, texture_y + texture_size,
			texture_x + texture_size, texture_y,
			texture_x + texture_size, texture_y + texture_size,
			texture_x + texture_size, texture_y,
			texture_x, texture_y + texture_size,
		});
	}
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

void setup()
{
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
	glewInit();
#endif

	glViewport(0, 0, 1280, 720);
}

void reset(std::vector<Entity> &enemies, std::vector<Entity>&rocks)
{

	Entity en1(enemies[0].sprite), en2(enemies[12].sprite), en3(enemies[24].sprite);
	float x = -3.3;
	float y = 1.5;
	for (int i = 0; i < enemies.size(); i++)
	{
		if (i < 12) {
			if (i % 6 == 0) { x = -3.3; y -= 0.3; }
			en1.position.x = x;
			en1.position.y = y;
			enemies[i] = en1;
			x += 0.6;
		}
		else if (i > 11 && i < 24) {
			if (i % 6 == 0) { x = -3.3; y -= 0.3; }
			en2.position.x = x;
			en2.position.y = y;
			enemies[i] = en2;
			x += 0.6;
		}

		else if (i > 23 && i < 36) {
			if (i % 6 == 0) { x = -3.3; y -= 0.3; }
			en3.position.x = x;
			en3.position.y = y;
			enemies[i] = en3;
			x += 0.6;
		}
		enemies[i].velocity.x = 0.5;
	}

	for (int i = 0; i < rocks.size(); i++)
	{
		rocks[i].hp = 50;
		rocks[i].Dead = false;
	}

}


int main(int argc, char *argv[])
{

	setup();

	ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");

	projectionMatrix.setOrthoProjection(-3.55, 3.55, -2.0f, 2.0f, -1.0f, 1.0f);
	glUseProgram(program.programID);

	float lastFrameTicks = 0.0f;

	GLuint font = LoadTexture("font1.png");
	GLuint sheet = LoadTexture("invader-.png");
	GLuint other = LoadTexture("other.png");

	SheetSprite ship(&program, sheet, 133.0f / 540.0f, 626.0f / 685.0f, 73.0f / 540.0f, 52.0f / 685.0f, 0.1);
	SheetSprite enemy1(&program, sheet, 295.0f / 540.0f, 2.0f / 685.0f, 80.0f / 540.0f, 80.0f / 685.0f, 0.1);
	SheetSprite enemy2(&program, sheet, 2.0f / 540.0f, 2.0f / 685.0f, 110.0f / 540.0f, 80.0f / 685.0f, 0.1);
	SheetSprite enemy3(&program, sheet, 2.0f / 540.0f, 122.0f / 685.0f, 120.0f / 540.0f, 80.0f / 685.0f, 0.1);
	SheetSprite bullet(&program, other, 0 / 1026.0f, 323 / 1026.0f, 16 / 1026.0f, 38 / 1026.0f, 0.1);
	SheetSprite bullet1(&program, sheet, 470.0f / 540.0f, 382.0f / 685.0f, 28.0f / 540.0f, 51.0f / 685.0f, 0.1);
	SheetSprite menu(&program, sheet, 2.0f / 540.0f, 2.0f / 685.0f, 502.0f / 540.0f, 200.0f / 685.0f, 1);
	SheetSprite rock(&program, other, 492.0f / 1026.0f, 0.0f / 1026.0f, 101.0f / 1026.0f, 71.0f / 1026.0f, 0.4);

	Entity en1(enemy1);
	Entity en2(enemy2);
	Entity en3(enemy3);
	Entity player(ship);
	Entity bulls(bullet1);
	Entity shield(rock);
	std::vector<Entity> enemies;
	std::vector<Entity> playerBullets;
	std::vector<Entity> rocks;
	enum GameState{ GAME_MENU, GAME_START, GAME_OVER };

	player.position.y = -1.85;
	bulls.position.x = 5;

	int state = 0;
	int score = 0;
	float shootTimmer = 0;
	float x = -3.3;
	float y = 1.5;
	int z = -3.5;
	bool coll = false;

	int enemiesDead = 0;
	std::string result;

	for (int i = 0; i < 7; i++)
	{

		shield.position.x = z;
		shield.position.y = -1.25;
		if (i % 2 == 0) rocks.push_back(shield);
		z += 1;
	}


	for (int i = 0; i < 36; i++)
	{
		if (i < 12) {
			if (i % 6 == 0) { x = -3.3; y -= 0.3; }
			en1.position.x = x;
			en1.position.y = y;
			enemies.push_back(en1);
			x += 0.6;
		}
		else if (i > 11 && i < 24) {
			if (i % 6 == 0) { x = -3.3; y -= 0.3; }
			en2.position.x = x;
			en2.position.y = y;
			enemies.push_back(en2);
			x += 0.6;
		}

		else if (i > 23 && i < 36) {
			if (i % 6 == 0) { x = -3.3; y -= 0.3; }
			en3.position.x = x;
			en3.position.y = y;
			enemies.push_back(en3);
			x += 0.6;
		}
		enemies[i].velocity.x = 0.5;
		enemies[i].Dead = false;

	}

	SDL_Event event;
	bool done = false;
	const Uint8 *keys = SDL_GetKeyboardState(NULL);

	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
	Mix_Chunk *pFire;
	pFire = Mix_LoadWAV("pFire.wav");
	Mix_Chunk *eFire;
	eFire = Mix_LoadWAV("eFire.wav");
	Mix_Chunk *pHit;
	pHit = Mix_LoadWAV("pHit.wav");
	Mix_Chunk *eHit;
	eHit = Mix_LoadWAV("eHit.wav");

	while (!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
		}

		glClear(GL_COLOR_BUFFER_BIT);

		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;

		if (state == GAME_MENU)
		{
			if (keys[SDL_SCANCODE_RETURN])
			{
				state = GAME_START;
			}
		}
		else if (state == GAME_START){
			if (keys[SDL_SCANCODE_LEFT])
			{
				if (player.position.x - player.size.x > -3.55)
				{
					player.velocity.x = -1.5;
					player.Update(elapsed);
				}

			}
			if (keys[SDL_SCANCODE_RIGHT])
			{
				if (player.position.x + player.size.x < 3.55)
				{
					player.velocity.x = 1.5;
					player.Update(elapsed);
				}
			}

			if (keys[SDL_SCANCODE_SPACE])
			{
				if (playerBullets.size() == 0)
				{
					Mix_PlayChannel(-1, pFire, 0);
					Entity bulls(bullet);
					bulls.position.x = player.position.x;
					bulls.position.y = -1.85 + player.size.y / 2 + bulls.size.y / 2;
					bulls.velocity.y = 4.0;
					playerBullets.push_back(bulls);
					
				}
			}
		}

		else if (state == GAME_OVER)
		{
			if (keys[SDL_SCANCODE_SPACE])
			{
				state = GAME_MENU;
			}
		}

		switch (state){
		case GAME_MENU:
			modelMatrix.identity();
			modelMatrix.Translate(-2.3, 1.5, 0);
			program.setModelMatrix(modelMatrix);
			program.setProjectionMatrix(projectionMatrix);
			program.setViewMatrix(viewMatrix);
			DrawText(&program, font, "Space Invaders", 0.4, 0);

			modelMatrix.identity();
			modelMatrix.Translate(0, 0.25, 0);
			program.setModelMatrix(modelMatrix);
			menu.Draw();

			modelMatrix.identity();
			modelMatrix.Translate(-2.3, -1, 0);
			program.setModelMatrix(modelMatrix);
			DrawText(&program, font, "Press enter to start...", 0.2, 0);
			break;
		case GAME_START:

			program.setProjectionMatrix(projectionMatrix);
			program.setViewMatrix(viewMatrix);
			player.Draw();

			bulls.Update(elapsed);
			bulls.Draw();

			modelMatrix.identity();
			modelMatrix.Translate(-3.5, 1.95, 0);
			program.setModelMatrix(modelMatrix);
			DrawText(&program, font, "Score: " + std::to_string(score), 0.2, 0);

			modelMatrix.identity();
			modelMatrix.Translate(2.0, 1.95, 0);
			program.setModelMatrix(modelMatrix);
			DrawText(&program, font, "Lives: " + std::to_string(player.hp / 10), 0.2, 0);

			for (int i = 0; i < enemies.size(); i++)
			{
				if (!enemies[i].Dead)
				{
					enemies[i].Draw();
					enemies[i].Update(elapsed);
				}
			}

			for (int i = 0; i < rocks.size(); i++)
			{
				if (!rocks[i].Dead) rocks[i].Draw();
			}

			for (int i = 0; i < playerBullets.size(); i++)
			{
				playerBullets[i].Update(elapsed);
				playerBullets[i].Draw();
			}

			shootTimmer += elapsed;
			if (shootTimmer > 2)
			{

				int enemyInd = rand() % 36;
				if (!enemies[enemyInd].Dead)
				{
					Mix_PlayChannel(-1, eFire, 0);
					bulls.position.x = enemies[enemyInd].position.x;
					bulls.position.y = enemies[enemyInd].position.y - bulls.size.y / 2 - enemies[enemyInd].size.y / 2;
					bulls.velocity.y = -3;
					shootTimmer = 0;
					
				}

			}

			for (int i = 0; i < playerBullets.size(); i++) {
				if (playerBullets[i].position.y > 1.5) {
					playerBullets.erase(playerBullets.begin() + i);
				}
				break;
			}

			for (int i = 0; i < 36; i++)
			{
				if (enemies[i].position.x >= 3.5 || enemies[i].position.x <= -3.5)
				{
					coll = true;
				}
		
	
				for (int j = 0; j < playerBullets.size(); j++)
				{
					if (playerBullets[j].Collision(enemies[i]) && !enemies[i].Dead)
					{
						Mix_PlayChannel(-1, eHit, 0);
						playerBullets.erase(playerBullets.begin() + j);
						enemies[i].Dead = true;
						enemiesDead += 1;
						score += 10;
						
					}
				}
			}

			if (bulls.Collision(player))
			{
				Mix_PlayChannel(-1, pHit, 0);
				player.hp -= 10;
				if (player.hp == 0) player.Dead = true;
				bulls.position.x = 10;
				Mix_PlayChannel(-1, pHit, 0);
			}

			for (int i = 0; i < rocks.size(); i++)
			{
				if (!rocks[i].Dead){
					for (int j = 0; j < playerBullets.size(); j++)
					{
						if (playerBullets[j].Collision(rocks[i]))
						{
							playerBullets.erase(playerBullets.begin() + j);
						}
					}

					if (bulls.Collision(rocks[i]))
					{
						rocks[i].hp -= 10;
						if (rocks[i].hp == 0) rocks[i].Dead = true;
						bulls.position.x = 10;
					}
				}
			}

			for (int i = 0; i < 36; i++)
			{
				if (coll)
				{
					enemies[i].velocity.x *= -1;
				}
				
			
			}
			coll = false;
			

			if (player.Dead || enemiesDead == enemies.size())
			{
				if (player.Dead) result = "YOU LOSE"; else result = "YOU WIN";
				shootTimmer = 0;
				score = 0;
				enemiesDead = 0;
				player.hp = 50;
				player.Dead = false;
				player.position.x = 0;
				state = GAME_OVER;

			}
			break;

		case GAME_OVER:
			
			reset(enemies, rocks);
	
			modelMatrix.identity();
			modelMatrix.Translate(-2.3, 0, 0);
			program.setModelMatrix(modelMatrix);

			DrawText(&program, font, result, 0.5, 0);

			modelMatrix.identity();
			modelMatrix.Translate(-2.3, -1, 0);
			program.setModelMatrix(modelMatrix);

			DrawText(&program, font, "Press space to restart...", 0.2, 0);

			break;
		}

		SDL_GL_SwapWindow(displayWindow);
	}

	Mix_FreeChunk(pFire);
	Mix_FreeChunk(eFire);
	Mix_FreeChunk(pHit);
	Mix_FreeChunk(eHit);

	SDL_Quit();
	return 0;
}
