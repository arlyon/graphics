#version 410 core

in vec3 screen;
in vec3 world;
in vec3 normal;
in vec2 texcoord;

out vec4 color;

uniform vec3 cameraPos;
uniform float time;

uniform vec3 lightDir = vec3(0, -1, 0);
uniform int bpm = 125;
uniform float ambientStrength = 0.5;
uniform vec3 fogColor = vec3(20, 23, 55) / 255;
uniform vec3 ambientColor = vec3(0.1, 0.4, 0.7);
uniform vec3 backgroundColor = vec3(0.1, 0.12, 0.33);

#define PI 3.14

// blinn-phong with directional lighting to imitate the sun
void main()
{
    vec3 albedo = vec3(0.2);

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

    color = vec4(mix(light, fog, smoothstep(20.0, 60.0, screen.z)), 1.0);
}