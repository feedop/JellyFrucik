export module simulation:engine;

import std;
import <glm/glm.hpp>;
import <glm/gtc/matrix_transform.hpp>;
import <glm/gtx/quaternion.hpp>;

import cube;
import handle;
import math;
import :config;
import :data;

static constexpr float linearRestingLength = 1.0f / 3;
static const float diagonalRestingLength = linearRestingLength * std::sqrtf(2.0f);

struct Neighbor
{
	int index;
	float restingLength;
};

std::array<std::vector<Neighbor>, 64> generateNeighborMap()
{
	static constexpr int cubeSize = 4;

	std::array<std::vector<Neighbor>, 64> neighbors;

	auto index = [](int x, int y, int z) -> int
	{
		return x + y * cubeSize + z * cubeSize * cubeSize;
	};

	for (int z = 0; z < cubeSize; ++z)
	{
		for (int y = 0; y < cubeSize; ++y)
		{
			for (int x = 0; x < cubeSize; ++x)
			{
				int currentIndex = index(x, y, z);

				// Check direct neighbors in the X, Y, and Z directions
				if (x > 0) neighbors[currentIndex].emplace_back(index(x - 1, y, z), linearRestingLength);
				if (x < cubeSize - 1) neighbors[currentIndex].emplace_back(index(x + 1, y, z), linearRestingLength);
				if (y > 0) neighbors[currentIndex].emplace_back(index(x, y - 1, z), linearRestingLength);
				if (y < cubeSize - 1) neighbors[currentIndex].emplace_back(index(x, y + 1, z), linearRestingLength);
				if (z > 0) neighbors[currentIndex].emplace_back(index(x, y, z - 1), linearRestingLength);
				if (z < cubeSize - 1) neighbors[currentIndex].emplace_back(index(x, y, z + 1), linearRestingLength);

				// Check face diagonals on the XY-plane
				if (x > 0 && y > 0) neighbors[currentIndex].emplace_back(index(x - 1, y - 1, z), diagonalRestingLength);
				if (x < cubeSize - 1 && y > 0) neighbors[currentIndex].emplace_back(index(x + 1, y - 1, z), diagonalRestingLength);
				if (x > 0 && y < cubeSize - 1) neighbors[currentIndex].emplace_back(index(x - 1, y + 1, z), diagonalRestingLength);
				if (x < cubeSize - 1 && y < cubeSize - 1) neighbors[currentIndex].emplace_back(index(x + 1, y + 1, z), diagonalRestingLength);

				// Check face diagonals on the XZ-plane
				if (x > 0 && z > 0) neighbors[currentIndex].emplace_back(index(x - 1, y, z - 1), diagonalRestingLength);
				if (x < cubeSize - 1 && z > 0) neighbors[currentIndex].emplace_back(index(x + 1, y, z - 1), diagonalRestingLength);
				if (x > 0 && z < cubeSize - 1) neighbors[currentIndex].emplace_back(index(x - 1, y, z + 1), diagonalRestingLength);
				if (x < cubeSize - 1 && z < cubeSize - 1) neighbors[currentIndex].emplace_back(index(x + 1, y, z + 1), diagonalRestingLength);

				// Check face diagonals on the YZ-plane
				if (y > 0 && z > 0) neighbors[currentIndex].emplace_back(index(x, y - 1, z - 1), diagonalRestingLength);
				if (y < cubeSize - 1 && z > 0) neighbors[currentIndex].emplace_back(index(x, y + 1, z - 1), diagonalRestingLength);
				if (y > 0 && z < cubeSize - 1) neighbors[currentIndex].emplace_back(index(x, y - 1, z + 1), diagonalRestingLength);
				if (y < cubeSize - 1 && z < cubeSize - 1) neighbors[currentIndex].emplace_back(index(x, y + 1, z + 1), diagonalRestingLength);
			}
		}
	}

	return neighbors;
}


export class SimulationEngine
{
public:
	SimulationEngine(const SimulationConfig& config) : config(config)
	{}

	void start()
	{
		data = SimulationData::zeroData();
		running = true;
		disturb();
		simulationThread = std::thread(&SimulationEngine::threadFunction, this);
	}

	void stop()
	{
		running = false;
		simulationThread.join();
	}

	void pause()
	{
		std::lock_guard<decltype(cvMutex)> lk(cvMutex);
		paused = true;
	}

	void resume()
	{
		std::lock_guard<decltype(cvMutex)> lk(cvMutex);
		paused = false;
		cv.notify_one();
	}

	void setConfig(const SimulationConfig& config)
	{
		const std::lock_guard<std::mutex> lock(configMutex);
		auto oldModel = this->config.handleModel;
		this->config = config;
		this->config.handleModel = oldModel;
	}

	void setConfigHandleModel(const glm::mat4& model)
	{
		const std::lock_guard<std::mutex> lock(configMutex);
		this->config.handleModel = model;
	}

	void setData(const SimulationData& data)
	{
		stop();
		this->data = data;
		start();
	}

	void disturb()
	{
		float disturbance;
		bool disturbVelocities;
		{
			const std::lock_guard<std::mutex> lock(configMutex);
			disturbance = config.disturbance;
			disturbVelocities = config.disturbVelocities;
		}
		SimulationData localData;
		{
			const std::lock_guard<std::mutex> lock(dataMutex);
			localData = data;
		}
		
		if (disturbVelocities)
		{
			std::for_each(std::execution::par, localData.velocities.begin(), localData.velocities.end(), [&](auto&& vel)
			{
				vel += disturbance * math::randomVector();
			});
		}
		else
		{
			std::for_each(std::execution::par, localData.positions.begin(), localData.positions.end(), [&](auto&& pos)
			{
				pos += disturbance * math::randomVector();
			});
		}

		{
			const std::lock_guard<std::mutex> lock(disturbDataMutex);
			data.positions = localData.positions;
			data.velocities = localData.velocities;
		}
	}

	SimulationData getData()
	{
		const std::lock_guard<std::mutex> lock(dataMutex);
		return data;
	}

	inline bool isPaused() const
	{
		return paused;
	}

	inline float getBoxSize() const
	{
		return boxScale;
	}

	static const auto& getNeighborMap()
	{
		return neighborMap;
	}

	inline static constexpr std::array<int, 8> indicesConnectedToHandle{ 0, 3, 15, 12, 48, 51, 63, 60 };

private:
	SimulationConfig config;
	float boxScale = 6.0f;
	SimulationData data;

	std::mutex configMutex, dataMutex, disturbDataMutex, cvMutex;
	std::condition_variable cv;
	std::atomic_bool running = false;
	bool paused = false;
	std::thread simulationThread;

	inline static const auto neighborMap = generateNeighborMap();

	void threadFunction()
	{
		static const auto handleVertices = Handle::createVertices();
		static constexpr auto range = std::ranges::views::iota(0, 64);
		static constexpr auto handleRange = std::ranges::views::iota(0, 8);

		SimulationData localData;
		SimulationConfig localConfig;
		{
			const std::lock_guard<std::mutex> lock(configMutex);
			localConfig = config;
		}

		while (running)
		{
			nap();

			auto clockBegin = std::chrono::steady_clock::now();
			{			
				// Copy config to local
				{
					const std::lock_guard<std::mutex> lock(configMutex);
					localConfig = config;
				}
				// Copy data to local
				{
					const std::lock_guard<std::mutex> lock(dataMutex);
					localData = data;
				}

				const std::lock_guard<std::mutex> lock(disturbDataMutex);

				// Simulation
				
				// Additional spring forces with the handle
				std::for_each(std::execution::par, handleRange.begin(), handleRange.end(), [&](auto i)
				{
					int myIndex = indicesConnectedToHandle[i];
					glm::vec3 toNeighbor = glm::vec3(localConfig.handleModel * glm::vec4(handleVertices[i], 1)) - localData.positions[myIndex];
					float l = glm::length(toNeighbor);
					if (l < math::eps)
						return; // continue
					glm::vec3 direction = glm::normalize(toNeighbor);
					float projectedVelocity = glm::dot(localData.velocities[myIndex], direction);
					float f = -localConfig.k * projectedVelocity + localConfig.c2 * l;
					glm::vec3 dv_dt = f / localConfig.m * direction;
					localData.velocities[myIndex] += dv_dt * localConfig.dt;
				});
				
				// Calculate velocities
				std::for_each(std::execution::par, range.begin(), range.end(), [&](auto i)
				{	
					for (auto& neighbor : neighborMap[i])
					{
						// Calculate spring forces
						glm::vec3 toNeighbor = localData.positions[neighbor.index] - localData.positions[i];
						glm::vec3 direction = glm::normalize(toNeighbor);
						float l = glm::length(toNeighbor) - neighbor.restingLength;
						float projectedVelocity = glm::dot(localData.velocities[i], direction);
						float f = - localConfig.k * projectedVelocity + localConfig.c1 * l;
						glm::vec3 dv_dt = f / localConfig.m * direction;
						localData.velocities[i] += dv_dt * localConfig.dt;
					}
				});		

				// Propagate calculated velocities into positions
				std::for_each(std::execution::par, range.begin(), range.end(), [&](auto i)
				{
					localData.positions[i] += localConfig.dt * localData.velocities[i];
				});

				// Detect collisions
				std::for_each(std::execution::par, range.begin(), range.end(), [&](auto i)
				{
					detectCollisions(localData.positions[i], localData.velocities[i], localConfig.dt, localConfig.mi);
				});

				localData.t += localConfig.dt;

				// Copy data
				{
					const std::lock_guard<std::mutex> lock(dataMutex);
					data = localData;
				}	
			}
			
			// Sleep
			int ns = localConfig.dt * 1e9;
			auto clockEnd = std::chrono::steady_clock::now();
			int elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(clockEnd - clockBegin).count();

			if (elapsed < ns)
				std::this_thread::sleep_for(std::chrono::nanoseconds(ns - elapsed));
			else
				std::cerr << "Can't sleep - the simulation step took too long\n";
		}
	}

	void nap()
	{
		std::unique_lock<decltype(cvMutex)> lk(cvMutex);
		cv.wait(lk, [this] { return !paused; });
	}

	void detectCollisions(glm::vec3& position, glm::vec3& velocity, float dt, float mi)
	{
		// Check each axis for potential collision
		for (int axis = 0; axis < 3; ++axis)
		{
			// Axis-aligned components
			float axisComponent = position[axis];
			float axisVelocity = velocity[axis];

			auto bounce = [&](float traveledOutside)
			{
				float timeOutside = traveledOutside / axisVelocity;
				position -= velocity * timeOutside; // Back to intersection point
				velocity[axis] *= -mi; // Reflect and slow down
				position += velocity * timeOutside; // Travel the remaining distance with the new velocity
			};

			// Check collision with the two faces of this axis
			if (axisComponent > boxScale / 2)
			{
				bounce(axisComponent - boxScale / 2);
				detectCollisions(position, velocity, dt, mi);
				return;
			}
			if (axisComponent < -boxScale / 2)
			{
				bounce(axisComponent + boxScale / 2);
				detectCollisions(position, velocity, dt, mi);
				return;
			}
		}
	}
};