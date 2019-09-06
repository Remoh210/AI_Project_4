#pragma once
#include <glm/glm.hpp>
#include "iRigidBody.h"

namespace nPhysics
{
	struct sWheelInfo
	{
		int index;
		glm::vec3 halfExtents;
		float width;
		float suspensionRestLength;
		float radius;
	};
	class iVehicle
	{
	public:
		virtual ~iVehicle() {}
		virtual void SetSteering(float value) = 0;
		virtual void AddWheel(sWheelInfo& wheelInfo) = 0;
		virtual void SetBrakes(float value) = 0;
		virtual void AddEngineForce(float value) = 0;
		virtual void GetChassisTransform(glm::mat4& transformOut) = 0;
		virtual glm::quat GetChassisRotation() = 0;
		virtual float GetSpeed() = 0;
		virtual glm::vec3 GetChassisPositon() = 0;
		virtual glm::quat GetWheelRotation(int index) = 0;
		virtual glm::vec3 GetWheelPosition(int index) = 0;
		virtual glm::vec3 GetWheelRotationEuler(int index) = 0;
		virtual void GetWheelTransform(size_t index, glm::mat4& transformOut) = 0;
	protected:
		iVehicle() {}
		iVehicle(iRigidBody* chassis) {}
	};
}