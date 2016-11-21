#pragma once
#include "ShaderProgram.h";

class SheetSprite {
public:
	SheetSprite();
	SheetSprite(ShaderProgram *program,unsigned int textureID, float u, float v, float width, float height, float
		size) :program(program),textureID(textureID), u(u), v(v), width(width), height(height), size(size){}
	void Draw();
	float size;
	unsigned int textureID;
	float u;
	float v;
	float width;
	float height;
	ShaderProgram *program;
	
};
