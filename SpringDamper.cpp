#include "SpringDamper.h"



SpringDamper::SpringDamper(Particle* p1, Particle* p2, float springConst, float dampConst)
{
	this->p1 = p1;
	this->p2 = p2;
	k_s = springConst;
	k_d = dampConst;
	restLength = glm::length(p2->position - p1->position);
}


SpringDamper::~SpringDamper()
{
}

void SpringDamper::ComputeForce()
{
	glm::vec3 d = p2->position - p1->position;
	glm::vec3 e = glm::normalize(d);
	float fsd = -k_s * (restLength - glm::length(d)) - k_d * (glm::dot(e,p1->velocity) - glm::dot(e,p2->velocity));
	glm::vec3 f = fsd * e;
	p1->ApplyForce(f);
	p2->ApplyForce(-f);
}
