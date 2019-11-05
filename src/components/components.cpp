#include <iostream>
#include <sstream>
#include <fstream>

#include "components.hpp"
#include "../initialize.hpp"

/**
 * Compiles the provided shader.
 *
 * @param shaderPath The path of the shader.
 * @param shaderType The type of shader.
 * @returns A unique ID for the compiled shader.
 */
GLuint compileShader(const std::string& shaderPath, const GLuint shaderType) {
	GLuint shaderID = glCreateShader(shaderType);

	std::string shaderCode;
	std::ifstream shaderCodeStream(shaderPath, std::ios::in);
	if (shaderCodeStream.is_open()) {
		std::stringstream stringStream;
		stringStream << shaderCodeStream.rdbuf();
		shaderCode = stringStream.str();
		shaderCodeStream.close();
	}
	else {
		throw "Couldn't read shader.";
	}

	char const* sourcePointer = shaderCode.c_str();
	glShaderSource(shaderID, 1, &sourcePointer, nullptr);
	glCompileShader(shaderID);

	GLint isCompiled = 0;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE) {
		GLint maxLength = 0;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(shaderID, maxLength, &maxLength, &errorLog[0]);
		printf("%s\n", &errorLog[0]);
		glDeleteShader(shaderID); // Don't leak the shader.
		throw "Couldn't compile shader.";
	}

	return shaderID;
}

/**
 * Given a vertex shader and fragment shader, creates a new shader program.
 * @returns A unique ID for the linked program.
 * @note The provided vertex shader and fragment shader are deleted.
 */
GLuint linkProgram(const GLuint vertexShaderID, const GLuint fragmentShaderID) {
	GLuint programID = glCreateProgram();
	GLint status;

	for (auto shaderID : { vertexShaderID, fragmentShaderID }) {
		glAttachShader(programID, shaderID);
	}

	glLinkProgram(programID);
	glGetProgramiv(programID, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		int length = 0;
		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &length);
		if (length > 0) {
			GLchar infoLog[512];
			glGetProgramInfoLog(programID, 512, nullptr, infoLog);
			std::cerr << infoLog;
		}
		std::exit(1);
	}

	for (auto shaderID : { vertexShaderID, fragmentShaderID }) {
		glDetachShader(programID, shaderID);
		glDeleteShader(shaderID);
	}

	glValidateProgram(programID);
	glGetProgramiv(programID, GL_VALIDATE_STATUS, &status);
	if (status == GL_FALSE) {
		int length = 0;
		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &length);
		if (length > 0) {
			GLchar infoLog[512];
			glGetProgramInfoLog(programID, 512, nullptr, infoLog);
			std::cerr << infoLog;
		}
		std::exit(1);
	}

	return programID;
}


/**
 * Create a program from the given shaders..
 * @returns A unique ID for the program that uses the provided shaders.
 */
GLuint loadShaders(const std::string& vertexShaderPath, const std::string& fragmentShaderPath) {
	GLuint vertexShaderID = compileShader(vertexShaderPath, GL_VERTEX_SHADER);
	GLuint fragmentShaderID = compileShader(fragmentShaderPath, GL_FRAGMENT_SHADER);
	return linkProgram(vertexShaderID, fragmentShaderID);
}

/**
 * Initializes a triangle into the given vertex buffer and array.
 *
 * Creates a buffer and sets vertexBufferID to the new value.
 * Selects the bufferand loads the vertices into it.
 *
 * @param vertexBufferID
 * @param vertexArrayID
 */
bool convertGeometry(tinyobj::mesh_t mesh, const std::vector<tinyobj::real_t>& vertices, GLuint& vertexBufferID, GLuint& vertexArrayID) {
	for (auto vert : mesh.num_face_vertices) {
		// only support triangles
		if (vert != 3) return false;
	}

	std::vector<tinyobj::real_t> vec;
	for (auto index : mesh.indices) {
		uint8_t vertexCount = 3;
		for (int i = 0; i < vertexCount; i++) {
			vec.push_back(vertices[index.vertex_index * vertexCount + i]);
		}
	}

	glGenBuffers(1, &vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tinyobj::real_t) * vec.size(), &vec.at(0), GL_STATIC_DRAW);

	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	return true;
}

renderable::renderable(const std::string& model, const std::string& vertex, const std::string& fragment) {
	auto reader = tinyobj::ObjReader{};
	if (!reader.ParseFromFile(model)) {
		std::cerr << "Couldn't load file " << model << "." << std::endl;
		std::exit(1);
	}

	GLuint vertexBufferID, vertexArrayID;
	GLuint triangles = reader.GetShapes()[0].mesh.num_face_vertices.size();
	if (!convertGeometry(reader.GetShapes()[0].mesh, reader.GetAttrib().vertices, vertexBufferID, vertexArrayID)) {
		std::cout << "Error parsing geometry. Are you only using triangles?" << std::endl;
	}

	GLuint shaderProgramID;
	try {
		shaderProgramID = loadShaders(vertex, fragment);
	}
	catch (const char* error) {
		std::cerr << error << std::endl;
		std::exit(1);
	}

	this->vertexArrayID = vertexArrayID;
	this->vertexBufferID = vertexBufferID;
	this->shaderProgramID = shaderProgramID;
	this->triangles = triangles;
}
