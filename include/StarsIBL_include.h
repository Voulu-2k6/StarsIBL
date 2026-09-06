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

// my structs
struct quaternion{ // quaternions for camera rotation.
	vec3 ijk;
	float i, j, k, w;
	quaternion(vec3 dir, float real): ijk(dir), i(dir.x), j(dir.y), k(dir.z), w(real) {}
	quaternion conj(){ return quaternion(-ijk, w); } // complex conjugate
	quaternion operator*(quaternion q2){             // quaternion mult: i -> j -> k
		float i_ret = j*q2.k - k*q2.j + i*q2.w + w*q2.i;     // jk = i, kj = -i, i*1 = i;
		float j_ret = k*q2.i - i*q2.k + j*q2.w + w*q2.j;     // ki = j, ik = -j, j*1 = j;
		float k_ret = i*q2.j - j*q2.i + k*q2.w + w*q2.k;     // ij = k, ji = -k, k*1 = k;
		float real_ret = -(i*q2.i + j*q2.j + k*q2.k) + w*q2.w;  // i^2 = j^2 = k^2 = ijk = -1, 1*1 = 1;
		return quaternion(vec3(i_ret, j_ret, k_ret), real_ret);
	}
	vec3 rotate(vec3 pos, double alpha){  // rotate p around q by alpha: q(alpha/2)*p*q(alpha/2)^conj
		if (abs(alpha) < 0.00001) { return pos; }
		float halpha = alpha*0.5;
		quaternion p = quaternion(pos, 0.0);
		quaternion q = quaternion(vec3(i*sin(halpha), j*sin(halpha), k*sin(halpha)), cos(halpha));
		quaternion rot = (q*p)*q.conj();
		return rot.ijk;
	}
};

struct watch{
	float l_frame = 0.0;
	float l_second = 0.0;

	float c_frame = 0.0;
	float c_second = 0.0;

	bool do_fps = false;
	int fps = 0;

	float tick(){
		l_frame = c_frame;
		c_frame = glfwGetTime();

		l_second = c_second;
		c_second = (int) c_frame;

		if(do_fps){
			if(l_second != c_second){cout << "FPS: " << fps << endl; fps = 0; }
			fps++;
		}

		return c_frame - l_frame;
	}

	void set_frame_count(bool status){ do_fps = status; }
};

// my classes
#include "Shader.h"
#include "Sphere.h"
#include "Camera.h"

#endif
