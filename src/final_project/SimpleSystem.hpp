#ifndef SIMPLE_SYSTEM_H_
#define SIMPLE_SYSTEM_H_

#include "ParticleSystemBase.hpp"
namespace GLOO {
    class SimpleSystem: public ParticleSystemBase {
    public:
        ~SimpleSystem() {
        }

        ParticleState ComputeTimeDerivative(const ParticleState& state,
                                                    float time) const override
        {
            auto position= glm::vec3(-state.positions[0][1],state.positions[0][0],0);
            std::vector<glm::vec3> positions;
            std::vector<glm::vec3> velocitys;
            positions.push_back(position);
            velocitys.push_back(glm::vec3(0,0,0));
            ParticleState simple_state = {positions,velocitys};
            return simple_state;
        }
    };
}  // namespace GLOO

#endif
