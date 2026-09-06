#ifndef STARS_IBL_SPHERE__
#define STARS_IBL_SPHERE__

#include "StarsIBL_include.h"

class Sphere{
private:
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;

	double radius;

	vector<vec3> vertices;
	vector<unsigned int> indices;

	// vec3 albedo;
	// vec3 reflectance;
	// float roughness;
	// float metal;

	void getVertexData(int m, int n){

		int layer_size = m + 4;

		double step_phi = PI/(n + 2);       // span the hemisphere over y
		double phi = PI*0.5;				// begin at phi PI*0.5, sin(phi) = 1;

		double step_theta = (2*PI)/(layer_size); // span the circle over theta
		double theta = 0.0;					// begin at theta 0, reset each level so this is just for clarity

		double local_y = radius; 			// begin at the peak of the sphere and iterate down (local_y = radius*sin(phi))
		double local_r = 0.0;               // radius at each y level of the sphere, also reset each level

		// generate vertices in model space
		for(int i = -1; i <= n + 1; i++){   // foreach level
			if(i >= 0 && i < n + 1){ 	         // if not on the poles
				theta = 0.0f;
				local_r = sqrt(radius*radius - local_y*local_y);
				for(int j = 0; j < layer_size; j++){  // iterate around the current ring
					theta += step_theta;
					vertices.push_back(vec3(local_r*sin(theta), local_y, local_r*cos(theta)));
				}
			}
			else{					       		 // else single vertex for poles
				vertices.push_back(vec3(0.0, local_y, 0.0));
			}
			phi -= step_phi;
			local_y = radius*sin(phi); 		// drop to the next level
		}

		// generate indices
		// circles iterate towards positive x from positive z, ensure proper winding
		// we should end up with some
		int count = 1; // begin just under the cap vertex
		for(int i = 0; i < layer_size; i ++){
			indices.push_back(0);
			indices.push_back(count + i);
			indices.push_back(count + (i + 1) % layer_size);
		}
		for(int j = 0; j < n; j++){
			count = 1 + j*layer_size;
			for(int i = 0; i < layer_size; i++){ // twice as many triangles per row as the ends
				indices.push_back(count + i);
				indices.push_back(count + layer_size + i);
				indices.push_back((count + layer_size) + (i + 1) % layer_size);

				indices.push_back(count + i);
				indices.push_back((count + layer_size) + (i + 1) % layer_size);
				indices.push_back(count + (i + 1) % layer_size);
			}
		}
		int bottom = vertices.size() - 1;
		count = bottom - layer_size; // one for the
		for(int i = 0; i < m + 4; i++){ // m + 4 triangles on the bottom
			indices.push_back(count + i);
			indices.push_back(bottom);
			indices.push_back(count + (i + 1) % layer_size);
		}

		haltCheck("Sphere: GetVertexData");
	}

	void setUpMesh(int m, int n){
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
		getVertexData(m, n);
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
	Sphere(float r, int m, int n): radius(r) { setUpMesh(m, n); }
	void Draw(shared_ptr<Shader> shader){
		shader->use();
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
};

#endif
