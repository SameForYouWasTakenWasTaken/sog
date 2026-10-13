#version 460 core

layout (location = 0) in vec2 TexCoords;
layout (location = 0) out vec4 FragColor;

uniform sampler2D screenTexture;
uniform float time;
uniform float intensity;

void main()
{
    vec2 uv = TexCoords;
    vec2 p = uv - 0.5;

    float r = length(p);
    float a = atan(p.y, p.x);

    float wave1 = sin(r * 35.0 - time * 5.0 + a * 4.0);
    float wave2 = cos(a * 9.0 + r * 18.0 + time * 3.0);
    float wave3 = sin(p.x * 24.0 + p.y * 19.0 + time * 2.0);

    float displacement =
    (wave1 * 0.55 + wave2 * 0.3 + wave3 * 0.15)
    * 0.035 * intensity;

    vec2 dir = p / max(r, 0.0001);
    uv += dir * displacement;

    // Subtle chromatic separation
    vec2 aberration = dir * abs(displacement) * 0.4;
    
    vec3 color;
    color.r = texture(screenTexture, uv + aberration).r;
    color.g = texture(screenTexture, uv).g;
    color.b = texture(screenTexture, uv - aberration).b;

    FragColor = vec4(color, 1.0);
}
