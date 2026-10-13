#pragma once

namespace sog::gfx
{
class Attachment
{
  public:
    [[nodiscard]]
    static Attachment color(GLuint index, const Texture2D& texture);

    [[nodiscard]]
    static Attachment depth(const Texture2D& texture);

    [[nodiscard]]
    static Attachment depth(const RenderBuffer& renderBuffer);

    [[nodiscard]]
    AttachmentPoint point() const noexcept;

    [[nodiscard]]
    GLuint id() const noexcept;

    [[nodiscard]]
    AttachmentResourceType resourceType() const noexcept;

  private:
    Attachment(AttachmentPoint point, AttachmentResourceType resourceType, GLuint id);

    AttachmentPoint m_Point;
    AttachmentResourceType m_ResourceType;
    GLuint m_Id;
};
} // namespace sog::gfx