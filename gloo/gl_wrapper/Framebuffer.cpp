#include "Framebuffer.hpp"

#include <stdexcept>

#include "BindGuard.hpp"
#include "gloo/utils.hpp"

namespace GLOO {
Framebuffer::Framebuffer() {
  GL_CHECK(glGenFramebuffers(1, &handle_));//创建帧缓冲对象
}

Framebuffer::~Framebuffer() {
  if (handle_ != GLuint(-1))
    GL_CHECK(glDeleteFramebuffers(1, &handle_));
}

Framebuffer::Framebuffer(Framebuffer&& other) noexcept {
  handle_ = other.handle_;
  other.handle_ = GLuint(-1);
}

Framebuffer& Framebuffer::operator=(Framebuffer&& other) noexcept {
  handle_ = other.handle_;
  other.handle_ = GLuint(-1);
  return *this;
}

void Framebuffer::Bind() const {
  GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, handle_));
}

void Framebuffer::Unbind() const {
  GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));//为了让所有的渲染操作对主窗口产生影响我们必须通过绑定为0来使默认帧缓冲被激活
}

void Framebuffer::AssociateTexture(const Texture& texture, GLenum attachment) {
  Bind();//把帧缓冲对象绑定到当前帧缓冲 GL_FRAMEBUFFER
  // TODO: call glFramebufferTexture2D with correct arguments.
  // Make sure you use GL_CHECK to detect potential errors.
  GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture.GetHandle(), 0));
  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (status != GL_FRAMEBUFFER_COMPLETE) {
    throw std::runtime_error("Incomplete framebuffer!");
  }
  Unbind();
}

static_assert(std::is_move_constructible<Framebuffer>(), "");
static_assert(std::is_move_assignable<Framebuffer>(), "");

static_assert(!std::is_copy_constructible<Framebuffer>(), "");
static_assert(!std::is_copy_assignable<Framebuffer>(), "");
}  // namespace GLOO
