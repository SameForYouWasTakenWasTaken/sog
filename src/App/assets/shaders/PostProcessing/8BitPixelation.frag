#version 450 core

layout (location = 0) in vec2 TexCoords;
layout (location = 0) out vec4 FragColor;

uniform sampler2D screenTexture;

void main()
{
    // Adjust pixelation resolution (higher number = bigger pixels)
    float pixelSize = 100.0;
    vec2 uv = floor(TexCoords * pixelSize) / pixelSize;

    vec3 col = texture(screenTexture, uv).rgb;

    // Color quantization (reduces color steps to look like retro hardware)
    float levels = 8.0;
    col = floor(col * levels) / levels;

    FragColor = vec4(col, 1.0);
}