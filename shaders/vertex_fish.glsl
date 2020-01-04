#version 410 core

layout (location = 0) in vec3 positionAttribute;
layout (location = 1) in vec3 normalAttribute;
layout (location = 2) in vec2 texcoordAttribute;
layout (location = 3) in mat4 mvpMatrixInstance;
// 4
// 5 in use by the matrix instance
// 6
layout (location = 7) in float timeOffsetInstance;
layout (location = 8) in float hueOffsetInstance;

out vec3 screen;
out vec3 world;
out vec3 normal;
out vec2 texcoord;
out float hueOffset;

uniform float time;

#define PI 3.14

vec3 yaw(vec3 modelSpace) {
    float yaw_a = sin((time + timeOffsetInstance) * PI * 2) * 0.1;
    float yaw_cos = cos(yaw_a);
    float yaw_sin = sin(yaw_a);

    return vec3(
        modelSpace.x * yaw_cos - modelSpace.z * yaw_sin,
        modelSpace.y,
        modelSpace.x * yaw_sin + modelSpace.z * yaw_cos
    );
}

vec3 twist(vec3 modelSpace) {
    float twist_a = smoothstep(-3.0, 8.0, modelSpace.z) * sin(modelSpace.z + (time + timeOffsetInstance) * PI * 2) * 0.8;
    float twist_cos = cos(twist_a);
    float twist_sin = sin(twist_a);

    return vec3(
        modelSpace.x * twist_cos - modelSpace.y * twist_sin,
        modelSpace.x * twist_sin + modelSpace.y * twist_cos,
        modelSpace.z
    );
}

vec3 swim(vec3 modelSpace) {
    float twist_a = smoothstep(-1.5, 5.0, modelSpace.z) * sin(modelSpace.z + (time + timeOffsetInstance) * PI * 2) * 0.6;
    float twist_cos = cos(twist_a);
    float twist_sin = sin(twist_a);

    return vec3(
        modelSpace.x * twist_cos - modelSpace.z * twist_sin,
        modelSpace.y,
        modelSpace.x * twist_sin + modelSpace.z * twist_cos
    );
}

vec3 translate() {
    float loc = sin((time + timeOffsetInstance) * (3 * PI) / 2) * 0.3;
    return vec3(pow(abs(loc), 0.77) / 6 * sign(loc), 0, 0);
}

void main()
{
    // apply model space transformations
    vec3 modelSpace = positionAttribute;
    modelSpace = swim(modelSpace);
    modelSpace = twist(modelSpace);
    modelSpace = yaw(modelSpace);
    modelSpace += translate();

    // set vertex position
    gl_Position = mvpMatrixInstance * vec4(modelSpace, 1.0);

    // export normals and texture coordinates
    screen = gl_Position.xyz;
    world = positionAttribute;
    normal = normalAttribute;
    texcoord = texcoordAttribute;
    hueOffset = hueOffsetInstance;
}