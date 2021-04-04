#ifndef EULER_NODE_H_
#define EULER_NODE_H_
# include <algorithm>
# include "ParticleState.hpp"
# include "SimpleSystem.hpp"
# include "ForwardEulerIntegrator.hpp"
# include "gloo/SceneNode.hpp"
# include "IntegratorFactory.hpp"

namespace GLOO {
    class EulerNode:public SceneNode {
    public:
        EulerNode(ParticleState state,SimpleSystem system,IntegratorType type,float step){
            state_ = state;
            system_ = system;
            integrator_ = IntegratorFactory::CreateIntegrator<SimpleSystem,ParticleState>(type);
            step_ = step;

            current_time_ = 0;
            auto sphere_node = make_unique<SceneNode>();
            auto shader_ = std::make_shared<PhongShader>();
            sphere_node->CreateComponent<ShadingComponent>(shader_);
            sphere_node->CreateComponent<RenderingComponent>(PrimitiveFactory::CreateSphere(0.15f, 25, 25));
            sphere_node->CreateComponent<MaterialComponent>(std::make_shared<Material>( Material::GetDefault()));
            sphere_node->GetTransform().SetPosition(state_.positions[0]);
            this->AddChild(std::move(sphere_node));
        }
        ~EulerNode() {
        }
        void Update(double delta_time) override {
            int numSteps = int(delta_time/std::min(delta_time,double(step_)));
            for (int i=0;i<numSteps;i++){
                state_ = integrator_->Integrate(system_,state_,current_time_,std::min(delta_time,double(step_)));
                current_time_ += std::min(delta_time,double(step_));
            }
            this->GetChild(0).GetTransform().SetPosition(state_.positions[0]);
        }
    private:
        ParticleState state_;
        SimpleSystem system_;
        std::unique_ptr<IntegratorBase<SimpleSystem,ParticleState>> integrator_;
        double current_time_;
        float step_;
    };
}// namespace GLOO

#endif