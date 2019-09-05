#pragma once
#include "iRigidBody.h"
#include "iConstraint.h"
#include "iSoftBody.h"
#include "iVehicle.h"
namespace nPhysics {
	class iPhysicsWorld
	{
	public:
		virtual ~iPhysicsWorld() {};

		virtual void SetGravity(const glm::vec3& gravity) = 0;
		virtual bool AddBody(iRigidBody* body) = 0;
		virtual bool RemoveBody(iRigidBody* body) = 0;
		virtual bool AddVehicle(iVehicle* vehicle) = 0;
		//Constraints
		

		virtual void AddConstraint(iConstraint* constraint) = 0;
		virtual void RemoveConstraint(iConstraint* constraint) = 0;
		virtual std::pair<std::string, std::string> GetLastColPair() = 0;
		virtual bool RayCast(glm::vec3& from, glm::vec3& to) = 0;
		virtual iRigidBody* RayCastGetObject(glm::vec3& from, glm::vec3& to) = 0;
		//SoftBody
		//virtual bool AddBody(iSoftBody* body) = 0;
		//virtual bool RemoveBody(iSoftBody* body) = 0;

		virtual void Update(float dt) = 0;

	protected:
		iPhysicsWorld() {}
		iPhysicsWorld(const iPhysicsWorld& other) {}
		iPhysicsWorld& operator=(const iPhysicsWorld& other) { return *this; }
	};

}