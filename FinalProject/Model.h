#pragma once

#include <iostream>
#include <map>
#include <SOIL2/SOIL2.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Mesh.h"

class Model
{
public:
	std::vector<Mesh> meshes;
	std::map<std::string, GLuint> textureMap;
	std::float_t maxX = 0.0f;
	std::float_t maxY = 0.0f;
	std::float_t maxZ = 0.0f;
	std::float_t minX = 0.0f;
	std::float_t minY = 0.0f;
	std::float_t minZ = 0.0f;
	glm::vec3 translate = glm::vec3(0, 0, 0), rotate = glm::vec3(0, 0, 0), scale = glm::vec3(1, 1, 1);
	bool show = true;
	int id;
	Model() {};
	Model(int tid) { id = tid; }
	void load(std::string filepath);
	void draw(GLuint program);
	void Change(double ScaleX, double ScaleY, double ScaleZ, double TransX, double TransY, double TransZ);

};