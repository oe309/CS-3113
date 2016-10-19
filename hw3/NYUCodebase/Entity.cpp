#include "Entity.h"


void Entity::Update(float elapsed)
{

	position.x += velocity.x*elapsed;
	position.y += velocity.y*elapsed;
}

void Entity::Draw()
{
	Matrix modelMatrix;
	modelMatrix.identity();
	modelMatrix.Translate(position.x, position.y, position.z);

	sprite.program->setModelMatrix(modelMatrix);
	sprite.Draw();
}

bool Entity::Collision(Entity object)
{
	if (position.x + size.x / 2 <= object.position.x - object.size.x / 2 || position.x - size.x / 2 >= object.position.x + object.size.x / 2 ||
		position.y + size.y / 2 <= object.position.y - object.size.y / 2 || position.y - size.y / 2 >= object.position.y + object.size.y / 2)
	{
		return false;
	}
	else
		return true;
}