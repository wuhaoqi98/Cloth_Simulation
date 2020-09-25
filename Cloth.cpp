#include "Cloth.h"



Cloth::Cloth(float width, float height, int numX, int numY, 
	float weight, float springConst, float dampConst, glm::mat4 transform)
{
	glGenBuffers(1, &VBO);
	g = glm::vec3(0, -9.8f, 0);
	airConst = 1.29f * 0.05f;
	windDir = glm::normalize(glm::vec3(0, -7, -5));
	windSpeed = 10;
	dt = 0.002f;
	moveSpeed = 5;
	planeSize = 5;

	particleNumX = numX;
	particleNumY = numY;
	toWorld = transform;

	float
		particleDistX = width / (numX-1),
		particleDistY = height / (numY-1),
		numParticles = numX * numY,
		particleMass = weight / numParticles;

	//Create particles
	for (int y = 0; y < numY; y++) {
		for (int x = 0; x < numX; x++) {
			glm::vec3 pos = glm::vec3(x * particleDistX, -y * particleDistY, 0);
			particles.push_back(new Particle(pos, particleMass));
		}
	}
		

	//Create triangles
	for (int y = 1; y < numY; y++) {
		for (int x = 1; x < numX; x++) {
			// p1 - p2
			//  | / |
			// p3 - p4
			Particle* p1 = particles[(y - 1)*numX + x - 1];
			Particle* p2 = particles[(y - 1)*numX + x];
			Particle* p3 = particles[y*numX + x - 1];
			Particle* p4 = particles[y*numX + x];

			triangles.push_back(new Triangle(p3, p1, p2));
			triangles.push_back(new Triangle(p2, p4, p3));
		}
	}

	//Define dampers between the particles
	for (int i = 0; i < numParticles; i++) {
		bool
			leftCol = i % numX == 0,
			rightCol = (i + 1) % numX == 0,
			topRow = i < numX,
			topRows = i < numX * 2, //top 2 rows
			leftCols = leftCol || (i - 1) % numX == 0; //2 leftmost columns

		//vertical (up)
		if (!topRow)
			dampers.push_back(new SpringDamper(particles[i], particles[i - numX], springConst, dampConst));
		//if (!topRows)
		//	dampers.push_back(new SpringDamper(particles[i], particles[i - (numX * 2)], springConst, dampConst));

		//horizontal (left)
		if (!leftCol)
			dampers.push_back(new SpringDamper(particles[i], particles[i - 1], springConst, dampConst));
		//if (!leftCols)
		//	dampers.push_back(new SpringDamper(particles[i], particles[i - 2], springConst, dampConst));


		//diagonal \  .
		if (!topRow && !leftCol)
			dampers.push_back(new SpringDamper(particles[i], particles[i - 1- numX], springConst, dampConst));

		//diagonal /  .
		if (!topRow && !rightCol)
			dampers.push_back(new SpringDamper(particles[i], particles[i +1- numX], springConst, dampConst));
	}

	//give the cloth a tiny push in the z direction
	//particles[numParticles / 2 + numX / 2]->ApplyForce(glm::vec3(0, 0, -1));

	//Make top edge static
	for (int i = 0; i < numX; i++) {
		particles[i]->SetStatic();
	} 

	vertices = vector<Vertex>(3 * triangles.size());
	Vertex v1 = { glm::vec3(-planeSize, -3, planeSize), glm::vec3(0,1,0) };
	Vertex v2 = { glm::vec3(planeSize, -3, planeSize), glm::vec3(0,1,0) };
	Vertex v3 = { glm::vec3(-planeSize, -3, -planeSize), glm::vec3(0,1,0) };
	Vertex v4 = { glm::vec3(planeSize, -3, -planeSize), glm::vec3(0,1,0) };
	vertices.push_back(v1);
	vertices.push_back(v2);
	vertices.push_back(v4);
	vertices.push_back(v1);
	vertices.push_back(v3);
	vertices.push_back(v4);
	Update();
}


Cloth::~Cloth()
{
}


void Cloth::SetBuffers(const vector<Vertex> &vertices) {

	// Store vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void Cloth::Move(glm::vec3 v)
{
	for (int i = 0; i < particleNumX; i++) {
		particles[i]->position += v * moveSpeed;
	}
}

void Cloth::Update()
{
	// apply gravity 
	for (int i = 0; i < particles.size(); i++) {
		particles[i]->ApplyForce(particles[i]->mass * g);
	}
	// compute damper forces
	for (int i = 0; i < dampers.size(); i++) {
		dampers[i]->ComputeForce();
	}
	// calculate aerodynamic forces
	for (int i = 0; i < triangles.size(); i++) {
		triangles[i]->CalculateForces(airConst, windDir * windSpeed);
	}
	// integrate motion
	for (int i = 0; i < particles.size(); i++) {
		particles[i]->Update(dt);
		particles[i]->CheckCollision(-2.9f, 1, 0.2f);
	}


	// calculate normal of particles
	for (int i = 0; i < particles.size(); i++) {
		particles[i]->normal = glm::vec3(0);
	}
	for (int i = 0; i < triangles.size(); i++) {
		triangles[i]->UpdateParticles();
	}
	for (int i = 0; i < particles.size(); i++) {
		particles[i]->normal = glm::normalize(particles[i]->normal);
		//particles[i]->normal = glm::vec3(0);
	}

	// put all particles in vertices
	for (int i = 0; i < triangles.size(); i++) {
		Triangle* t = triangles[i];
		vertices[3 * i] = { t->p1->position, t->p1->normal };
		vertices[3 * i + 1] = { t->p2->position, t->p2->normal };
		vertices[3 * i + 2] = { t->p3->position, t->p3->normal };
	}

	SetBuffers(vertices);
}

void Cloth::Draw(const glm::mat4 &viewProjMtx, uint shader) {
	// Set up shader
	glUseProgram(shader);
	glUniformMatrix4fv(glGetUniformLocation(shader, "ModelMtx"), 1, false, (float*)&toWorld);

	glm::mat4 mvpMtx = viewProjMtx * toWorld;
	glUniformMatrix4fv(glGetUniformLocation(shader, "ModelViewProjMtx"), 1, false, (float*)&mvpMtx);

	// Set up state
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	uint posLoc = 0;
	glEnableVertexAttribArray(posLoc);
	glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

	uint normLoc = 1;
	glEnableVertexAttribArray(normLoc);
	glVertexAttribPointer(normLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)12);

	glUniform3fv(glGetUniformLocation(shader, "LightColor"), 1, &glm::vec3(1, 0, 0)[0]);
	// Draw geometry
	glDrawArrays(GL_TRIANGLES, 0, triangles.size()*3);
	glUniform3fv(glGetUniformLocation(shader, "LightColor"), 1, &glm::vec3(1, 1, 1)[0]);
	glDrawArrays(GL_TRIANGLES, triangles.size() * 3, 6);

	// Clean up state
	glDisableVertexAttribArray(normLoc);
	glDisableVertexAttribArray(posLoc);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glUseProgram(0);
}