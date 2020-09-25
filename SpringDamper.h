#pragma once

#include "Particle.h"

class SpringDamper
{
private:
	float k_s;
	float k_d;
	float restLength;
	Particle* p1;
	Particle* p2;

public:
	SpringDamper(Particle* p1, Particle* p2, float springConst, float dampConst);
	~SpringDamper();

	void ComputeForce();
};

