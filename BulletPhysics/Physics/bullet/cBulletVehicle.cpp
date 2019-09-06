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
		mRaycastVehicle->setSteeringValue(value, 0);
		mRaycastVehicle->setSteeringValue(value, 1);
	}

	float cBulletVehicle::GetSpeed()
	{
		return mRaycastVehicle->getCurrentSpeedKmHour();
	}

	void cBulletVehicle::AddWheel(sWheelInfo& wheelInfo)
	{
		//mRaycastVehicle->setCoordinateSystem(0, 1, 2);
		btRaycastVehicle::btVehicleTuning VehicleTuning;
		//VehicleTuning.m_maxSuspensionForce = 50.51;
		btRaycastVehicle::btVehicleTuning VehicleTuning2;
		btRaycastVehicle::btVehicleTuning VehicleTuning3;
		btRaycastVehicle::btVehicleTuning VehicleTuning4;
		float connectionHeight = -3.2f;
		btVector3 Dir(0.0f, 0.0f, 1.0f);
		btVector3 Axil(-1.0f, 0.0f, 0.0f);
		float suspension = 2.0f;

		//float suspension_stiffness = 50.0f;
		//VehicleTuning.m_suspensionStiffness = suspension_stiffness;
		//btVector3 ConnectionPoint(wheelInfo.halfExtents.y - (0.3f * wheelInfo.width), connectionHeight, 2 * wheelInfo.halfExtents.y - wheelInfo.radius);
		//mRaycastVehicle->addWheel(ConnectionPoint, btVector3(0.0f, -1.0f, 0.0f), Axil, 0.2f, 5.0f, VehicleTuning4, false);
		//ConnectionPoint = btVector3(-wheelInfo.halfExtents.y - (0.3f * wheelInfo.width), connectionHeight, 2 * wheelInfo.halfExtents.y - wheelInfo.radius)
	    mRaycastVehicle->addWheel(btVector3(-5.0f, connectionHeight, 10.0f), btVector3(0.0f, -1.0f, 0.0f), Axil, suspension, wheelInfo.radius, VehicleTuning, true);
		mRaycastVehicle->addWheel(btVector3(5.0f, connectionHeight, 10.0f), btVector3(0.0f, -1.0f, 0.0f), Axil, suspension, wheelInfo.radius, VehicleTuning, true);

		mRaycastVehicle->addWheel(btVector3(-5.0f, connectionHeight, -10.0f), btVector3(0.0f, -1.0f, 0.0f), Axil, suspension, wheelInfo.radius, VehicleTuning, false);
		mRaycastVehicle->addWheel(btVector3(5.0f, connectionHeight, -10.0f), btVector3(0.0f, -1.0f, 0.0f), Axil, suspension, wheelInfo.radius, VehicleTuning, false);

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
		return nConvert::ToSimple(mRaycastVehicle->getWheelInfo(index).m_worldTransform.getRotation());
	}
	glm::vec3 cBulletVehicle::GetWheelRotationEuler(int index)
	{
		//glm::vec3 EulerRotation;
		float x, y, z;
		mRaycastVehicle->getWheelInfo(index).m_worldTransform.getRotation().getEulerZYX(x, y, z);
		return glm::vec3(x, y, z);
	}
	glm::vec3 cBulletVehicle::GetWheelPosition(int index)
	{
		return nConvert::ToSimple(mRaycastVehicle->getWheelInfo(index).m_worldTransform.getOrigin());
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
		//VehicleTuning.m_suspensionCompression =
		cBulletPhysicsWorld* bulletworld = dynamic_cast<cBulletPhysicsWorld*>(world);
		btVehicleRaycaster* raycaster = new btDefaultVehicleRaycaster(bulletworld->GetBulletWorld());
		cBulletRigidBody* bRb = dynamic_cast<cBulletRigidBody*>(mChassis);
		mRaycastVehicle = new btRaycastVehicle(VehicleTuning, bRb->GetBulletBody(), raycaster);
		mRaycastVehicle->setCoordinateSystem(0, 1, 2);
	}

	cBulletVehicle::~cBulletVehicle()
	{
	}
}