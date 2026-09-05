// Main file for rendering the scene.

#include "StarsIBL_include.h"

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
int main(int argc, char** argv){
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
	if(haltCheck("glad")){ cout << "Killing program" << endl; glfwTerminate(); return 1; }

 // openGL settings
	glPointSize(20.0f);

/* ============================================================================================== *
 *     CREATING SHADER PROGRAMS (QF 1.1)								 						  *
 * ============================================================================================== */

	auto main_program = (shared_ptr<Shader>) make_shared<Shader>(MAIN_VSRC, nullptr, MAIN_FSRC);
	if(!main_program->is_valid()){ cout << "StarsIBL: main: main_shader program failed." << endl; glfwTerminate(); return 0; }
	if(haltCheck("shader")){ cout << "Killing program" << endl; glfwTerminate(); return 1; }

/* ============================================================================================== *
 *     CREATING SCENE OBJECTS (QF 1.2)								 						      *
 * ============================================================================================== */

	auto sphere = (shared_ptr<Sphere>) make_shared<Sphere>(vec3(0.0, 0.0, -2.0), 0.8);
	if(haltCheck("sphere")){ cout << "Killing program" << endl; glfwTerminate(); return 1; }

/* ============================================================================================== *
 *     RENDERING LOOP (QF2)								 										  *
 * ============================================================================================== */
	while(!glfwWindowShouldClose(win)){
		starsilb_handle_input(win); // handle system inputs

		glClearColor(0.5, 0.0, 1.0, 1.0); // using purple,
		glClear(GL_COLOR_BUFFER_BIT);     // clear the old frame

		// render frame
		sphere->Draw(main_program);
		if(haltCheck("draw")){ cout << "Killing program" << endl; glfwTerminate(); return 1; }

		glfwSwapBuffers(win); // show the rendered frame
		glfwPollEvents();     // checking for system inputs
	}

	cout << "Test, hello " << ((argc > 1) ? argv[1] : "0") << endl;

	glfwTerminate();
	return 0;
}

/* ============================================================================================== *
 *     FUNCTION DEFINITIONS (QF3)								 								  *
 * ============================================================================================== */

void starsilb_handle_input(GLFWwindow* w){
	if(glfwGetKey(w, GLFW_KEY_ESCAPE) == GLFW_PRESS){
		glfwSetWindowShouldClose(w, true);
	}
}
