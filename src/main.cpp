// Main file for rendering the scene.

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

using namespace std;

// function prototypes
void starsilb_handle_input(GLFWwindow* w);

/* ============================================================================================== *
 *     SCENE GLOBALS (QF0)								 										  *
 * ============================================================================================== */

const int SCREEN_WIDTH = 2880;
const int SCREEN_HEIGHT = 1800;

/* ============================================================================================== *
 *     MAIN FUNCTION (QF1)								 										  *
 * ============================================================================================== */
int main(){
 // Define the GLFW context window
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // Force GLFW to use the version we include
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // shaders written in glsl version 420 core, so create core profile
	GLFWwindow* win = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "StarsIBL", 0, 0); // create window upper-left spanning the screen
	glfwMakeContextCurrent(win);
	//glfwSetFramebufferSizeCallback
	//glfwSetCursorPosCallback
	//glfwSetInputMode

 // Initialize OpenGL pointers with glad
	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){ cout << "StarsIBL: Glad failed to load" << endl; return -1; }

/* ============================================================================================== *
 *     RENDERING LOOP (QF2)								 										  *
 * ============================================================================================== */
	while(!glfwWindowShouldClose(win)){
		starsilb_handle_input(win); // handle system inputs

		glClearColor(0.5, 0.0, 1.0, 1.0); // using purple,
		glClear(GL_COLOR_BUFFER_BIT);     // clear the old frame
		// render frame

		glfwSwapBuffers(win); // show the rendered frame
		glfwPollEvents();     // checking for system inputs
	}

	cout << "Test, hello" << endl;

	return 0;
}

/* ============================================================================================== *
 *     FUNCTION DEFINITIONS (QF3)								 										  *
 * ============================================================================================== */

void starsilb_handle_input(GLFWwindow* w){
	if(glfwGetKey(w, GLFW_KEY_ESCAPE) == GLFW_PRESS){
		glfwSetWindowShouldClose(w, true);
	}
}
