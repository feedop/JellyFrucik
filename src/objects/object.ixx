export module object;

import std;
import <glm/glm.hpp>;
import <glm/gtc/quaternion.hpp>;
import <glm/gtx/quaternion.hpp>;

import drawable;
import math;

export class Object : public Drawable
{
public:
	inline void setModel(const glm::mat4& newModel)
	{
		model = newModel;
	}

	inline auto& getModel() const
	{
		return model;
	}

	void translate(const glm::vec3& translation)
	{
		position += translation;
		update();
	}

	void rotate(float angleRadians, const glm::vec3& axis)
	{
		rotation = glm::normalize(glm::angleAxis(angleRadians, axis) * rotation);
		update();
	}

protected:
	glm::vec3 position{ 0, 0, 0 };
	glm::quat rotation{ {0,0,0} };
	glm::mat4 model{ 1.0f };
	glm::vec4 color;

	Object(const glm::vec4& color) : color(color)
	{}

private:
	void update()
	{
		model = math::translate(position) * math::translate({ 0.5, 0.5, 0.5 }) * glm::toMat4(rotation) * math::translate({ -0.5, -0.5, -0.5 });
	}
};