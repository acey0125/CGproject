#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <SOIL2/SOIL2.h>
#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
using namespace std;

string readShaderFile(string filepath);
GLuint getShaderProgram(string fshader, string vshader);
GLuint TextureMap(vector<const GLchar*> faces);