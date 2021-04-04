#ifndef GLOO_SHADOW_SHADER_H_
#define GLOO_SHADOW_SHADER_H_

#include "ShaderProgram.hpp"
#include "gloo/SceneNode.hpp"
#include "gloo/components/RenderingComponent.hpp"

namespace GLOO {
// A simple shader for debug purposes.
    class ShadowShader : public ShaderProgram {
    public:
        ShadowShader()
        : ShaderProgram(std::unordered_map<GLenum, std::string>(
                {{GL_VERTEX_SHADER, "shadow.vert"},
                 {GL_FRAGMENT_SHADER, "shadow.frag"}})) {
        }
        void AssociateVertexArray(VertexArray& vertex_array) const {
            if (!vertex_array.HasPositionBuffer()) {
                throw std::runtime_error("Simple shader requires vertex positions!");
            }
            vertex_array.LinkPositionBuffer(GetAttributeLocation("vertex_position"));
        }
        void SetTargetNode(const SceneNode& node,
                      const glm::mat4& model_matrix) const {
            // Associate the right VAO before rendering.
            AssociateVertexArray(node.GetComponentPtr<RenderingComponent>()
                                         ->GetVertexObjectPtr()
                                         ->GetVertexArray());

            // Set transform.
            SetUniform("model_matrix", model_matrix);
        }
        void SetLightMatrix(const glm::mat4& world_to_light_ndc_matrix)const{
            SetUniform("world_to_light_ndc_matrix",world_to_light_ndc_matrix);
        }
//        void SetSun(const CameraComponent& sun) const {
//            SetUniform("view_matrix", camera.GetViewMatrix());
//            SetUniform("projection_matrix", camera.GetProjectionMatrix());
//        }
    };
}  // namespace GLOO

#endif
