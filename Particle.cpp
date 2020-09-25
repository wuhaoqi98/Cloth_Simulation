#include "Particle.h"



Particle::Particle(glm::vec3 pos, float m)
{
	position = pos;
	velocity = glm::vec3(0);
	mass = m;
	force = glm::vec3(0);
	isStatic = false;
	normal = glm::vec3(0);
}


Particle::~Particle()
{
}

void Particle::Update(float dt)
{
	if (isStatic) {
		return;
	}
	glm::vec3 acc = (1.0f / mass) * force;
	velocity += acc * dt;
	position += velocity * dt;
	
	force = glm::vec3(0);
}

void Particle::Draw()
{
}

void Particle::ApplyForce(glm::vec3 f)
{
	force += f;
}

void Particle::SetStatic()
{
	isStatic = true;
}

void Particle::CheckCollision(float y0, float elasticity, float friction)
{
	if (position.y < y0) {
		position.y += y0 - position.y;
		velocity.y = -elasticity * velocity.y;
		velocity.x = (1 - friction) * velocity.x;
		velocity.z = (1 - friction) * velocity.z;
	}
}
