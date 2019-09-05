#include "cBulletPhysicsFactory.h"
#include "cBulletRigidBody.h"
#include "cBulletVehicle.h"
#include "bullet_shapes.h"
#include "cBulletPhysicsWorld.h"
#include <nConvert.h>

namespace nPhysics
{
	cBulletPhysicsFactory::~cBulletPhysicsFactory() {}

	iPhysicsWorld* cBulletPhysicsFactory::CreatePhysicsWorld()
	{
		return new cBulletPhysicsWorld();
	}

	iRigidBody* cBulletPhysicsFactory::CreateRigidBody(const sRigidBodyDef & def, iShape* shape)
	{
		return new cBulletRigidBody(def, shape);
	}

	iSphereShape* cBulletPhysicsFactory::CreateSphereShape(float radius)
	{
		return new cBulletSphereShape(radius);
	}

	iPlaneShape* cBulletPhysicsFactory::CreatePlaneShape(const glm::vec3 & normal, float constant)
	{
		return new cBulletPlaneShape(normal, constant);
	}

	iCylinderShape * cBulletPhysicsFactory::CreateCylinderShape(const glm::vec3 & halfExtent, int axis)
	{
		return new cBulletCylinderShape(halfExtent, axis);
	}

	iCapsuleShape* cBulletPhysicsFactory::CreateCapsuleShape(float height, float radius, int axis)
	{
		return new cBulletCapsuleShape(height, radius, axis);
	}

	iBoxShape * cBulletPhysicsFactory::CreateBoxShape(const glm::vec3 & halfExtents)
	{
		return new cBulletBoxShape(halfExtents);
	}

	iMeshCollider * cBulletPhysicsFactory::CreateMeshCollider(const GL_Triangle* triangles, size_t numOfTriangles)
	{
		return new cBulletMeshCollider(triangles, numOfTriangles);
	}

	iVehicle * cBulletPhysicsFactory::CreateVehicle(iRigidBody * chassis, iPhysicsWorld* world)
	{
		cBulletVehicle* vehicle = new cBulletVehicle(chassis);
		vehicle->SetUpWorld(world);
		return vehicle;

	}

	iConstraint * cBulletPhysicsFactory::CreatHingeConstraint(iRigidBody * rb, const glm::vec3 & pivot, const glm::vec3 & axis)
	{
		return new cBulletHingeConstraint((cBulletRigidBody*)rb, nConvert::ToBullet(pivot), nConvert::ToBullet(axis));
	}

	iConstraint * cBulletPhysicsFactory::CreatHingeConstraint(iRigidBody * rbA, iRigidBody * rbB, const glm::vec3 & pivotInA, const glm::vec3 & pivotInB, const glm::vec3 & axisInA, const glm::vec3 & axisInB)
	{
		return new cBulletHingeConstraint((cBulletRigidBody*)rbA, (cBulletRigidBody*)rbB, nConvert::ToBullet(pivotInA), nConvert::ToBullet(pivotInB), nConvert::ToBullet(axisInA), nConvert::ToBullet(axisInB));
	}

	iConstraint * cBulletPhysicsFactory::CreatePointToPointConstraint(iRigidBody * rb, const glm::vec3 & pivot)
	{
		return new cBulletPoinToPointConstraint((cBulletRigidBody*)rb, nConvert::ToBullet(pivot));
	}

	iConstraint * cBulletPhysicsFactory::CreatePointToPointConstraint(iRigidBody * rbA, iRigidBody * rbB, const glm::vec3 & pivotInA, const glm::vec3 & pivotInB)
	{
		return new cBulletPoinToPointConstraint((cBulletRigidBody*)rbA, (cBulletRigidBody*)rbB, nConvert::ToBullet(pivotInA), nConvert::ToBullet(pivotInB));
	}




}