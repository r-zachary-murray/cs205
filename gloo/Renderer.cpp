
#include <cassert>
#include <iostream>
#include <glad/glad.h>
#include <glm/gtx/string_cast.hpp>

#include "Application.hpp"
#include "Scene.hpp"
#include "utils.hpp"
#include "gl_wrapper/BindGuard.hpp"
#include "shaders/ShaderProgram.hpp"
#include "shaders/ShadowShader.hpp"
#include "components/ShadingComponent.hpp"
#include "components/CameraComponent.hpp"
#include "debug/PrimitiveFactory.hpp"
#include "gloo/gl_wrapper/Framebuffer.hpp"

namespace {
const size_t kShadowWidth = 4096;
const size_t kShadowHeight = 4096;
const glm::mat4 kLightProjection =
    glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 1.0f, 80.0f);
}  // namespace

namespace GLOO {
Renderer::Renderer(Application& application) : application_(application) {
  UNUSED(application_);
  // TODO: you may want to initialize your framebuffer and texture(s) here.
  shadow_depth_tex_ = make_unique<Texture>();
    //shadow_depth_tex_.
  shadow_depth_tex_->Reserve(GL_DEPTH_COMPONENT, kShadowWidth, kShadowHeight, GL_DEPTH_COMPONENT, GL_FLOAT);
//  shadow_depth_tex_->Initialize();
   // Framebuffer framebuffe_;
    shadow_framebuffer_ = make_unique<Framebuffer>();
    shadow_framebuffer_->AssociateTexture(*shadow_depth_tex_, GL_DEPTH_ATTACHMENT);
  // To render a quad on in the lower-left of the screen, you can assign texture
  // to quad_ created below and then call quad_->GetVertexArray().Render().
  plain_texture_shader_ = make_unique<PlainTextureShader>();
  //quad_ = PrimitiveFactory::CreateQuad();
}

void Renderer::SetRenderingOptions() const {
  GL_CHECK(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));

  // Enable depth test.
  GL_CHECK(glEnable(GL_DEPTH_TEST));
  GL_CHECK(glDepthFunc(GL_LEQUAL));

  // Enable blending for multi-pass forward rendering.
  GL_CHECK(glEnable(GL_BLEND));
  GL_CHECK(glBlendFunc(GL_ONE, GL_ONE));
}

void Renderer::Render(const Scene& scene) const {
  SetRenderingOptions();
  RenderScene(scene);
  // TODO: When debugging your shadow map, call DebugShadowMap to render a
  // quad at the bottom left corner to display the shadow map.
 // DebugShadowMap();
}

void Renderer::RecursiveRetrieve(const SceneNode& node,
                                 RenderingInfo& info,
                                 const glm::mat4& model_matrix) {
  // model_matrix is parent to world transformation.
  glm::mat4 new_matrix =
      model_matrix * node.GetTransform().GetLocalToParentMatrix();
  auto robj_ptr = node.GetComponentPtr<RenderingComponent>();
  if (robj_ptr != nullptr && node.IsActive())
    info.emplace_back(robj_ptr, new_matrix);

  size_t child_count = node.GetChildrenCount();
  for (size_t i = 0; i < child_count; i++) {
    RecursiveRetrieve(node.GetChild(i), info, new_matrix);
  }
}

Renderer::RenderingInfo Renderer::RetrieveRenderingInfo(
    const Scene& scene) const {
  RenderingInfo info;
  const SceneNode& root = scene.GetRootNode();
  // Efficient implementation without redundant matrix multiplications.
  RecursiveRetrieve(root, info, glm::mat4(1.0f));
  return info;
}

void Renderer::RenderScene(const Scene& scene) const {
  GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

  const SceneNode& root = scene.GetRootNode();
  auto rendering_info = RetrieveRenderingInfo(scene);
  auto light_ptrs = root.GetComponentPtrsInChildren<LightComponent>();
  if (light_ptrs.size() == 0) {
    // Make sure there are at least 2 passes of we don't forget to set color
    // mask back.
    return;
  }

  CameraComponent* camera = scene.GetActiveCameraPtr();

  {
    // Here we first do a depth pass (note that this has nothing to do with the
    // shadow map). The goal of this depth pass is to exclude pixels that are
    // not really visible from the camera, in later rendering passes. You can
    // safely leave this pass here without understanding/modifying it, for
    // assignment 5. If you are interested in learning more, see
    // https://www.khronos.org/opengl/wiki/Early_Fragment_Test#Optimization

    GL_CHECK(glDepthMask(GL_TRUE));
    bool color_mask = GL_FALSE;
    GL_CHECK(glColorMask(color_mask, color_mask, color_mask, color_mask));

    for (const auto& pr : rendering_info) {
      auto robj_ptr = pr.first;
      SceneNode& node = *robj_ptr->GetNodePtr();
      auto shading_ptr = node.GetComponentPtr<ShadingComponent>();
      if (shading_ptr == nullptr) {
        std::cerr << "Some mesh is not attached with a shader during rendering!"
                  << std::endl;
        continue;
      }
      ShaderProgram* shader = shading_ptr->GetShaderPtr();

      BindGuard shader_bg(shader);

      // Set various uniform variables in the shaders.
      shader->SetTargetNode(node, pr.second);
      shader->SetCamera(*camera);

      robj_ptr->Render();
    }
  }

  // The real shadow map/Phong shading passes.
  for (size_t light_id = 0; light_id < light_ptrs.size(); light_id++) {
    // TODO: render the shadow map viewed from the light.
    // This should be rendered to the shadow framebuffer instead of the default
    // one. You should only render shadow if the light can cast shadow (e.g.
    // directional light).
    LightComponent& light1 = *light_ptrs.at(light_id);
    RenderShadow(rendering_info,light1);

    GL_CHECK(glDepthMask(GL_FALSE));
    bool color_mask = GL_TRUE;
    GL_CHECK(glColorMask(color_mask, color_mask, color_mask, color_mask));

    for (const auto& pr : rendering_info) {
      auto robj_ptr = pr.first;
      SceneNode& node = *robj_ptr->GetNodePtr();
      auto shading_ptr = node.GetComponentPtr<ShadingComponent>();
      if (shading_ptr == nullptr) {
        std::cerr << "Some mesh is not attached with a shader during rendering!"
                  << std::endl;
        continue;
      }
      ShaderProgram* shader = shading_ptr->GetShaderPtr();

      BindGuard shader_bg(shader);

      // Set various uniform variables in the shaders.
      shader->SetTargetNode(node, pr.second);
      shader->SetCamera(*camera);

      LightComponent& light = *light_ptrs.at(light_id);
      shader->SetLightSource(light);
      // TODO: pass in the shadow texture to the shader via SetShadowMapping if
      // the light can cast shadow.
      glm::mat4 light_mat = kLightProjection*glm::inverse(light.GetNodePtr()->GetTransform().GetLocalToWorldMatrix());
      shader->SetPCF(application_.argParser.draw_pcf);
      shader->SetShadowMapping(*shadow_depth_tex_,light_mat);

      robj_ptr->Render();
    }
  }

  // Re-enable writing to depth buffer.
  GL_CHECK(glDepthMask(GL_TRUE));
}

void Renderer::RenderShadow( RenderingInfo &rendering_info,LightComponent& light) const
{
    if (light.CanCastShadow()==true) {
        GL_CHECK(glViewport(0, 0, kShadowWidth, kShadowHeight));
        shadow_framebuffer_->Bind();
        //GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, &shadow_framebuffer_));
        //glBindFramebuffer(shadow_framebuffer_,0);
        GL_CHECK(glDepthMask(GL_TRUE));
        GL_CHECK(glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE));
        GL_CHECK(glClear(GL_DEPTH_BUFFER_BIT));

        for (const auto &pr : rendering_info) {
            auto robj_ptr = pr.first;
            SceneNode &node = *robj_ptr->GetNodePtr();
            ShadowShader *shader = new ShadowShader();

            BindGuard shader_bg(shader);
            // Set various uniform variables in the shaders.
            shader->SetTargetNode(node, pr.second);
            glm::mat4 light_mat =
                    kLightProjection * glm::inverse(light.GetNodePtr()->GetTransform().GetLocalToWorldMatrix());

            shader->SetLightMatrix(light_mat);

            shader->SetLightSource(light);

            robj_ptr->Render();
        }
        GL_CHECK(glViewport(0, 0, application_.GetWindowSize()[0], application_.GetWindowSize()[1]));
        //unbind
        shadow_framebuffer_->Unbind();
        //GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    }
}

void Renderer::RenderTexturedQuad(const Texture& texture, bool is_depth) const {
  BindGuard shader_bg(plain_texture_shader_.get());
  plain_texture_shader_->SetVertexObject(*quad_);
  plain_texture_shader_->SetTexture(texture, is_depth);
  quad_->GetVertexArray().Render();
}

void Renderer::DebugShadowMap() const {
  GL_CHECK(glDisable(GL_DEPTH_TEST));
  GL_CHECK(glDisable(GL_BLEND));

  glm::ivec2 window_size = application_.GetWindowSize();
  glViewport(0, 0, window_size.x / 4, window_size.y / 4);
  RenderTexturedQuad(*shadow_depth_tex_, true);

  glViewport(0, 0, window_size.x, window_size.y);
}
}  // namespace GLOO
