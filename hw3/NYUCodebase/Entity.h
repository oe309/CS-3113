#pragma once
#include "Vector3.h";
#include "SheetSprite.h";


class Entity {
public:
	Entity() :Dead(false),hp(50){ }
	Entity(SheetSprite sprt) :sprite(sprt), Dead(false), hp(50)
	{
		size.x = (sprt.width / sprt.height)*sprt.size;
		size.y = sprt.size;
		if (hp > 0) Dead = false; else Dead = true;
	}
	void Update(float elapsed);
	void Draw();
	bool Collision(Entity other);
	Vector3 position;
	Vector3 velocity;

	Vector3 size;
	float rotation;
	SheetSprite sprite;
	bool Dead;
	int hp;
};

