#ifdef _WINDOWS
	#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "ShaderProgram.h";
#include "Matrix.h";


#ifdef _WINDOWS
	#define RESOURCE_FOLDER ""
#else
	#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

SDL_Window* displayWindow;


GLuint LoadTexture(const char *image_path){
	SDL_Surface *surface = IMG_Load(image_path);

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	SDL_FreeSurface(surface);

	return textureID;
}


int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
	glewInit();
#endif

	glViewport(0, 0, 1280,720);

	ShaderProgram program(RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl");


	Matrix projectionMatrix;
	Matrix modelMatrix;
	Matrix viewMatrix;
	projectionMatrix.setOrthoProjection(-3.55, 3.55, -2.0f, 2.0f, -1.0f, 1.0f);
	glUseProgram(program.programID);
	
	float lastFrameTicks = 0.0f;
	float p1y = 0.0f;
	float p2y = 0.0f;
	float ball_x = 0.0f;
	float ball_y = 0.0f;
	float ball_vx = 3.0f;
	float ball_vy = 3.0f;

	SDL_Event event;
	bool done = false;
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

		const Uint8 *keys = SDL_GetKeyboardState(NULL);

		if (keys[SDL_SCANCODE_W])
		{
			if (p1y + 0.6f < 2.0f)
			{
				p1y += elapsed*3;
			}
		}
		if (keys[SDL_SCANCODE_S])
		{
			if (p1y - 0.6f > -2.0f)
			{
				p1y -= elapsed*3;
			}
		}
		if (keys[SDL_SCANCODE_UP])
		{
			if (p2y + 0.6f < 2.0f)
			{
				p2y += elapsed*3;
			}
		}
		if (keys[SDL_SCANCODE_DOWN])
		{
			if (p2y - 0.6f > -2.0f)
			{
				p2y -= elapsed*3;
			}
		}
		
		//Left Paddle
		modelMatrix.identity();
		modelMatrix.Translate(0, p1y, 0);
		program.setModelMatrix(modelMatrix);
		program.setProjectionMatrix(projectionMatrix);
		program.setViewMatrix(viewMatrix);
	
		float vertices[] = { -3.5f, -0.5f, -3.3f, -0.5f, -3.3f, 0.5f, -3.5f, -0.5f, -3.3f, 0.5f, -3.5f, 0.5f };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
		glEnableVertexAttribArray(program.positionAttribute);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		
		
		//Right Paddle 
		modelMatrix.identity();
		modelMatrix.Translate(0, p2y, 0);
		program.setModelMatrix(modelMatrix);
		float vertices2[] = { 3.5f, -0.5f, 3.3f, -0.5f, 3.3f, 0.5f, 3.5f, -0.5f, 3.3f, 0.5f, 3.5f, 0.5f };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices2);
		glEnableVertexAttribArray(program.positionAttribute);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		
		
		//Ball
		ball_x += ball_vx*elapsed;
		ball_y += ball_vy*elapsed;
		modelMatrix.identity();
		modelMatrix.Translate(ball_x,ball_y,0);
		program.setModelMatrix(modelMatrix);
		float vertices3[] = { 0.1f, -0.1f, -0.1f, -0.1f, -0.1f, 0.1f, 0.1f, -0.1f, -0.1f, 0.1f, 0.1f, 0.1f };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices3);
		glEnableVertexAttribArray(program.positionAttribute);
		glDrawArrays(GL_TRIANGLES, 0, 6);


		//if ball hits top or bottom
		if (ball_y + 0.1f > 2.0f || ball_y - 0.1f < -2.0f)
		{
			ball_vy *= -1.0f;
		}

		//if ball collides with left paddle 
		else if ((ball_x - 0.1f <= -3.3f) && (ball_y - 0.1f <= p1y + 0.5f) && (ball_y + 0.1f >= p1y - 0.5f))
		{
			ball_vx *= -1.0f;
		}

		//if ball collides with right paddle 
		else if ((ball_x + 0.1f >= 3.3f) && (ball_y - 0.1f <= p2y + 0.5f) && (ball_y + 0.1f >= p2y - 0.5f)){
			ball_vx *= -1.0f;
		}

		//Player 1 Wins
		else if (ball_x + 0.1f >= 3.55f)
		{
			p1y = 0.0f;
			p2y = 0.0f;
			ball_x = 0.0f;
			ball_y = 0.0f;
			ball_vx = 3.0f;
			ball_vy = 3.0f;
		}

		//Player 2 Wins
		else if (ball_x - 0.1f <= -3.55f)
		{
			p1y = 0.0f;
			p2y = 0.0f;
			ball_x = 0.0f;
			ball_y = 0.0f;
			ball_vx = 3.0f;
			ball_vy = 3.0f;
		}


		glDisableVertexAttribArray(program.positionAttribute);


		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return 0;
}
