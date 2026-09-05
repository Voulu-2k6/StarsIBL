#ifndef STARS_IBL_SPHERE__
#define STARS_IBL_SPHERE__

#include "StarsIBL_include.h"

class Sphere{
private:
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;

	vec3 center;
	double radius;

	vector<vec3> vertices;
	vector<unsigned int> indices;

	// vec3 albedo;
	// vec3 reflectance;
	// float roughness;
	// float metal;

	void getVertexData(){
		int m = 0; // m triangle strips
		int n = 0; // 4 + n points per ring

		double step_y = (2*radius)/(m + 2); // span the sphere over y
		double local_y = radius; 			// begin at the peak of the sphere and iterate down

		double step_theta = (2*PI)/(n + 4); // span the circle over theta
		double theta = 0.0;					// begin at theta 0, reset each level so this is just for clarity

		double local_r = 0.0;               // radius at each y level of the sphere, also reset each level

		// generate vertices
		for(int i = -1; i <= m + 1; i++){ // foreach level
			if(i >= 0 && i < m + 1){ 	  // if not on the poles
				theta = 0.0f;
				local_r = sqrt(radius*radius - local_y*local_y);
				for(int j = 0; j < n + 4; j++){ // iterate around the current ring
					theta += step_theta;
					vertices.push_back(vec3(local_r*cos(theta), local_y, local_r*sin(theta)));
				}
			}
			else{					  // else single vertex for poles
				vertices.push_back(vec3(0.0, local_y, 0.0));
			}
			local_y -= step_y; // drop to the next level
		}

		// generate indices (TODO)
		for(unsigned int i = 0; i < vertices.size(); i++){
			indices.push_back(i);
		}
		haltCheck("Sphere: GetVertexData");
	}

	void setUpMesh(){
	 // generate the vertex array object (defines vertex attributes)
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

	 // generate the vertex and element (index) array objects
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	 // populate vertex data given center and radius (extend to handle more detail)
		haltCheck("Sphere: Pre GetVertex");
		getVertexData();
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*vertices.size(), &vertices[0], GL_STATIC_DRAW);      // populate vertex buffer
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*indices.size(), &indices[0], GL_STATIC_DRAW); // index

	 // enable attribute pointers: (0 -> position)
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

	 // reset buffer bindings
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
public:
	~Sphere(){ glDeleteBuffers(1, &VBO); glDeleteBuffers(1, &EBO); glDeleteBuffers(1, &VAO); delete this; }
	Sphere(vec3 c, float r): center(c), radius(r) { setUpMesh(); }
	void Draw(shared_ptr<Shader> shader){
		shader->use();
		glBindVertexArray(VAO);
		glDrawElements(GL_POINTS, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
};

#endif
