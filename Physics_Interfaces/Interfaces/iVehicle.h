#pragma once
#include <glm/glm.hpp>
#include "iRigidBody.h"

namespace nPhysics
{
	class iVehicle
	{
	public:
		virtual ~iVehicle() {}
		virtual void SetSteering(float value) = 0;
		virtual void SetBrakes(float value) = 0;
		virtual void AddEngineForce(float value) = 0;
		virtual void GetChassisTransform(glm::mat4& transformOut) = 0;
		virtual void GetWheelTransform(size_t index, glm::mat4& transformOut) = 0;
	protected:
		iVehicle() {}
		iVehicle(iRigidBody* chassis) {}
	};
}