#pragma once
#include "RenderSettings.hpp"
#include "glad/gl.h"

namespace sog::gfx::backend
{
inline void enable(const GLenum e) { glEnable(e); }
inline void disable(const GLenum e) { glDisable(e); }

inline void set(const bool val, const GLenum e)
{
    if (val)
        enable(e);
    else
        disable(e);
}
} // namespace sog::gfx::backend
namespace sog::gfx
{
inline void SetRenderingState(const RenderSettings& settings)
{
    backend::set(settings.depthTest, GL_DEPTH_TEST);
    backend::set(settings.blending, GL_BLEND);
    backend::set(settings.faceCulling, GL_CULL_FACE);
}
} // namespace sog::gfx