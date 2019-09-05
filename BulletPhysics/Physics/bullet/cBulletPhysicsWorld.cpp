#include "cBulletPhysicsWorld.h"
#include "nConvert.h"
#include "cBulletRigidBody.h"
#include "cBulletConstraints.h"
#include "cBulletVehicle.h"
#include "BulletCollision/CollisionDispatch/btInternalEdgeUtility.h"


std::pair<std::string, std::string> lastColNames;

extern ContactAddedCallback	gContactAddedCallback;


//gContactAddedCallback = MyContactAddedCallback;//or add it to an existing callback
//
//bool MyContactAddedCallback(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1)
//{
//	btAdjustInternalEdgeContacts(cp, colObj1Wrap, colObj0Wrap, partId1, index1);
//	return true;
//}

bool callbackFunc(btManifoldPoint& cp,
	const btCollisionObjectWrapper* obj1, int id1, int index1,
	const btCollisionObjectWrapper* obj2, int id2, int index2)
{
	((nPhysics::cBulletRigidBody*)obj1->getCollisionObject()->getUserPointer())->SetCollision(true);
	((nPhysics::cBulletRigidBody*)obj2->getCollisionObject()->getUserPointer())->SetCollision(true);
	lastColNames.first = ((nPhysics::cBulletRigidBody*)obj1->getCollisionObject()->getUserPointer())->GetGOName();
	lastColNames.second = ((nPhysics::cBulletRigidBody*)obj2->getCollisionObject()->getUserPointer())->GetGOName();



	btAdjustInternalEdgeContacts(cp, obj1, obj2, id1, index1);
	return false;
}

nPhysics::cBulletPhysicsWorld::cBulletPhysicsWorld()
	: iPhysicsWorld()
{
	mCollisionConfiguration = new btDefaultCollisionConfiguration();

	//use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	mDispatcher = new btCollisionDispatcher(mCollisionConfiguration);

	//btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	mOverlappingPairCache = new btDbvtBroadphase();

	//the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	mSolver = new btSequentialImpulseConstraintSolver;

	mDynamicsWorld = new btDiscreteDynamicsWorld(mDispatcher, mOverlappingPairCache, mSolver, mCollisionConfiguration);
	mDynamicsWorld->setGravity(btVector3(0, -10, 0));

	gContactAddedCallback = callbackFunc;
}

nPhysics::cBulletPhysicsWorld::~cBulletPhysicsWorld()
{
	if(mDynamicsWorld)
	{
		delete mDynamicsWorld;
		mDynamicsWorld = 0;
	}
	if (mSolver)
	{
		delete mSolver;
		mSolver = 0;
	}
	if (mOverlappingPairCache)
	{
		delete mOverlappingPairCache;
		mOverlappingPairCache = 0;
	}
	if (mDispatcher)
	{
		delete mDispatcher;
		mDispatcher = 0;
	}
	if (mCollisionConfiguration)
	{
		delete mCollisionConfiguration;
		mCollisionConfiguration = 0;
	}
}






void nPhysics::cBulletPhysicsWorld::SetGravity(const glm::vec3& gravity)
{
	mDynamicsWorld->setGravity(nConvert::ToBullet(gravity));
}

bool nPhysics::cBulletPhysicsWorld::AddBody(iRigidBody* body)
{
	//Check Type
	//If type is BODY_TYPE_RIGID_BODY
	cBulletRigidBody* bulletRigidBody = dynamic_cast<cBulletRigidBody*>(body);
	if (!bulletRigidBody)
	{
		return false;
	}
	mDynamicsWorld->addRigidBody(bulletRigidBody->GetBulletBody());


	return true;

}

bool nPhysics::cBulletPhysicsWorld::RemoveBody(iRigidBody * body)
{
	cBulletRigidBody* bulletRigidBody = dynamic_cast<cBulletRigidBody*>(body);
	if (!bulletRigidBody)
	{
		return false;
	}
	btRigidBody* BulletBtBody = bulletRigidBody->GetBulletBody();
	mDynamicsWorld->removeRigidBody(BulletBtBody);
	return true;

}

bool nPhysics::cBulletPhysicsWorld::AddVehicle(iVehicle * vehicle)
{
	cBulletVehicle* bulletVehicle = dynamic_cast<cBulletVehicle*>(vehicle);
	if (!bulletVehicle)
	{
		return false;
	}
	this->mDynamicsWorld->addVehicle(bulletVehicle->GetBulletVehicle());
	return true;
}

void nPhysics::cBulletPhysicsWorld::AddConstraint(iConstraint * constraint)
{
	eConstraintType type = constraint->GetConstraintType();

	switch (type)
	{
	case nPhysics::CONSTRAINT_TYPE_POINT_TO_POINT:
	{
		cBulletPoinToPointConstraint* basConstraint = dynamic_cast<cBulletPoinToPointConstraint*>(constraint);
		this->mDynamicsWorld->addConstraint(basConstraint->GetTypedConstraint());

	}
		break;
	case nPhysics::CONSTRAINT_TYPE_HINGE:
	{
		cBulletHingeConstraint* hingeConstraint = dynamic_cast<cBulletHingeConstraint*>(constraint);
		this->mDynamicsWorld->addConstraint(hingeConstraint->GetTypedConstraint());
	}
		break;
	default:
		break;
	}
}

void nPhysics::cBulletPhysicsWorld::RemoveConstraint(iConstraint * constraint)
{
}

std::pair<std::string, std::string> nPhysics::cBulletPhysicsWorld::GetLastColPair()
{
	return lastColNames;
}

bool nPhysics::cBulletPhysicsWorld::RayCast(glm::vec3 & from, glm::vec3& to)
{
	btCollisionWorld::ClosestRayResultCallback res(nConvert::ToBullet(from), nConvert::ToBullet(to));
	mDynamicsWorld->rayTest(nConvert::ToBullet(from), nConvert::ToBullet(to), res);
	return res.hasHit();
}

nPhysics::iRigidBody* nPhysics::cBulletPhysicsWorld::RayCastGetObject(glm::vec3 & from, glm::vec3& to)
{
	btCollisionWorld::ClosestRayResultCallback resObject(nConvert::ToBullet(from), nConvert::ToBullet(to));
	mDynamicsWorld->rayTest(nConvert::ToBullet(from), nConvert::ToBullet(to), resObject);
	//TODO: Return hit object
	//return dynamic_cast<nPhysics::cBulletRigidBody*>resObject.m_collisionObject->getUserPointer();
	if (resObject.hasHit())
	{
		//return ((nPhysics::iRigidBody*)resObject.m_collisionObject->getUserPointer());
		return reinterpret_cast<nPhysics::iRigidBody*>(resObject.m_collisionObject->getUserPointer());
	}
	else
	{
		return NULL;
	}
	//return resObject.hasHit();
}

void nPhysics::cBulletPhysicsWorld::Update(float dt)
{
	//not working with 120hz monitor?
	mDynamicsWorld->stepSimulation(dt, 10);
	//mDynamicsWorld->stepSimulation(dt, 0);
}

btDiscreteDynamicsWorld * nPhysics::cBulletPhysicsWorld::GetBulletWorld()
{
	return mDynamicsWorld;
}
