#ifndef STARS_IBL_SHADER__
#define STARS_IBL_SHADER__

#include "StarsIBL_include.h"

class Shader {
private:
	unsigned int id;
	bool valid = true;
	int load_shader_from_source(const char* source, GLenum type){

	 // stream utilities
		const char* code;
		string str_buf;
		stringstream str_strm;
		ifstream f_strm;
		f_strm.exceptions(ifstream::failbit | ifstream::badbit);

	 // load shader
		try{
			f_strm.open(source);
			str_strm << f_strm.rdbuf();
			str_buf = str_strm.str();
			code = str_buf.c_str();
		}
		catch(ifstream::failure &e){
			cout << "StarsIBL: Shader: File read failure: " << e.what() << endl;
			f_strm.close();
			return -1;
		}
		f_strm.close();

	 // compile shader
		unsigned int shader;
		shader = glCreateShader(type);
		glShaderSource(shader, 1, &code, NULL);
		glCompileShader(shader);

	 // error check
		int success;
		char infoLog[512];
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if(!success){
			glGetShaderInfoLog(shader, 512, NULL, infoLog);
			string s = (type == GL_VERTEX_SHADER) ? "vertex" : (type == GL_FRAGMENT_SHADER) ? "fragment" : "geometry";
			cout << "StarsIBL: Shader: Error compiling " << s << " shader: " << infoLog; // opengl should append a newline to the error message already
			return -1;
		}

	    return (int) shader;
	}
public:
	~Shader(){ glDeleteProgram(id); }
	Shader(const char* v_source, const char* g_source, const char* f_source){
		bool do_geo = (g_source != nullptr);

	 // load shaders
		int vertex = load_shader_from_source(v_source, GL_VERTEX_SHADER);
		int geometry = do_geo ? load_shader_from_source(g_source, GL_GEOMETRY_SHADER) : -1;
		int fragment = load_shader_from_source(f_source, GL_FRAGMENT_SHADER);
		if(haltCheck("shader: load")){ valid = false; }

	 // error check
		if(vertex < 0 || fragment < 0 || (do_geo && geometry < 0)){
			cout << "StarsIBL: Shader: could not create program" << endl;
			valid = false;
			id = 0;

			glDeleteShader((unsigned int) vertex);
			if(do_geo){ glDeleteShader((unsigned int) geometry); }
			glDeleteShader((unsigned int) fragment);

			return;
		}
		if(haltCheck("shader: error check")){ valid = false; }


	 // create shader program
		id = glCreateProgram();
		glAttachShader(id, (unsigned int) vertex);
		if(do_geo){ glAttachShader(id, (unsigned int) geometry); }
		glAttachShader(id, (unsigned int) fragment);
		glLinkProgram(id);
		if(haltCheck("shader: link")){ valid = false; }

	 // cleanup
		glDeleteShader((unsigned int) vertex);
		if(do_geo){ glDeleteShader((unsigned int) geometry); }
		glDeleteShader((unsigned int) fragment);
		if(haltCheck("shader: delete")){ valid = false; }

	}

	bool is_valid(){ return valid; }
	void use(){ glUseProgram(id); }
};

#endif
