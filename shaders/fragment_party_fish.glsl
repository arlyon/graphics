#version 410 core

in vec3 screen;
in vec3 world;
in vec3 normal;
in vec2 texcoord;
in float hueOffset;

out vec4 color;

uniform vec3 cameraPos;
uniform float time;

uniform sampler2D diffuse;
uniform sampler2D metallic;
uniform sampler2D roughness;

uniform vec3 lightDir = vec3(0, -1, 0);
uniform int bpm = 125;
uniform float ambientStrength = 0.5;
uniform vec3 fogColor = vec3(20, 23, 55) / 255;
uniform vec3 ambientColor = vec3(0.1, 0.4, 0.7);
uniform vec3 backgroundColor = vec3(0.1, 0.12, 0.33);

#define PI 3.14

// sourced from https://gist.github.com/mairod/a75e7b44f68110e1576d77419d608786
vec3 hueShift(vec3 color, float hueAdjust){
    const vec3 kRGBToYPrime = vec3(0.299, 0.587, 0.114);
    const vec3 kRGBToI      = vec3(0.596, -0.275, -0.321);
    const vec3 kRGBToQ      = vec3(0.212, -0.523, 0.311);
    const vec3 kYIQToR = vec3(1.0, 0.956, 0.621);
    const vec3 kYIQToG = vec3(1.0, -0.272, -0.647);
    const vec3 kYIQToB = vec3(1.0, -1.107, 1.704);

    float YPrime = dot(color, kRGBToYPrime);
    float I      = dot(color, kRGBToI);
    float Q      = dot(color, kRGBToQ);
    float hue    = atan(Q, I);
    float chroma = sqrt(I * I + Q * Q);

    hue += hueAdjust;
    Q = chroma * sin(hue);
    I = chroma * cos(hue);

    vec3 yIQ = vec3(YPrime, I, Q);
    return vec3(dot(yIQ, kYIQToR), dot(yIQ, kYIQToG), dot(yIQ, kYIQToB));
}

// blinn-phong with directional lighting to imitate the sun
void main()
{
    vec3 albedo = hueShift(texture(diffuse, texcoord).rgb, hueOffset * PI * 2);

    // calculate ambient contribution
    vec3 ambient = ambientStrength * ambientColor * albedo;

    // calculate diffuse contribution
    vec3 lightDir = normalize(lightDir);
    vec3 normal = normalize(normal);
    float diffuseAmount = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diffuseAmount * albedo * 1.2f;

    // calculate specular contribution
    vec3 viewDir = normalize(cameraPos - world);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);
    vec3 specular = vec3(0.5) * spec;

    // calculate light flash
    vec3 light = mix(ambient + diffuse + specular, vec3(0.04, 0.08, 0.15), (sin(time * 2 * PI * (bpm / 60)) + 1.4) / 3);
    // calculate fog
    vec3 fog = mix(fogColor, backgroundColor, smoothstep(40.0, 150.0, screen.z));

    color = vec4(mix(light, fog, smoothstep(20, 60.0, screen.z)), 1.0);
}