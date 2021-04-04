#ifndef SNOW_NODE_H_
#define SNOW_NODE_H_
#include <string>
#include <vector>

#include "gloo/SceneNode.hpp"
#include "gloo/VertexObject.hpp"
#include "gloo/shaders/ShaderProgram.hpp"
#include "Grid.hpp"
#include "Particle.hpp"
#include <glm/vec4.hpp>

namespace GLOO {
	class SnowNode :public SceneNode {
	public:
		SnowNode(int n = 80, float hardening = 20.f, float E = 2.0e4, float nu = 0.1f);
        ~SnowNode(){};
		void Update(double delta_time) override;
	private:
		void DecomposePlasity(Particle& p);
		void initSnow();
		void PlotSnow();
		void Particle_to_Grid(double delta_time);
		void Grid_to_Particle(double delta_time);
		void add_1kp_cube(glm::vec3 center, int c);
		void add_1kp_sphere(glm::vec3 center, int c, glm::vec3 V);
		std::vector<Particle> particles;
		const int grid_resl;
		Grid grid;
		float mu_0, lambda_0, hardening, Dinv;
	};
}
#endif 

