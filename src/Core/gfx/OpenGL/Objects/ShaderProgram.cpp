#include "ShaderProgram.hpp"

#include "../../../Utility/Uniforms.hpp"

namespace sog::gfx
{

std::optional<Uniform> ShaderProgram::getLocation(const std::string& name)
{
    // find cached
    if (m_CachedUniforms.contains(name))
        return m_CachedUniforms[name];

    auto loc = uniform::getLocation(*this, name);
    // set cache
    if (loc.valid())
    {
        m_CachedUniforms[name] = loc;
        return loc;
    }

    return std::nullopt;
}
} // namespace sog::gfx