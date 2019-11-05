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
 * Selects the buffer and loads the vertices into it.
 *
 * @param vertexBufferID
 * @param vertexArrayID
 * @param triangles A pointer to place the number of triangles into.
 */
bool
loadGeometry(const tinyobj::ObjReader& reader, GLuint& vertexBufferID, GLuint& vertexArrayID, GLuint& triangles) {
	// only support triangles
	const uint8_t faceSides = 3;

	triangles = 0;
	for (const auto& shape : reader.GetShapes()) {
		triangles += shape.mesh.num_face_vertices.size();
		for (auto vert : shape.mesh.num_face_vertices) {
			if (vert != faceSides) return false;
		}
	}

	std::vector<tinyobj::real_t> vec;
	auto vertices = reader.GetAttrib().vertices;
	auto normals = reader.GetAttrib().normals;
	auto texcoords = reader.GetAttrib().texcoords;
	for (const auto& shape : reader.GetShapes()) {
		for (auto index : shape.mesh.indices) {
			for (int i = 0; i < faceSides; i++) vec.push_back(vertices[index.vertex_index * 3 + i]);
			for (int i = 0; i < faceSides; i++) vec.push_back(normals[index.normal_index * 3 + i]);

			//texture coordinates with inverted Y
			vec.push_back(texcoords[index.texcoord_index * 2]);
			vec.push_back(1.0f - texcoords[index.texcoord_index * 2 + 1]);
		}
	}

	glGenBuffers(1, &vertexBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tinyobj::real_t) * vec.size(), &vec.at(0), GL_STATIC_DRAW);

	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (2 * faceSides + 2) * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, (2 * faceSides + 2) * sizeof(float), (void*)(faceSides * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, (2 * faceSides + 2) * sizeof(float), (void*)(2 * faceSides * sizeof(float)));
	glEnableVertexAttribArray(2);

	return true;
}

renderable::renderable(const std::string& model, const std::string& vertex, const std::string& fragment) {
	auto reader = tinyobj::ObjReader{};
	if (!reader.ParseFromFile(model)) {
		std::cerr << "Couldn't load file " << model << "." << std::endl;
		std::exit(1);
	}

	GLuint vertexBufferID, vertexArrayID, triangles = 0;
	if (!loadGeometry(reader, vertexBufferID, vertexArrayID, triangles)) {
		std::cerr << "Error parsing geometry for " << model << ". Are you only using triangles?" << std::endl;
		std::exit(1);
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

void renderable::render(position pos, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix) {
	glUseProgram(this->shaderProgramID);
	glBindVertexArray(this->vertexArrayID);

	glm::mat4 modelMatrix = glm::mat4(1.0f);
	modelMatrix[3][0] = pos.position.x;
	modelMatrix[3][1] = pos.position.y;
	modelMatrix[3][2] = pos.position.z;
	glm::mat4 mvp = projectionMatrix * viewMatrix * modelMatrix;
	GLuint mvpID = glGetUniformLocation(this->shaderProgramID, "MVP");
	glUniformMatrix4fv(mvpID, 1, GL_FALSE, &mvp[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, this->triangles * 3);

	glBindVertexArray(0);
	glUseProgram(0);
}

void renderable::close() {
	glDeleteBuffers(1, &this->vertexBufferID);
	glDeleteVertexArrays(1, &this->vertexArrayID);
	glDeleteProgram(shaderProgramID);
}