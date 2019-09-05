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
	void cBulletVehicle::AddWheel()
	{
		//mRaycastVehicle->setCoordinateSystem(0, 1, 2);
		btRaycastVehicle::btVehicleTuning VehicleTuning;
		btRaycastVehicle::btVehicleTuning VehicleTuning2;
		btRaycastVehicle::btVehicleTuning VehicleTuning3;
		btRaycastVehicle::btVehicleTuning VehicleTuning4;
		btVector3 ConnectionPoint(-10.0f, -10.0f, 10.0f);
		btVector3 Dir(0.0f, 0.0f, 1.0f);
		btVector3 Axil(-1.0f, 0.0f, 0.0f);
		
	    mRaycastVehicle->addWheel(btVector3(-5.0f, -5.0f, 10.0f), btVector3(0.0f, -1.0f, 0.0f), Axil, 0.2f, 5.0f, VehicleTuning2, true);
		mRaycastVehicle->addWheel(btVector3(5.0f, -5.0f, 10.0f), btVector3(0.0f, -1.0f, 0.0f), Axil, 0.2f, 5.0f, VehicleTuning2, true);

		mRaycastVehicle->addWheel(btVector3(-5.0f, -5.0f, -10.0f), btVector3(0.0f, -1.0f, 0.0f), Axil, 0.2f, 5.0f, VehicleTuning3, false);
		mRaycastVehicle->addWheel(btVector3(5.0f, -5.0f, -10.0f), btVector3(0.0f, -1.0f, 0.0f), Axil, 0.2f, 5.0f, VehicleTuning4, false);
	}
	void cBulletVehicle::SetBrakes(float value)
	{
		mRaycastVehicle->setBrake(value, 2);
		mRaycastVehicle->setBrake(value, 3);
	}
	void cBulletVehicle::AddEngineForce(float value)
	{
		//mRaycastVehicle->updateVehicle(0.1f);
		//mRaycastVehicle->updateSuspension(0.1f);
		//mRaycastVehicle->updateFriction(0.1f);
		mRaycastVehicle->applyEngineForce(value, 0);
		mRaycastVehicle->applyEngineForce(value, 1);
		mRaycastVehicle->applyEngineForce(value, 2);
		mRaycastVehicle->applyEngineForce(value, 3);
	}
	glm::quat cBulletVehicle::GetChassisRotation()
	{
		return nConvert::ToSimple(mRaycastVehicle->getChassisWorldTransform().getRotation());
	}
	glm::vec3 cBulletVehicle::GetChassisPositon()
	{
		return nConvert::ToSimple(mRaycastVehicle->getChassisWorldTransform().getOrigin());
	}
	glm::quat cBulletVehicle::GetWheelRotation(int index)
	{
		return nConvert::ToSimple(mRaycastVehicle->getWheelTransformWS(index).getRotation());
	}
	glm::vec3 cBulletVehicle::GetWheelPosition(int index)
	{
		return nConvert::ToSimple(mRaycastVehicle->getWheelTransformWS(index).getOrigin());
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