#include <iostream>
#include <sstream>
#include <fstream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "components.hpp"

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

bool loadTextures(const tinyobj::ObjReader &reader, std::map<std::string, GLuint> &textures) {
    auto materials = reader.GetMaterials();

    for (auto &material : materials) {
        // Only load the texture if it is not already loaded
        if (material.diffuse_texname.length() == 0) continue;
        if (textures.find(material.diffuse_texname) != textures.end()) continue;

        GLuint texture_id;
        glGenTextures(1, &texture_id);
        glBindTexture(GL_TEXTURE_2D, texture_id);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        int components;
        int image_width, image_height;
        uint8_t *image = stbi_load(material.diffuse_texname.c_str(), &image_width, &image_height, &components, STBI_default);
        if (!image) {
            std::cerr << "Unable to load texture: " << material.diffuse_texname << std::endl;
            exit(1);
        } else {
            std::cout << "Loaded texture " << texture_id << ": " << material.diffuse_texname << std::endl;
        }

        if (components == 3)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        else if (components == 4)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        else {
            std::cerr << "Unable to load texture with " << components << " components: " << material.diffuse_texname << std::endl;
            exit(1);
        }

        glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, 0);
        stbi_image_free(image);
        textures.insert(std::make_pair(material.diffuse_texname, texture_id));
    }

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

	std::map<std::string, GLuint> textures;
	if (!loadTextures(reader, textures)) {
		std::cerr << "Error loading textures for " << model << "." << std::endl;
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

	if (textures.size()) this->textureID = textures.begin()->second; // todo(arlyon) support multiple textures
	this->vertexArrayID = vertexArrayID;
	this->vertexBufferID = vertexBufferID;
	this->shaderProgramID = shaderProgramID;
	this->triangles = triangles;
}

void renderable::render(position pos, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, float time) {
	glUseProgram(this->shaderProgramID);
	glBindVertexArray(this->vertexArrayID);
	glBindTexture(GL_TEXTURE_2D, this->textureID);

	glm::mat4 modelMatrix = glm::mat4(1.0f);
	modelMatrix[3][0] = pos.position.x;
	modelMatrix[3][1] = pos.position.y;
	modelMatrix[3][2] = pos.position.z;
	glm::mat4 mvp = projectionMatrix * viewMatrix * modelMatrix;

    GLuint mvpID = glGetUniformLocation(this->shaderProgramID, "mvp");
    glUniformMatrix4fv(mvpID, 1, GL_FALSE, &mvp[0][0]);

	// todo(arlyon) only set time once
	GLuint timeID = glGetUniformLocation(this->shaderProgramID, "time");
	glUniform1f(timeID, time);

	glDrawArrays(GL_TRIANGLES, 0, this->triangles * 3);
	glBindVertexArray(0);
	glUseProgram(0);
}

void renderable::close() {
	glDeleteBuffers(1, &this->vertexBufferID);
	glDeleteVertexArrays(1, &this->vertexArrayID);
	glDeleteProgram(shaderProgramID);
}
