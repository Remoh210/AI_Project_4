#pragma once
#include <glm/glm.hpp>
#include "iRigidBody.h"
#include "iPhysicsWorld.h"
#include "sRigidBodyDef.h"
#include "sSoftBodyDef.h"
#include "shapes.h"
#include "iSoftBody.h"
#include "iWreckingBallPhysics.h"
#include "iConstraint.h"
#include "GL_Vertex.h"
#include "GL_Triangle.h"
#include "iVehicle.h"


namespace nPhysics
{
	class iPhysicsFactory
	{
	public:
		virtual ~iPhysicsFactory() {};
		virtual iPhysicsWorld* CreatePhysicsWorld() = 0;
		virtual iRigidBody* CreateRigidBody(const sRigidBodyDef& def, iShape* shape) = 0;
		virtual iSphereShape* CreateSphereShape(float radius) = 0;
		virtual iPlaneShape* CreatePlaneShape(const glm::vec3& normal, float constant) = 0;
		virtual iCylinderShape* CreateCylinderShape(const glm::vec3& halfExtent, int axis) = 0;
		virtual iCapsuleShape* CreateCapsuleShape(float height, float radius, int axis) = 0;
		virtual iBoxShape* CreateBoxShape(const glm::vec3& halfExtents) = 0;
		virtual iMeshCollider* CreateMeshCollider(const GL_Triangle* triangles, size_t numOfTriangles) = 0;

		virtual iVehicle* CreateVehicle(iRigidBody * chassis, iPhysicsWorld* world) = 0;
		virtual iConstraint* CreatHingeConstraint(iRigidBody* rb, const glm::vec3& pivot, const glm::vec3& axis) = 0;
		virtual iConstraint* CreatHingeConstraint(iRigidBody* rbA, iRigidBody* rbB, const glm::vec3& pivotInA, const glm::vec3& pivotInB, const glm::vec3& axisInA, const glm::vec3& axisInB) = 0;
		virtual iConstraint* CreatePointToPointConstraint(iRigidBody* rb, const glm::vec3& pivot) = 0;
		virtual iConstraint* CreatePointToPointConstraint(iRigidBody* rbA, iRigidBody* rbB, const glm::vec3& pivotInA, const glm::vec3& pivotInB) = 0;
		//virtual iSoftBody* CreateSoftBody(const sSoftBodyDef& sbDef) = 0;

		//virtual iWreckingBallPhysics* CreateWreckingBall(std::vector<sWreckingBallPNodeDef>& wreckingDef) = 0;

	};

}