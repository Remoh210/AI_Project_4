#pragma once
#include <Interfaces/iPhysicsFactory.h>
#include "cBulletConstraints.h"



namespace nPhysics
{
	class cBulletPhysicsFactory : public iPhysicsFactory
	{
	public:
		virtual ~cBulletPhysicsFactory();
		virtual iPhysicsWorld* CreatePhysicsWorld();
		virtual iRigidBody* CreateRigidBody(const sRigidBodyDef& def, iShape* shape);

		//basicShapes
		virtual iSphereShape* CreateSphereShape(float radius);
		virtual iPlaneShape* CreatePlaneShape(const glm::vec3& normal, float constant);
		virtual iCylinderShape* CreateCylinderShape(const glm::vec3& halfExtent, int axis);
		virtual iCapsuleShape* CreateCapsuleShape(float height, float radius, int axis);
		virtual iBoxShape* CreateBoxShape(const glm::vec3& halfExtents);

		virtual iMeshCollider* CreateMeshCollider(const GL_Triangle * triangles, size_t numOfTriangles);

		virtual iVehicle* CreateVehicle(iRigidBody * chassis, iPhysicsWorld* world);
		virtual iConstraint* CreatHingeConstraint(iRigidBody* rb, const glm::vec3& pivot, const glm::vec3& axis);
		virtual iConstraint* CreatHingeConstraint(iRigidBody* rbA, iRigidBody* rbB, const glm::vec3& pivotInA, const glm::vec3& pivotInB, 
			const glm::vec3& axisInA, const glm::vec3& axisInB);
		virtual iConstraint* CreatePointToPointConstraint(iRigidBody* rb, const glm::vec3& pivot);
		virtual iConstraint* CreatePointToPointConstraint(iRigidBody* rbA, iRigidBody* rbB,
			const glm::vec3& pivotInA, const glm::vec3& pivotInB);
	};
}
