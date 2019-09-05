#pragma once
#include <Interfaces/iPhysicsWorld.h>
#include <Interfaces/iConstraint.h>
//#include <Interfaces/iRigidBody.h>
#include "btBulletDynamicsCommon.h"
#include <stdio.h>
#include <string>

namespace nPhysics {
	class cBulletPhysicsWorld : public iPhysicsWorld
	{
	public:
		cBulletPhysicsWorld();
		~cBulletPhysicsWorld();

		void SetGravity(const glm::vec3& gravity);
		bool AddBody(iRigidBody* body);
		bool RemoveBody(iRigidBody* body);

		//Constraints
		virtual bool AddVehicle(iVehicle* vehicle);
		virtual void AddConstraint(iConstraint* constraint);
		virtual void RemoveConstraint(iConstraint* constraint);
		virtual std::pair<std::string, std::string> GetLastColPair();
		virtual bool RayCast(glm::vec3& from, glm::vec3& to);
		virtual iRigidBody* RayCastGetObject(glm::vec3& from, glm::vec3& to);
		void Update(float dt);

		btDiscreteDynamicsWorld* GetBulletWorld();
	protected:
		cBulletPhysicsWorld(const cBulletPhysicsWorld& other) {}
		cBulletPhysicsWorld& operator=(const cBulletPhysicsWorld& other) { return *this; }
	private:

		btDefaultCollisionConfiguration* mCollisionConfiguration;

		//use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
		btCollisionDispatcher* mDispatcher;

		//btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
		btBroadphaseInterface* mOverlappingPairCache;

		//the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
		btSequentialImpulseConstraintSolver* mSolver = new btSequentialImpulseConstraintSolver;

		btDiscreteDynamicsWorld* mDynamicsWorld;

	};

}