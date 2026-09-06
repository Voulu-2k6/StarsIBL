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

// image loader
#include "ftms.h"

// namespace declarations
using namespace std;
using namespace glm;

// common constants
double PI = 3.141592653589793238462643383279502884;

// shader file sources
const char* MAIN_VSRC = "shaders/m_vertex.txt";
const char* MAIN_FSRC = "shaders/m_fragment.txt";

// utility functions
bool haltCheck(const char* where){
	GLenum e;
	bool halt = false;
	while((e = glGetError()) != GL_NO_ERROR){
		cout << "Error checks - " << where << ": ";
		halt = true;
		cout << e << " ";
	}
	if(halt){cout << endl;}
	return halt;
}

// my classes
#include "Shader.h"
#include "Sphere.h"

#endif
