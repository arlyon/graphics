#version 410 core

layout (location = 0) in vec3 positionAttribute;
layout (location = 1) in vec3 normalAttribute;
layout (location = 2) in vec2 texcoordAttribute;

out vec3 position;
out vec3 normal;
out vec2 texcoord;

uniform mat4 mvp;
uniform float time;
uniform float timeOffset;

#define PI 3.14

// Check https://thebookofshaders.com/05/ for more info
// about how smoothstep works...
float headMask(vec3 worldSpace, float start, float stop) {
    return smoothstep(start, stop, worldSpace.z);
}

vec3 yaw(vec3 worldSpace) {
    float yaw_a = sin((time + timeOffset) * PI * 2) * 0.2;
    float yaw_cos = cos(yaw_a);
    float yaw_sin = sin(yaw_a);

    return vec3(
        worldSpace.x * yaw_cos - worldSpace.z * yaw_sin,
        worldSpace.y,
        worldSpace.x * yaw_sin + worldSpace.z * yaw_cos
    );
}

vec3 twist(vec3 worldSpace) {
    float twist_a = headMask(worldSpace, -3, 8) * sin(worldSpace.z + (time + timeOffset) * PI * 2) * 0.8;
    float twist_cos = cos(twist_a);
    float twist_sin = sin(twist_a);

    return vec3(
        worldSpace.x * twist_cos - worldSpace.y * twist_sin,
        worldSpace.x * twist_sin + worldSpace.y * twist_cos,
        worldSpace.z
    );
}

vec3 swim(vec3 worldSpace) {
    float twist_a = headMask(worldSpace, -1.5, 5) * sin(worldSpace.z + (time + timeOffset) * PI * 2) * 0.6;
    float twist_cos = cos(twist_a);
    float twist_sin = sin(twist_a);

    return vec3(
        worldSpace.x * twist_cos - worldSpace.z * twist_sin,
        worldSpace.y,
        worldSpace.x * twist_sin + worldSpace.z * twist_cos
    );
}

vec3 translate(vec3 worldSpace) {
    float loc = sin((time + timeOffset) * (3 * PI) / 2) * 0.3;
    return vec3(pow(abs(loc), 0.77) / 6 * sign(loc), 0, 0);
}

void main()
{
    vec3 worldSpace = positionAttribute;
    worldSpace = swim(worldSpace);
    worldSpace = twist(worldSpace);
    worldSpace = yaw(worldSpace);
    worldSpace += translate(positionAttribute);

    // export normals and texture coordinates
    position = worldSpace;
    normal = normalAttribute;
    texcoord = texcoordAttribute;

    gl_Position = mvp * vec4(worldSpace, 1.0);
}