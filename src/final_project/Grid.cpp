#include "Grid.hpp"
#include <math.h>
#include <iostream>
#include <algorithm>
using Vec = Eigen::Vector3f;
using Veci = Eigen::Vector3i;
using Mat = Eigen::Matrix3f;
using Vec4 = Eigen::Vector4f;
void Grid::ClearGrid() {
	for (size_t i = 0; i < points.size(); i++) {
		points[i] = Vec4::Zero();
	}
}

void Grid::ScatterParticle(const Particle &p, Mat stresstum) {//with Affine
	Veci floor_pos= (p.x * inv_h - Vec(0.5, 0.5, 0.5)).cast<int>();
	
	Vec frac_pos = p.x*inv_h - (floor_pos).cast<float>();
	Vec w[3] = {
	0.5*(1.5*Vec(1,1,1) - frac_pos).cwiseProduct(1.5*Vec(1,1,1) - frac_pos),
	0.75*Vec(1,1,1) - (Vec(1,1,1) - frac_pos).cwiseProduct(Vec(1,1,1) - frac_pos),
	0.5f*(0.5*Vec(1,1,1) - frac_pos).cwiseProduct(0.5*Vec(1,1,1) - frac_pos)
	};
	for (int i = 0; i < 3; i++) 
		for (int j = 0; j < 3; j++) 
			for(int k = 0; k < 3; k++)	{
			Vec dpos = (Vec(i,j,k) - frac_pos)*h ;
			// Translational momentum
			Vec4 mass_x_velocity(p.v(0) * p.m, p.v(1) * p.m, p.v(2) * p.m, p.m);
			Veci pos(floor_pos(0) + i, floor_pos(1) + j, floor_pos(2) + k);
			int idx = PosToIndex(pos);
			Vec Affine = p.m*(p.C*dpos)+ stresstum*dpos;
			auto weight = w[i](0)*w[j](1)*w[k](2);
			points[idx] += (
				weight * (
					mass_x_velocity+ Vec4(Affine(0), Affine(1), Affine(2),0)
					)
				);
			
		}
}

void Grid::CollectGrid(Particle &p,float dt) {
	Veci floor_pos= (p.x * inv_h - Vec(0.5, 0.5, 0.5)).cast<int>();
	Vec frac_pos = p.x*inv_h - (floor_pos).cast<float>();
	Vec w[3] = {
	0.5*(1.5*Vec(1,1,1) - frac_pos).cwiseProduct(1.5*Vec(1,1,1) - frac_pos),
	0.75*Vec(1,1,1) - (Vec(1,1,1) - frac_pos).cwiseProduct(Vec(1,1,1) - frac_pos),
	0.5f*(0.5*Vec(1,1,1) - frac_pos).cwiseProduct(0.5*Vec(1,1,1) - frac_pos)
	};
	p.C = Mat::Zero();
	p.v = Vec::Zero();
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			for (int k = 0; k < 3; k++) {
				Vec dpos = (Vec(i, j, k) - frac_pos);
				Veci pos(floor_pos(0) + i, floor_pos(1) + j, floor_pos(2) + k);
				int idx = PosToIndex(pos);
				float weight = w[i](0)*w[j](1)*w[k](2);
				Vec point_v = Vec(points[idx](0), points[idx](1), points[idx](2));
				p.v += weight*point_v;
				p.C += 4 * inv_h*weight*point_v* dpos.transpose();
			}
	p.x += dt * p.v;
	
	p.F = (Mat::Identity() + dt * p.C) * p.F;
	p.Jp *= p.F.determinant();
}

void Grid::Updategrid(float dt) {
	
	for (size_t i = 0; i < points.size(); i++) {
		if (points[i](3) > 0) {
			points[i] /= points[i](3);
			points[i](1) -= dt *200;
			DetectCollision(i);
		}
	}
}

void Grid::DetectCollision(int i) {
	Veci pos = IndexToPos(i);
	float x = (float)(pos(0)) / grid_res, y = (float)(pos(1)) / grid_res, z=(float)(pos(2)) / grid_res;
	if (x < boundary || x>(1 - boundary)|| y>1 - boundary || z<boundary || z>(1 - boundary)) {
		points[i] = Vec4::Zero();
	}
	if (y < boundary) {
		points[i] = Vec4::Zero();
		//points[i][1] = std::max(points[i](1), 0.f);
	}
}