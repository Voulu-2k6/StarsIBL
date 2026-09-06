#ifndef STARS_IBL_CAMERA__
#define STARS_IBL_CAMERA__

#include "StarsIBL_include.h"

class Camera{
private:
 // world space
	vec3 global_up = vec3(0.0, 1.0, 0.0);
	vec3 pos;

 // tangent space
	vec3 right;
	vec3 front;

 // cursor info
	float sensitivity;
	double x_pos = 0.0;
	double y_pos = 0.0;
	bool active = false;

 // player info
	float speed;

public:
	~Camera() = default;
	Camera(vec3 p, vec3 f, float sen, float spe){
		pos = p;
		sensitivity = sen;
		speed = spe;

		front = normalize(f);
		right = (front.y > 0.99) ? normalize(cross(front, vec3(0.0, 0.0, 1.0))): normalize(cross(front, global_up));
	}

 // mutator methods
	void set_sens(float s){ sensitivity = s; }
	void set_front(vec3 f){ front = f; right = (front.y > 0.99) ? normalize(cross(front, vec3(0.0, 0.0, 1.0))): normalize(cross(front, global_up)); }
	void set_pos(vec3 p){ pos = p; }
	void set_speed(float s){ speed = s; }

 // access methods
	vec3 get_pos(){ return pos; }
	mat4 get_view(){ return lookAt(pos, pos + front, global_up); }

 // move according to new cursor coords
	void handle_mouse(double x, double y){
		if(!active){ // first mouse movement
			x_pos = x;
			y_pos = y;
			active = true;
			return;
		}

		double theta = radians((x_pos - x)*sensitivity);
		double phi = radians((y_pos - y)*sensitivity);
		x_pos = x;
		y_pos = y;
		quaternion vertical = quaternion(right, 1.0f);
		quaternion horizontal = quaternion(global_up, 1.0f);

		front = normalize(vertical.rotate(front, phi));
		front = normalize(horizontal.rotate(front, theta));
		right = (front.y > 0.99) ? normalize(cross(front, vec3(0.0, 0.0, 1.0))): normalize(cross(front, global_up));

	}

	void handle_move(GLFWwindow* win, float dt){
		float strafe = 0;
		float forward = 0;
		if(glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(win, GLFW_KEY_UP) == GLFW_PRESS){
			forward++;
		}
		if(glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(win, GLFW_KEY_DOWN) == GLFW_PRESS){
			forward--;
		}
		if(glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(win, GLFW_KEY_RIGHT) == GLFW_PRESS){
			strafe++;
		}
		if(glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(win, GLFW_KEY_LEFT) == GLFW_PRESS){
			strafe--;
		}
		if(abs(strafe) == 1 && abs(forward) == 1){
			strafe *= sqrt(2)*0.5;
			forward *= sqrt(2)*0.5;
		}

		strafe *= dt*speed;
		forward *= dt*speed;

		if(strafe != 0){
			pos = vec3(pos.x + right.x*strafe, pos.y + right.y*strafe, pos.z + right.z*strafe);
		}
		if(forward != 0){
			vec3 flat_front = normalize(cross(global_up, right));
			pos = vec3(pos.x + flat_front.x*forward, pos.y + flat_front.y*forward, pos.z + flat_front.z*forward);
		}

	}
};

#endif
