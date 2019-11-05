#version 410 core

layout (location = 0) in vec3 positionAttribute;
layout (location = 1) in vec3 normalAttribute;
layout (location = 2) in vec2 texcoordAttribute;

out vec3 normal;
out vec2 texcoord;

uniform mat4 MVP;
uniform float time;

#define PI 3.14

void main()
{
    vec4 worldSpace = vec4(positionAttribute, 1.0);

    // wiggle tail
    float offset = smoothstep(-1., 4., worldSpace.z) * sin(time * PI * 2);
    worldSpace.x += offset;
    worldSpace.z -= pow(offset, 2);

    // move side to side
    float loc = sin((time + 0.5) * (2*PI/3));
    worldSpace.x += pow(loc, 0.77) / 4 * sign(loc);

    // set position
    gl_Position = MVP * worldSpace;

    // export normals and texture coordinates
    normal = normalAttribute;
    texcoord = texcoordAttribute;
}