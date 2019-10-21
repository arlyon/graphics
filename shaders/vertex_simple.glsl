#version 410 core

in vec3 aPos;

uniform mat4 MVP;

void main()
{
    gl_Position = MVP * vec4(aPos, 1.0);
}