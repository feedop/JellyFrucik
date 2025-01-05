export module simulation:config;

import std;
import <glm/mat4x4.hpp>;

export struct SimulationConfig
{
	float dt;
	float m;
	float c1; // Between masses
	float c2; // Between masses and the wireframe
	float k;
	float disturbance;
	float mi;
	bool disturbVelocities;
	glm::mat4 handleModel;
};