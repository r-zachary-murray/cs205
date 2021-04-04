#include "SnowNode.hpp"
#include "gloo/shaders/SimpleShader.hpp"
#include "gloo/debug/PrimitiveFactory.hpp"
#include "gloo/components/RenderingComponent.hpp"
#include "gloo/components/ShadingComponent.hpp"
#include "gloo/components/MaterialComponent.hpp"
#include "gloo/shaders/PhongShader.hpp"
#include <Eigen/SVD>  
#include <Eigen/Dense>  
#include "iostream"
#include "ctime"
#include "cstdlib"
#define N  999
#define M_PI 3.1415
using Vec = Eigen::Vector3f;
using Veci = Eigen::Vector3i;
using Mat = Eigen::Matrix3f;
using Vec4 = Eigen::Vector4f;
namespace GLOO {
	SnowNode::SnowNode(int n, float hardening, float E, float nu) :grid_resl(n),grid(Grid(n)),hardening(hardening){
		mu_0 = E / (2 * (1 + nu));
		lambda_0 = E * nu / ((1 + nu) * (1 - 2 * nu));
		Dinv = 4 *n*n;
		initSnow();
	}
	void SnowNode::initSnow() {
        add_1kp_sphere(glm::vec3(0.3f, 0.25f, 0.5f), 1,glm::vec3(5.f,0,0));
		add_1kp_sphere(glm::vec3(0.65f, 0.25f, 0.5f), 1, glm::vec3(-10.f, 0, 0));
        for (auto &p : particles) {
            auto sphere_node = make_unique<SceneNode>();
            auto shader_ = std::make_shared<PhongShader>();
            sphere_node->CreateComponent<ShadingComponent>(shader_);
            sphere_node->CreateComponent<RenderingComponent>(PrimitiveFactory::CreateSphere(0.009f, 15, 15));
            sphere_node->CreateComponent<MaterialComponent>(std::make_shared<Material>(Material::GetDefault()));
            Material &material = sphere_node->GetComponentPtr<MaterialComponent>()->GetMaterial();
            material.SetDiffuseColor(glm::vec3(1.f, 1.f, 1.f));
            material.SetAmbientColor(glm::vec3(1.f));
            sphere_node->GetTransform().SetPosition(glm::vec3(p.x(0), p.x(1), p.x(2)));
            this->AddChild(std::move(sphere_node));
        }
    }
	void SnowNode::PlotSnow() {
	    for (int j = 0; j < particles.size(); j++) {
                this->GetChild(j).GetTransform().SetPosition(glm::vec3(particles[j].x(0), particles[j].x(1), particles[j].x(2)));
            }
        }
	void SnowNode::Update(double delta_time) {
		float dt = delta_time / 10.f;
		for (int i = 0; i < 10; i++) {
			grid.ClearGrid();
			Particle_to_Grid(dt);
			grid.Updategrid(dt);
			Grid_to_Particle(dt);
		}
		PlotSnow();
	}
	void PolarDe(Mat A, Mat& r, Mat& s) {
		Mat U, S, V;

		Eigen::JacobiSVD<Mat> svd(A, Eigen::ComputeThinU | Eigen::ComputeThinV);
		V = svd.matrixV(); U = svd.matrixU(); S=U.inverse() * A * (V.transpose().inverse());
		r = U * V.transpose();
		s = V * S*V.transpose();
	}
	void PrintMat3(Mat F) {
		std::cout << "\n" << F(0, 0) << "," << F(0, 1) << "," << F(0, 2) << "\n" <<
			F(1, 0) << "," << F(1, 1) << "," << F(1, 2) << "\n" <<
			F(2, 0) << "," << F(2, 1) << "," << F(2, 2) << std::endl;
	}
	void SnowNode::Particle_to_Grid(double delta_time) {
		for (auto &p : particles) {
			auto e = std::exp(hardening*(1.0f - p.Jp));
			auto mu = mu_0 * e;
			auto lambda = lambda_0 * e;
			auto J = p.F.determinant();
			Mat PF =  mu*(p.F*p.F.transpose()-Mat::Identity()) + lambda *std::log(J)*Mat::Identity();
			Mat stresstum = -(delta_time)  * Dinv*PF;
			grid.ScatterParticle(p,stresstum);
		}
	}
	float clamp(float x,float a,float b) {
		if (x > b) return b;
		if (x < a) return a;
		return x;
	}
	void SnowNode::DecomposePlasity(Particle& p) {
		Mat u, s, v;
		Eigen::JacobiSVD<Mat> svd(p.F, Eigen::ComputeThinU | Eigen::ComputeThinV);
		v = svd.matrixV(); u = svd.matrixU(); s = u.inverse() * p.F * v.transpose().inverse();
		for (int i = 0; i < 3; i++) {
			s(i,i) = clamp(s(i, i), 1.0f - 9e-1f, 1.0f + 5.5e-3f);
		}
		p.F = u * s*v.transpose();
		p.Jp /= p.F.determinant();
		p.Jp = clamp(p.Jp, 0.6f, 20.0f);
	}
	void SnowNode::Grid_to_Particle(double delta_time) {
		for (auto &p : particles) {
			grid.CollectGrid(p,delta_time);
			DecomposePlasity(p);
			p.x =Vec(clamp(p.x(0), 0, 1), clamp(p.x(1), 0, 1), clamp(p.x(2), 0, 1));
		}
	}

	void SnowNode::add_1kp_cube(glm::vec3 center, int c) {
		srand(time(NULL));
		for (int i = 0; i < 500; i++) {
			float x = std::rand() % (N + 1) / (float)(N + 1), y = std::rand() % (N + 1) / (float)(N + 1), z = std::rand() % (N + 1) / (float)(N + 1);
			Vec pos(x*2.f-1.f, y*2.f-1.f, z*2.f-1.f);
			auto cen = Vec(center[0], center[1], center[2]);
			auto v = pos * 0.08f + Vec(center[0], center[1], center[2]);
			particles.push_back(Particle(v, c));
		}
	}
	void SnowNode::add_1kp_sphere(glm::vec3 center, int c,glm::vec3 V) {
        srand(time(NULL));
        for (int i = 0; i < 280; i++) {
            double theta = 2.f * M_PI * std::rand() / double(RAND_MAX);
            double phi = acos(1.f - 2.f * std::rand() / double(RAND_MAX));
            double x = sin(phi) * cos(theta);
            double y = sin(phi) * sin(theta);
            double z = cos(phi);
            Vec pos(x, y, z);
            auto v = pos * 0.08f + Vec(center[0], center[1], center[2]);
			particles.push_back(Particle(v, c,Vec(V[0],V[1],V[2])));
        }
    }
}
