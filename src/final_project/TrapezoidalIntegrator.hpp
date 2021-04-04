#ifndef TRAPEZOIDAL_INTEGRATOR_H_
#define TRAPEZOIDAL_INTEGRATOR_H_

#include "IntegratorBase.hpp"

namespace GLOO {
    template <class TSystem, class TState>
    class TrapezoidalIntegrator : public IntegratorBase<TSystem, TState> {
    public:
        TState Integrate(const TSystem& system,
                         const TState& state,
                         float start_time,
                         float dt) const override {

            auto f0 = system.ComputeTimeDerivative(state,start_time);
            auto f1 = system.ComputeTimeDerivative(state+dt*f0,start_time+dt);

            return state+dt/float(2.0)*(f0+f1);
        }
    };
}  // namespace GLOO

#endif
