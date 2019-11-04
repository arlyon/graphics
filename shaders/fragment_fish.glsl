#version 410 core

in vec3 normal;
in vec2 texcoord;

out vec4 color;

uniform sampler2D textureImage;

void main()
{
    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    vec3 albedo = texture(textureImage, texcoord).rgb;

    float ambientStrength = 1.0;
    vec3 ambient = ambientStrength * lightColor;

    vec3 result = ambient * albedo;
    color = vec4(result, 1.0);
}