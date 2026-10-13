#version 460 core

layout (location = 0) in vec2 TexCoords;
layout (location = 0) out vec4 FragColor;

uniform sampler2D screenTexture;
uniform float time;

float hash(vec2 p)
{
    p = fract(p * vec2(123.34, 456.21));
    p += dot(p, p + 45.32);
    return fract(p.x * p.y);
}

void main()
{
    vec2 uv = TexCoords;

    float band = floor(uv.y * 35.0);
    float glitch = hash(vec2(band, floor(time * 18.0)));

    if (glitch > 0.82)
    uv.x += (glitch - 0.82) * 0.25;

    float aberration = 0.004 + glitch * 0.009;

    float r = texture(
        screenTexture, uv + vec2(aberration, 0.0)
    ).r;

    float g = texture(screenTexture, uv).g;

    float b = texture(
        screenTexture, uv - vec2(aberration, 0.0)
    ).b;

    vec3 color = vec3(r, g, b);

    float scanline = 0.88 + 0.12 *
    sin(uv.y * 1400.0);

    color *= scanline;

    FragColor = vec4(color, 1.0);
}
