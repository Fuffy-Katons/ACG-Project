#pragma once
#include <glew.h>
#include <glfw3.h>
#include <vector>
#include <string>
#include "../../Dependencies/stb_image.h"
GLuint loadBMP(const char * imagepath);
unsigned int loadCubemap(std::vector<std::string> faces);