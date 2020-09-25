#pragma once

#include "Triangle.h"
#include "SpringDamper.h"

class Cloth
{
public:

	
	glm::mat4 toWorld;
	vector<Particle*> particles;
	vector<SpringDamper*> dampers;
	vector<Triangle*> triangles;
	float windSpeed;

	Cloth(float width, float height, int numParticlesX, int numParticlesY, float weight, float springConst, float dampConst, glm::mat4 transform);
	~Cloth();

	void Update();
	void Draw(const glm::mat4 &viewProjMtx, uint shader);
	void SetBuffers(const vector<Vertex> &vertices);
	void Move(glm::vec3 v);

private:
	uint VBO;
	vector<Vertex> vertices;
	glm::vec3 g;
	float airConst;
	glm::vec3 windDir;
	float dt;
	int particleNumX, particleNumY;
	float moveSpeed;
	float planeSize;
};

