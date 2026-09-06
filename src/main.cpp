// Main file for rendering the scene.

#include "StarsIBL_include.h"

// function prototypes
void starsilb_handle_input(GLFWwindow* w, float dt);
void starsilb_mouse_callback(GLFWwindow* w, double x, double y);
void update_UBO(unsigned int ubo, size_t size, void* data);
void open_menu(GLFWwindow* w);

/* ============================================================================================== *
 *     SCENE GLOBALS (QF0)								 										  *
 * ============================================================================================== */

const int SCREEN_WIDTH = 1800;
const int SCREEN_HEIGHT = 1800;
auto camera = (shared_ptr<Camera>) make_shared<Camera>(vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, -1.0), 1.0, 1.0);
bool menu = false;

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
	glfwSetCursorPosCallback(win, starsilb_mouse_callback);
	//glfwSetFramebufferSizeCallback
	glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


 // Initialize OpenGL pointers with glad
	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){ cout << "StarsIBL: Glad failed to load" << endl; return -1; }
	if(haltCheck("glad")){ cout << "Killing program" << endl; glfwTerminate(); return 1; }

 // openGL settings
	glPointSize(20.0f);
	glEnable(GL_CULL_FACE);

/* ============================================================================================== *
 *     CREATING SCENE OBJECTS (QF 1.1)								 						      *
 * ============================================================================================== */

	int res_theta = 1000;
	int res_phi = 1000;
	auto sphere = (shared_ptr<Sphere>) make_shared<Sphere>(0.8, res_theta, res_phi);
	if(haltCheck("sphere")){ cout << "Killing program" << endl; glfwTerminate(); return 1; }

/* ============================================================================================== *
 *     INITIALIZE THE CAMERA (QF 1.2)								 						      *
 * ============================================================================================== */

	vec3 cam_pos_init = vec3(0.0);
	vec3 cam_front_init = vec3(0.0, 0.0, -1.0);
	float cam_sens = 0.03;
	float player_speed = 2.0;
	camera->set_pos(cam_pos_init);
	camera->set_front(cam_front_init);
	camera->set_sens(cam_sens);
	camera->set_speed(player_speed);

/* ============================================================================================== *
 *     CREATING SCENE TRANSFORM UBO (QF 1.3)								 					  *
 * ============================================================================================== */

	vector<mat4> transforms;
	mat4 model = translate(mat4(1.0), vec3(0.0, 0.0, -2.0));
	mat4 view = lookAt(vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, -1.0), vec3(0.0, 1.0, 0.0));         // eye, destination, localup (TBR with camera)
	mat4 proj = perspective((float)(PI*0.35), SCREEN_WIDTH/(float)SCREEN_HEIGHT, 0.1f, 100.0f); // fov, aspect ratio, near plane, far plane
	transforms.push_back(model);
	transforms.push_back(view);
	transforms.push_back(proj);

	unsigned int matrix_binding_point = 0;
	unsigned int UBO_matrix;
	glGenBuffers(1, &UBO_matrix);
	glBindBuffer(GL_UNIFORM_BUFFER, UBO_matrix);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(mat4)*transforms.size(), &transforms[0], GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, matrix_binding_point, UBO_matrix);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);

/* ============================================================================================== *
 *     CREATING SHADER PROGRAMS (QF 1.4)								 						  *
 * ============================================================================================== */

	auto main_program = (shared_ptr<Shader>) make_shared<Shader>(MAIN_VSRC, nullptr, MAIN_FSRC);
	if(!main_program->is_valid()){ cout << "StarsIBL: main: main_shader program failed." << endl; glfwTerminate(); return 0; }
	main_program->use();
	main_program->set_i("transforms", matrix_binding_point);
	main_program->set_3f("center", vec3(0.0));

/* ============================================================================================== *
 *     RENDERING LOOP (QF2)								 										  *
 * ============================================================================================== */
	if(haltCheck("pre-render")){ cout << "Killing program" << endl; glfwTerminate(); return 1; }

	watch my_watch; // struct to handle time, .tick(); returns the dt between calls
	my_watch.set_frame_count(true);
	while(!glfwWindowShouldClose(win)){

		starsilb_handle_input(win, my_watch.tick()); // handle system inputs

		// update view matrix from camera
		transforms[1] = camera->get_view();
		update_UBO(UBO_matrix, sizeof(mat4)*transforms.size(), (void *)&transforms[0]);
		if(haltCheck("camera")){ cout << "Killing program" << endl; glfwTerminate(); return 1; }

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

void starsilb_handle_input(GLFWwindow* w, float dt){
	if(glfwGetKey(w, GLFW_KEY_ESCAPE) == GLFW_PRESS){
		if(menu){ glfwSetWindowShouldClose(w, true); }
		else { menu = true; open_menu(w); }
	}
	camera->handle_move(w, dt);
}

void starsilb_mouse_callback(GLFWwindow* w, double x, double y){
	if(!menu){ 	camera->handle_mouse(x, y); }
}

void update_UBO(unsigned int ubo, size_t size, void* data){
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferData(GL_UNIFORM_BUFFER, size, data, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void open_menu(GLFWwindow* w){
	glfwSetInputMode(w, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}
