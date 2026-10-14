/**
 * @file SetRenderingState.hpp
 * @brief Defines the sog::gfx rendering-state helpers: backend enable/disable/set wrappers and
 * SetRenderingState() for applying a RenderSettings struct.
 */

#pragma once
#include "RenderSettings.hpp"
#include "glad/gl.h"

namespace sog::gfx::backend
{
/**
 * @brief Enables a GL capability.
 * @param e GL capability constant (e.g. GL_DEPTH_TEST).
 */
inline void enable(const GLenum e) { glEnable(e); }

/**
 * @brief Disables a GL capability.
 * @param e GL capability constant (e.g. GL_CULL_FACE).
 */
inline void disable(const GLenum e) { glDisable(e); }

/**
 * @brief Enables or disables a GL capability based on a boolean.
 * @param val true to enable the capability, false to disable it.
 * @param e GL capability constant to toggle.
 */
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
/**
 * @brief Applies a RenderSettings struct to the current GL context's global state.
 *
 * Translates each boolean into the corresponding glEnable/glDisable via
 * backend::set: depth testing (GL_DEPTH_TEST), blending (GL_BLEND) and face
 * culling (GL_CULL_FACE). Call once per frame or on state changes.
 *
 * @param settings Desired state; each field maps to one GL capability.
 */
inline void SetRenderingState(const RenderSettings& settings)
{
    backend::set(settings.depthTest, GL_DEPTH_TEST);
    backend::set(settings.blending, GL_BLEND);
    backend::set(settings.faceCulling, GL_CULL_FACE);
}
} // namespace sog::gfx
