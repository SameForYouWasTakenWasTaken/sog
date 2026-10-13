#version 460 core

layout (location = 0) out vec2 TexCoords;

void main()
{
    // Generates a full-screen triangle using vertex IDs (0, 1, 2)
    TexCoords = vec2((gl_VertexID << 1) & 2, gl_VertexID & 2);
    gl_Position = vec4(TexCoords * 2.0 - 1.0, 0.0, 1.0);
}