#include "ShadowViewerApp.hpp"

#include <glm/gtx/string_cast.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "gloo/shaders/PhongShader.hpp"
#include "gloo/shaders/SimpleShader.hpp"
#include "gloo/components/RenderingComponent.hpp"
#include "gloo/components/ShadingComponent.hpp"
#include "gloo/components/CameraComponent.hpp"
#include "gloo/components/LightComponent.hpp"
#include "gloo/components/MaterialComponent.hpp"
#include "gloo/MeshLoader.hpp"
#include "gloo/lights/AmbientLight.hpp"
#include "gloo/lights/PointLight.hpp"
#include "gloo/cameras/BasicCameraNode.hpp"
#include "gloo/cameras/ArcBallCameraNode.hpp"
#include "gloo/debug/PrimitiveFactory.hpp"
#include "EulerNode.hpp"
#include "SimpleSystem.hpp"
#include "ParticleState.hpp"
#include "SnowNode.hpp"

#include "SunNode.hpp"

namespace {
void SetAmbientToDiffuse(GLOO::MeshData& mesh_data) {
  // Certain groups do not have an ambient color, so we use their diffuse colors
  // instead.
  for (auto& g : mesh_data.groups) {
    if (glm::length(g.material->GetAmbientColor()) < 1e-3) {
      g.material->SetAmbientColor(g.material->GetDiffuseColor());
      g.material->SetAmbientTexture(g.material->GetDiffuseTexture());
    }
  }
}
}  // namespace

namespace GLOO {
ShadowViewerApp::ShadowViewerApp(const std::string& app_name,
                                 glm::ivec2 window_size)
    : Application(app_name, window_size) {
}

void ShadowViewerApp::SetupScene() {
  SceneNode& root = scene_->GetRootNode();

  // Setting up the camera. PLEASE DO NOT MODIFY THE INITIAL CAMERA TRANSFORM.
  auto camera_node = make_unique<ArcBallCameraNode>(50.0f, 1.0f, 10.0f);
  camera_node->GetTransform().SetPosition(glm::vec3(0.0f, 0.0f, 7.0f));
  //camera_node->GetTransform().SetRotation(glm::vec3(0.0f, 1.0f, 0.0f), kPi / 2);
  camera_node->Calibrate();
  scene_->ActivateCamera(camera_node->GetComponentPtr<CameraComponent>());
  root.AddChild(std::move(camera_node));
  // Add in the ambient light so the shadowed areas won't be completely black.
  auto ambient_light = std::make_shared<AmbientLight>();
  ambient_light->SetAmbientColor(glm::vec3(0.15f));
  auto ambient_node = make_unique<SceneNode>();
  ambient_node->CreateComponent<LightComponent>(ambient_light);
  root.AddChild(std::move(ambient_node));

  // Add in the directional light as the sun.
  auto light_ = std::make_shared<DirectionalLight>();
  light_->SetDiffuseColor(glm::vec3(1.0f, 1.0f, 1.0f));
  light_->SetSpecularColor(glm::vec3(1.0f, 1.0f, 1.0f));
  auto dir_light_node = make_unique<SceneNode>();
  dir_light_node->CreateComponent<LightComponent>(light_);
  root.AddChild(std::move(dir_light_node));

  // Load and set up the scene OBJ.
  MeshData mesh_data =
      MeshLoader::Import("assignment5/sponza_low/final_proj.obj");
  SetAmbientToDiffuse(mesh_data);

  std::shared_ptr<PhongShader> shader = std::make_shared<PhongShader>();
  std::shared_ptr<VertexObject> vertex_obj = std::move(mesh_data.vertex_obj);//hold the geometry data for all the meshes

  // TODO: Create the SceneNodes. All objects in the scene will share a
  // VertexObject. Each object is represented as a "group" within
  // mesh_data.groups. You will need to create a SceneNode for each object and
  // create a ShadingComponent and a RenderingComponent for it before adding the
  // node to the scene tree. You will also need to set the draw range of the
  // RenderingComponent to start at start_face_index of each group, with the
  // number of indices equal to num_indices of the group.

    //part1: simple example
//    std::vector<glm::vec3> positions_simple;
//    std::vector<glm::vec3> velocitys_simple;
//    positions_simple.push_back(glm::vec3(0.1f,0.1f,0.f));
//    velocitys_simple.push_back(glm::vec3(1.f,1.f,0.f));
//    ParticleState inital_state_simple = {positions_simple,velocitys_simple};
//    SimpleSystem inital_system_simple;
//
//    auto euler_node =make_unique<EulerNode>(inital_state_simple,inital_system_simple,IntegratorType::Euler,0.001f);
//
//    euler_node->GetTransform().SetPosition(glm::vec3(1.f,1.f,0.f));
//    root.AddChild(std::move(euler_node));
    int n=80;
    float hard = 10.f;
    float e = 1.0e4;
    float nu = 0.2f;

    auto snow_node = make_unique<SnowNode>();
    snow_node->GetTransform().SetPosition(glm::vec3(-0.3f,-0.5f,1.f));
    root.AddChild(std::move(snow_node));

   /* for (auto group: mesh_data.groups){
        auto group_node = make_unique<SceneNode>();
        group_node->CreateComponent<ShadingComponent>(shader);
        group_node->CreateComponent<MaterialComponent>(group.material);
        group_node->CreateComponent<RenderingComponent>(vertex_obj);
        group_node->GetComponentPtr<RenderingComponent>()->SetDrawRange(group.start_face_index,group.num_indices);
        root.AddChild(std::move(group_node));
    }*/
}

}  // namespace GLOO
