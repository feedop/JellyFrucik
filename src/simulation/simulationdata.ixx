export module simulation:data;

import std;
import <glm/vec3.hpp>;

import cube;

export struct SimulationData
{
	std::array<glm::vec3, 64> positions;
	std::array<glm::vec3, 64> velocities;
	double t;

	static constexpr SimulationData zeroData()
	{
		SimulationData data;
		data.t = 0.0;
		data.positions = Cube::baseMassPointPositions;
		std::fill(data.velocities.begin(), data.velocities.end(), glm::vec3{0,0,0});
		return data;
	}
};