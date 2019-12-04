#version 410 core

layout (location = 0) in vec3 positionAttribute;
layout (location = 1) in vec3 normalAttribute;
layout (location = 2) in vec2 texcoordAttribute;

out vec3 screen;
out vec3 world;
out vec3 normal;
out vec2 texcoord;

uniform mat4 mvp;
uniform float time;
uniform float timeOffset;

#define PI 3.14

// Check https://thebookofshaders.com/05/ for more info
// about how smoothstep works...
float headMask(vec3 modelSpace, float start, float stop) {
    return smoothstep(start, stop, modelSpace.z);
}

vec3 yaw(vec3 modelSpace) {
    float yaw_a = sin((time + timeOffset) * PI * 2) * 0.1;
    float yaw_cos = cos(yaw_a);
    float yaw_sin = sin(yaw_a);

    return vec3(
        modelSpace.x * yaw_cos - modelSpace.z * yaw_sin,
        modelSpace.y,
        modelSpace.x * yaw_sin + modelSpace.z * yaw_cos
    );
}

vec3 twist(vec3 modelSpace) {
    float twist_a = headMask(modelSpace, -3, 8) * sin(modelSpace.z + (time + timeOffset) * PI * 2) * 0.8;
    float twist_cos = cos(twist_a);
    float twist_sin = sin(twist_a);

    return vec3(
        modelSpace.x * twist_cos - modelSpace.y * twist_sin,
        modelSpace.x * twist_sin + modelSpace.y * twist_cos,
        modelSpace.z
    );
}

vec3 swim(vec3 modelSpace) {
    float twist_a = headMask(modelSpace, -1.5, 5) * sin(modelSpace.z + (time + timeOffset) * PI * 2) * 0.6;
    float twist_cos = cos(twist_a);
    float twist_sin = sin(twist_a);

    return vec3(
        modelSpace.x * twist_cos - modelSpace.z * twist_sin,
        modelSpace.y,
        modelSpace.x * twist_sin + modelSpace.z * twist_cos
    );
}

vec3 translate() {
    float loc = sin((time + timeOffset) * (3 * PI) / 2) * 0.3;
    return vec3(pow(abs(loc), 0.77) / 6 * sign(loc), 0, 0);
}

void main()
{
    vec3 modelSpace = positionAttribute;
    modelSpace = swim(modelSpace);
    modelSpace = twist(modelSpace);
    modelSpace = yaw(modelSpace);
    modelSpace += translate();

    gl_Position = mvp * vec4(modelSpace, 1.0);

    // export normals and texture coordinates
    screen = gl_Position.xyz;
    world = positionAttribute;
    normal = normalAttribute;
    texcoord = texcoordAttribute;
}