#ifndef STARS_IBL_INCLUDE_ROOT__
#define STARS_IBL_INCLUDE_ROOT__

// common includes
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <memory>

// opengl includes
#include <glad/glad.h>  // pointers
#include <GLFW/glfw3.h> // context
#include <glm/glm.hpp>  // gl math
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// namespace declarations
using namespace std;
using glm::vec3;

// common constants
double PI = 3.141592653589793238462643383279502884;

// shader file sources
const char* MAIN_VSRC = "shaders/m_vertex.txt";
const char* MAIN_FSRC = "shaders/m_fragment.txt";

// utility functions

// my classes
#include "Sphere.h"
#include "Shader.h"

#endif
