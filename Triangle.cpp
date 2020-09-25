#include "Triangle.h"



Triangle::Triangle(Particle* p1, Particle* p2, Particle* p3)
{
	this->p1 = p1;
	this->p2 = p2;
	this->p3 = p3;
	normal = glm::vec3(0);
}


Triangle::~Triangle()
{
}

void Triangle::CalculateForces(float c, glm::vec3 airVelocity)
{
	glm::vec3 v = (p1->velocity + p2->velocity + p3->velocity) / 3.0f;
	v -= airVelocity;
	glm::vec3 n = glm::cross((p2->position - p1->position), (p3->position - p1->position));
	normal = glm::normalize(n);
	float a = glm::length(n) / 2.0f;
	a = a * glm::dot(v, normal) / glm::length(v);
	glm::vec3 f = -0.5f * c * glm::length(v) * glm::length(v) * a * normal;
	p1->ApplyForce(f / 3.0f);
	p2->ApplyForce(f / 3.0f);
	p3->ApplyForce(f / 3.0f);
}

void Triangle::UpdateParticles()
{
	p1->normal += normal;
	p2->normal += normal;
	p3->normal += normal;
}
