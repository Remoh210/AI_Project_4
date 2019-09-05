#include "cBulletVehicle.h"
#include "nConvert.h"
#include "cBulletPhysicsWorld.h"
#include "cBulletRigidBody.h"

namespace nPhysics
{

	cBulletVehicle::cBulletVehicle(iRigidBody* chassis)
	{
		mChassis = chassis;
	}
	btRaycastVehicle * cBulletVehicle::GetBulletVehicle()
	{
		return mRaycastVehicle;
	}
	void cBulletVehicle::SetSteering(float value)
	{
		mRaycastVehicle->setSteeringValue(value, 2);
		mRaycastVehicle->setSteeringValue(value, 3);
	}
	void cBulletVehicle::SetBrakes(float value)
	{
		mRaycastVehicle->setBrake(value, 2);
		mRaycastVehicle->setBrake(value, 3);
	}
	void cBulletVehicle::AddEngineForce(float value)
	{
		mRaycastVehicle->applyEngineForce(value, 0);
		mRaycastVehicle->applyEngineForce(value, 1);
	}
	void cBulletVehicle::GetChassisTransform(glm::mat4 & transformOut)
	{
		transformOut = nConvert::ToSimple(mRaycastVehicle->getChassisWorldTransform());
	}
	void cBulletVehicle::GetWheelTransform(size_t index, glm::mat4 & transformOut)
	{
		transformOut = nConvert::ToSimple(mRaycastVehicle->getWheelTransformWS(index));
	}

	void cBulletVehicle::SetUpWorld(iPhysicsWorld * world)
	{
		btRaycastVehicle::btVehicleTuning VehicleTuning;
		cBulletPhysicsWorld* bulletworld = dynamic_cast<cBulletPhysicsWorld*>(world);
		btVehicleRaycaster* raycaster = new btDefaultVehicleRaycaster(bulletworld->GetBulletWorld());
		cBulletRigidBody* bRb = dynamic_cast<cBulletRigidBody*>(mChassis);
		mRaycastVehicle = new btRaycastVehicle(VehicleTuning, bRb->GetBulletBody(), raycaster);
	}

	cBulletVehicle::~cBulletVehicle()
	{
	}
}