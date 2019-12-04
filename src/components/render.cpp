#include <iostream>
#include <sstream>
#include <fstream>
#include <random>
#include <string>

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "components.hpp"
#include "render.hpp"

/**
 * Compiles the provided shader.
 *
 * @param shaderPath The path of the shader.
 * @param shaderType The type of shader.
 * @returns A unique ID for the compiled shader.
 */
GLuint compileShader(const std::string &shaderPath, const GLuint shaderType) {
    GLuint shaderID = glCreateShader(shaderType);

    std::string shaderCode;
    std::ifstream shaderCodeStream(shaderPath, std::ios::in);
    if (shaderCodeStream.is_open()) {
        std::stringstream stringStream;
        stringStream << shaderCodeStream.rdbuf();
        shaderCode = stringStream.str();
        shaderCodeStream.close();
    } else {
        throw "Couldn't read shader.";
    }

    char const *sourcePointer = shaderCode.c_str();
    glShaderSource(shaderID, 1, &sourcePointer, nullptr);
    glCompileShader(shaderID);

    GLint isCompiled = 0;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
        GLint maxLength = 0;
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &maxLength);

        std::cout << "Errors when compiling " << shaderPath << "." << std::endl;
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

    for (auto shaderID : {vertexShaderID, fragmentShaderID}) {
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

    for (auto shaderID : {vertexShaderID, fragmentShaderID}) {
        glDetachShader(programID, shaderID);
        glDeleteShader(shaderID);
    }

    return programID;
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
loadGeometry(const tinyobj::ObjReader &reader, GLuint &vertexBufferID, GLuint &vertexArrayID, GLsizei &triangles) {
    // only support triangles
    const uint8_t faceSides = 3;

    triangles = 0;
    for (const auto &shape : reader.GetShapes()) {
        triangles += (GLsizei) shape.mesh.num_face_vertices.size();
        for (auto vert : shape.mesh.num_face_vertices) {
            if (vert != faceSides) return false;
        }
    }

    std::vector<tinyobj::real_t> vec;
    auto vertices = reader.GetAttrib().vertices;
    auto normals = reader.GetAttrib().normals;
    auto texcoords = reader.GetAttrib().texcoords;
    for (const auto &shape : reader.GetShapes()) {
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
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (3 + 3 + 2) * sizeof(float), (void *) (0 * 3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, (3 + 3 + 2) * sizeof(float), (void *) (1 * 3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, (3 + 3 + 2) * sizeof(float), (void *) (2 * 3 * sizeof(float)));

    return true;
}

GLuint loadTexture(const std::string &file) {
    GLuint texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int components;
    int image_width, image_height;
    uint8_t *image = stbi_load(file.c_str(), &image_width, &image_height, &components, STBI_default);
    if (!image) {
        std::cerr << "Unable to load texture: " << file << std::endl;
        exit(1);
    } else {
        std::cout << "Loaded texture " << texture_id << ": " << file << std::endl;
    }

    if (components == 3)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    else if (components == 4)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    else {
        std::cerr << "Unable to load texture with " << components << " components: " << file << std::endl;
        exit(1);
    }

    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(image);
    return texture_id;
}

/**
 * Loads textures from a tinyobj reader, storing their texture name and ID in a map.
 *
 * todo(arlyon) only supports one material (the first)
 * todo(arlyon) only supports loading some well-defined textures
 */
bool loadTextures(const tinyobj::ObjReader &reader, material_textures &textures) {
    auto material = reader.GetMaterials()[0];

    if (material.diffuse_texname.empty()) textures.diffuse = {};
    else textures.diffuse = {loadTexture(material.diffuse_texname)};
    if (material.roughness_texname.empty()) textures.roughness = {};
    else textures.roughness = {loadTexture(material.roughness_texname)};
    if (material.metallic_texname.empty()) textures.metallic = {};
    else textures.metallic = {loadTexture(material.metallic_texname)};

    return true;
}

renderable::renderable(const std::string &model, shader renderShader) : renderShader(renderShader) {
    auto reader = tinyobj::ObjReader{};
    if (!reader.ParseFromFile(model)) {
        std::cerr << "Couldn't load file " << model << "." << std::endl;
        std::exit(1);
    }

    if (!loadGeometry(reader, this->vertexBufferID, this->vertexArrayID, this->triangles)) {
        std::cerr << "Error parsing geometry for " << model << ". Are you only using triangles?" << std::endl;
        std::exit(1);
    }

    material_textures textures = {};
    if (!loadTextures(reader, textures)) {
        std::cerr << "Error loading textures for " << model << "." << std::endl;
        std::exit(1);
    }
}

void
renderable::render(position objPos, position camPos, const glm::mat4 &projectionMatrix, const glm::mat4 &viewMatrix,
                   double time) {
    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), objPos.position) * glm::mat4_cast(objPos.orientation);
    glm::mat4 mvp = projectionMatrix * viewMatrix * modelMatrix;

    this->renderShader.use();
    this->renderShader.setFloat("time", (float) time);
    this->renderShader.setMatrix("mvp", mvp);
    this->renderShader.setVector("cameraPos", camPos.position);
    this->renderShader.prepareTextures();
    this->setTextures();
    this->draw();

    glUseProgram(0);
}

void renderable::draw(size_t count) {
    glBindVertexArray(this->vertexArrayID);
    if (count == 1) {
        glDrawArrays(GL_TRIANGLES, 0, this->triangles * 3);
    } else if (count > 1) {
        glDrawArraysInstanced(GL_TRIANGLES, 0, this->triangles * 3, count);
    }
    glBindVertexArray(0);
}

void renderable::close() {
    glDeleteBuffers(1, &this->vertexBufferID);
    glDeleteVertexArrays(1, &this->vertexArrayID);
    this->renderShader.close();
}

void renderable::setTextures() {
    if (textures.diffuse) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, *textures.diffuse);
    }

    if (textures.roughness) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, *textures.roughness);
    }

    if (textures.metallic) {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, *textures.metallic);
    }
}

void renderable::addVertexAttributeMatrix(GLuint index, GLuint bufferID) {
    glBindVertexArray(this->vertexArrayID);
    glBindBuffer(GL_ARRAY_BUFFER, bufferID);
    size_t vec4Size = sizeof(glm::vec4);
    for (uint8_t x : {0, 1, 2, 3}) {
        glEnableVertexAttribArray(index + x);
        glVertexAttribPointer(index + x, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *) (x * vec4Size));
        glVertexAttribDivisor(index + x, 1);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void renderable::addVertexAttributeFloat(GLuint index, GLuint bufferID) {
    glBindVertexArray(this->vertexArrayID);
    glBindBuffer(GL_ARRAY_BUFFER, bufferID);
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, 1, GL_FLOAT, GL_FALSE, 0, nullptr);
    glVertexAttribDivisor(index, 1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

shader::shader(const std::string &vertexShaderPath, const std::string &fragmentShaderPath) {
    try {
        GLuint vertexShaderID = compileShader(vertexShaderPath, GL_VERTEX_SHADER);
        GLuint fragmentShaderID = compileShader(fragmentShaderPath, GL_FRAGMENT_SHADER);
        this->shaderProgramID = linkProgram(vertexShaderID, fragmentShaderID);
    }
    catch (const char *error) {
        std::cerr << error << std::endl;
        std::exit(1);
    }
}

void shader::use() {
    glUseProgram(this->shaderProgramID);
}

void shader::setMatrix(const std::string &name, glm::mat4 matrix) {
    glUniformMatrix4fv(glGetUniformLocation(this->shaderProgramID, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
}

void shader::setInteger(const std::string &name, int i) {
    glUniform1i(glGetUniformLocation(this->shaderProgramID, name.c_str()), i);
}

void shader::setFloat(const std::string &name, float f) {
    glUniform1f(glGetUniformLocation(this->shaderProgramID, name.c_str()), f);
}

void shader::setVector(const std::string &name, glm::vec3 vector) {
    glUniform3fv(glGetUniformLocation(this->shaderProgramID, name.c_str()), 1, glm::value_ptr(vector));
}

void shader::prepareTextures() {
    setInteger("diffuse", 0);
    setInteger("roughness", 1);
    setInteger("metallic", 2);
}

void shader::close() {
    glDeleteProgram(shaderProgramID);
}
