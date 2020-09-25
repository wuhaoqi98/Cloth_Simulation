#pragma once

#include "Particle.h"

class Triangle
{
public:

	Particle* p1;
	Particle* p2;
	Particle* p3;
	glm::vec3 normal;

	Triangle(Particle* p1, Particle* p2, Particle* p3);
	~Triangle();

	void CalculateForces(float c, glm::vec3 airVelocity);
	void UpdateParticles();
};

