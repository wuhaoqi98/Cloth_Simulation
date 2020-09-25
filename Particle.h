#pragma once

#include "Core.h"

class Particle
{
private:
	
	glm::vec3 force;
	bool isStatic;

public:
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 normal;
	float mass;

	Particle(glm::vec3 pos, float m);
	~Particle();

	void Update(float dt);
	void Draw();
	void ApplyForce(glm::vec3 f);
	void SetStatic();
	void CheckCollision(float y0, float elasticity, float friction);
};

