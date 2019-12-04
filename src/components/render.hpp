//
// Created by Alexander Lyon on 2019-12-04.
//

#pragma once

#include <optional>

#include "physics.hpp"

struct material_textures {
    std::optional<GLuint> diffuse;
    std::optional<GLuint> roughness;
    std::optional<GLuint> metallic;
};


class shader {
    GLuint shaderProgramID; // the program to use when rendering this model
public:
    shader(const std::string &vertexShaderPath, const std::string &fragmentShaderPath);

    void use();

    void setMatrix(const std::string &name, glm::mat4 matrix);

    void setFloat(const std::string &name, float f);

    void setInteger(const std::string &name, int i);

    void setVector(const std::string &name, glm::vec3 vector);

    void loadTextures(material_textures textures);

    void close();

    void prepareTextures();
};

/**
 * A model to be rendered by OpenGL
 */
class renderable {
    GLuint vertexBufferID; // the vertex buffer for this model
    GLuint vertexArrayID; // the vertex array for this model
    GLsizei triangles; // the number of triangles
    material_textures textures; // the textures when rendering this model
    shader renderShader;
public:
    /**
    * Creates a renderable from a given obj, vertex shader, and fragment shader.
    * @param model The path to the model to use the renderable with.
    * @param vertex The path to the vertex shader to use.
    * @param fragment The path to the fragment shader to use.
    * @return A renderable.
    */
    renderable(const std::string &model, shader shader);

    void addVertexAttributeMatrix(GLuint index, GLuint bufferID);

    void addVertexAttributeFloat(GLuint index, GLuint bufferID);

    void render(position objPos, position camPos, const glm::mat4 &projectionMatrix, const glm::mat4 &viewMatrix, double time);

    void setTextures();

    void draw(size_t count = 1);

    void close();
};