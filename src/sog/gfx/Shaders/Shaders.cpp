#include "sog/gfx/Shaders/Shaders.hpp"

#include "sog/gfx/OpenGL/Objects/ShaderProgram.hpp"

namespace sog::gfx::shaders
{
namespace
{
/**
 * @brief Compiles and links a vertex and fragment shader pair.
 *
 * @param vertex GLSL vertex shader source.
 * @param fragment GLSL fragment shader source.
 * @return The resulting shader program.
 */
ShaderProgram createFromSources(const char* vertex, const char* fragment)
{
    VertexShader vertexShader;
    FragmentShader fragmentShader;

    vertexShader.createFromSource(vertex);
    fragmentShader.createFromSource(fragment);

    return ShaderProgram{vertexShader, fragmentShader};
}

constexpr const char* COMMON_FRAGMENT = R"(
    #version 460 core

    in vec4 vColor;

    out vec4 FragColor;

    void main()
    {
        FragColor = vColor;
    }
)";
} // namespace

ShaderProgram getCommonShader()
{
    constexpr const char* vertexSource = R"(
        #version 460 core

        layout (location = 0) in vec3 aPosition;
        layout (location = 1) in vec4 aColor;
        layout (location = 2) in vec2 aTexCoord;

        out vec4 vColor;

        void main()
        {
            gl_Position = vec4(aPosition, 1.0);
            vColor = aColor;
        }
    )";

    return createFromSources(vertexSource, COMMON_FRAGMENT);
}

ShaderProgram getMVPShader()
{
    constexpr const char* vertexSource = R"(
        #version 460 core

        layout (location = 0) in vec3 aPosition;
        layout (location = 1) in vec4 aColor;
        layout (location = 2) in vec2 aTexCoord;

        uniform mat4 uMVP;

        out vec4 vColor;

        void main()
        {
            gl_Position = uMVP * vec4(aPosition, 1.0);
            vColor = aColor;
        }
    )";

    return createFromSources(vertexSource, COMMON_FRAGMENT);
}

ShaderProgram getSolidColorShader()
{
    constexpr const char* vertexSource = R"(
        #version 460 core

        layout (location = 0) in vec3 aPosition;

        uniform mat4 uMVP;

        void main()
        {
            gl_Position = uMVP * vec4(aPosition, 1.0);
        }
    )";

    constexpr const char* fragmentSource = R"(
        #version 460 core

        uniform vec4 uColor;

        out vec4 FragColor;

        void main()
        {
            FragColor = uColor;
        }
    )";

    return createFromSources(vertexSource, fragmentSource);
}

ShaderProgram getTextureShader()
{
    constexpr const char* vertexSource = R"(
        #version 460 core

        layout (location = 0) in vec3 aPosition;
        layout (location = 1) in vec4 aColor;
        layout (location = 2) in vec2 aTexCoord;

        uniform mat4 uMVP;

        out vec4 vColor;
        out vec2 vTexCoord;

        void main()
        {
            gl_Position = uMVP * vec4(aPosition, 1.0);
            vColor = aColor;
            vTexCoord = aTexCoord;
        }
    )";

    constexpr const char* fragmentSource = R"(
        #version 460 core

        in vec4 vColor;
        in vec2 vTexCoord;

        uniform sampler2D uTexture;

        out vec4 FragColor;

        void main()
        {
            FragColor = texture(uTexture, vTexCoord) * vColor;
        }
    )";

    return createFromSources(vertexSource, fragmentSource);
}

ShaderProgram getInstancedShader()
{
    constexpr const char* vertexSource = R"(
        #version 460 core

        layout (location = 0) in vec3 aPosition;
        layout (location = 1) in vec4 aInstanceColor;
        layout (location = 2) in mat4 aInstanceTransform;

        uniform mat4 uViewProjection;

        out vec4 vColor;

        void main()
        {
            gl_Position = uViewProjection *
                          aInstanceTransform *
                          vec4(aPosition, 1.0);

            vColor = aInstanceColor;
        }
    )";

    return createFromSources(vertexSource, COMMON_FRAGMENT);
}

ShaderProgram getNormalVisualizationShader()
{
    constexpr const char* vertexSource = R"(
        #version 460 core

        layout (location = 0) in vec3 aPosition;
        layout (location = 1) in vec3 aNormal;

        uniform mat4 uMVP;
        uniform mat3 uNormalMatrix;

        out vec3 vNormal;

        void main()
        {
            gl_Position = uMVP * vec4(aPosition, 1.0);
            vNormal = normalize(uNormalMatrix * aNormal);
        }
    )";

    constexpr const char* fragmentSource = R"(
        #version 460 core

        in vec3 vNormal;

        out vec4 FragColor;

        void main()
        {
            FragColor = vec4(normalize(vNormal) * 0.5 + 0.5, 1.0);
        }
    )";

    return createFromSources(vertexSource, fragmentSource);
}

ShaderProgram getDepthShader()
{
    constexpr const char* vertexSource = R"(
        #version 460 core

        layout (location = 0) in vec3 aPosition;

        uniform mat4 uMVP;

        void main()
        {
            gl_Position = uMVP * vec4(aPosition, 1.0);
        }
    )";

    constexpr const char* fragmentSource = R"(
        #version 460 core

        void main()
        {
        }
    )";

    return createFromSources(vertexSource, fragmentSource);
}

ShaderProgram getScreenSpaceShader()
{
    constexpr const char* vertexSource = R"(
        #version 460 core

        layout (location = 0) in vec2 aPosition;
        layout (location = 1) in vec2 aTexCoord;

        out vec2 vTexCoord;

        void main()
        {
            gl_Position = vec4(aPosition, 0.0, 1.0);
            vTexCoord = aTexCoord;
        }
    )";

    constexpr const char* fragmentSource = R"(
        #version 460 core

        in vec2 vTexCoord;

        uniform sampler2D uTexture;

        out vec4 FragColor;

        void main()
        {
            FragColor = texture(uTexture, vTexCoord);
        }
    )";

    return createFromSources(vertexSource, fragmentSource);
}

} // namespace sog::gfx::shaders