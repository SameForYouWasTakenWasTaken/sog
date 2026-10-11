#version 460 core
out vec4 FragColor;

in vec4 vertexColor;
in vec2 TexCoord;

uniform sampler2D texture2D;

void main()
{
    FragColor = texture(texture2D, TexCoord);
}