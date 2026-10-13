#version 460 core

layout (location = 0) in vec2 TexCoords;
layout (location = 0) out vec4 FragColor;

uniform sampler2D screenTexture;
uniform float time;
void main()
{
    vec2 uv = TexCoords;
    vec2 center = vec2(0.5);

    vec2 p = uv - center;
    float radius = length(p);
    float angle = atan(p.y, p.x);

    // Animated spiral distortion
    angle += 2.5 * exp(-radius * 3.0)
    + sin(time * 2.0) * 0.15;

    float twist = 0.12 * sin(time * 1.5);
    angle += twist / (radius + 0.08);

    vec2 distortedUV = center +
    vec2(cos(angle), sin(angle)) * radius;

    vec3 color = texture(
        screenTexture, distortedUV
    ).rgb;

    // Vignette
    color *= smoothstep(0.75, 0.15, radius);

    FragColor = vec4(color, 1.0);
}
