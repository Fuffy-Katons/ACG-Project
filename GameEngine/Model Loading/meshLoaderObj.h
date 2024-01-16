#pragma once

#include <vector>
#include <GLEW\glew.h>
#include <GLFW\glfw3.h>
#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
#include "mesh.h"

class MeshLoaderObj
{
	public:
		MeshLoaderObj();
		Mesh loadObj(const std::string &filename, std::vector<Texture> textures);
		Mesh loadObj(const std::string &filename);
};

