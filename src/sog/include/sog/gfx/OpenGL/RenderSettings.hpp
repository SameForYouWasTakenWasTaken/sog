/**
 * @file RenderSettings.hpp
 * @brief Defines RenderSettings, a plain struct of boolean toggles for global pipeline state (depth test, face culling, blending).
 */

#pragma once

namespace sog::gfx
{
/**
 * @brief Desired global rendering state, applied via SetRenderingState().
 *
 * Each flag corresponds to a GL capability that is either enabled or disabled as
 * a whole (glEnable/glDisable), so a boolean is the natural representation.
 * Defaults disable all three capabilities.
 *
 * @see sog::SetRenderingState
 */
struct RenderSettings
{
    bool depthTest = false;  /**< Whether depth testing is enabled (GL_DEPTH_TEST). */
    bool faceCulling = false; /**< Whether back/front face culling is enabled (GL_CULL_FACE). */
    bool blending = false;    /**< Whether alpha blending is enabled (GL_BLEND). */
};
} // namespace sog::gfx
