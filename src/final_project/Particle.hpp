#ifndef PARTICLE_H_
#define PARTICLE_H_
#include <string>
#include <vector>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat3x3.hpp>
#include <Eigen/Dense>
using Vec = Eigen::Vector3f;
using Mat = Eigen::Matrix3f;
struct Particle {
	Vec x, v;
	Mat F, C;
	float Jp,m;
	int c;
	Particle(Vec x, int c, Vec v = Vec::Zero()) : x(x), v(v), c(c), F(Mat::Identity()), C(Mat::Zero()), Jp(1), m(1.f) {}
};
#endif