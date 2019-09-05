#pragma once

#include "BulletDynamics/Vehicle/btRaycastVehicle.h"
#include "Interfaces/iRigidBody.h"
#include "Interfaces/iVehicle.h"
#include "Interfaces/iPhysicsWorld.h"

namespace nPhysics
{
	class cBulletVehicle : public iVehicle
	{
	public:
		~cBulletVehicle();
		cBulletVehicle(iRigidBody* chassis);
		btRaycastVehicle* GetBulletVehicle();
		virtual void SetSteering(float value);
		virtual void SetBrakes(float value);
		virtual void AddEngineForce(float value);
		virtual void GetChassisTransform(glm::mat4& transformOut);
		virtual void GetWheelTransform(size_t index, glm::mat4& transformOut);
		void SetUpWorld(iPhysicsWorld* world);
		iPhysicsWorld* world;
	private:
		iRigidBody* mChassis;
		btRaycastVehicle* mRaycastVehicle;
	};
}