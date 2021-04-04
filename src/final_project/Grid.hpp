#ifndef GRID_H_
#define GRID_H_
#include <vector>
#include <math.h>
#include <iostream>
#include <Eigen/Dense>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include "Particle.hpp"
using Vec = Eigen::Vector3f;
using Veci = Eigen::Vector3i;
using Mat = Eigen::Matrix3f;
using Vec4 = Eigen::Vector4f;
class Grid {
public:
	Grid(int grid_res) :grid_res(grid_res),h(1.0f/grid_res),inv_h(1.0f/h),boundary(0.05f){ 
		points= std::vector<Vec4>(pow((grid_res + 1), 3), Vec4::Zero());
	};
	void ScatterParticle(const Particle &p,Mat stresstum);
	void CollectGrid(Particle &p,float dt);
	void ClearGrid();
	void Updategrid(float dt);
private:
	void DetectCollision(int i);
	int PosToIndex(Veci pos) {
		int idx=pos(0) * (grid_res + 1)*(grid_res + 1) + pos(1) * (grid_res + 1) + pos(2);
		if (idx<0 || idx>points.size()) {
			return 0;
		}
		return idx;
	}
	Veci IndexToPos(int index) {
		return Veci(index / ((grid_res + 1)*(grid_res + 1)), (index % ((grid_res + 1)*(grid_res + 1))) / (grid_res + 1), (index % ((grid_res + 1)*(grid_res + 1))) % (grid_res + 1));
	}
	int grid_res;
	float h, inv_h,boundary;
	std::vector<Vec4> points;//velocity+mass

};
#endif 
