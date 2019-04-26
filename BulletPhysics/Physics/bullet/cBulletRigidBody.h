#pragma once
#include <Interfaces/iRigidBody.h>
#include <Interfaces/sRigidBodyDef.h>
#include <btBulletDynamicsCommon.h>


namespace nPhysics
{
	class cBulletRigidBody : public iRigidBody
	{

	public:
		cBulletRigidBody(const sRigidBodyDef& def, iShape* shape);
		~cBulletRigidBody();

		inline btRigidBody* GetBulletBody() { return mBody; }

		virtual iShape* GetShape();
		virtual glm::mat4 GetTransform();
		virtual glm::vec3 GetPosition();
		virtual glm::vec3 GetEulerRotation();
		virtual glm::mat4 GetMatRotation();
		virtual float GetMass();
		virtual bool GetCollision();
		virtual glm::vec3 GetVelocity();
		virtual glm::vec3 GetAccel();
		virtual glm::vec3 GetAngulatVelocity();
		virtual std::string GetGOName();

		virtual void SetPosition(glm::vec3 position);
		virtual void ApplyForce(glm::vec3 force, glm::vec3 relPos = glm::vec3(0.0f));
		virtual void ApplyImpulse(glm::vec3 force, glm::vec3 relPos = glm::vec3(0.0f));
		virtual void SetGravity(glm::vec3 gravity);
		virtual void SetEulerRotation(glm::vec3 rotation);
		virtual void SetMatRotation(glm::mat4 rotation);
		virtual void SetMass(float mass);
		virtual void SetVelocity(glm::vec3 velocity);
		virtual void SettAccel(glm::vec3 accel);
		virtual void SetCollision(bool coll);
		virtual void SetQuatRotation(glm::quat rotation);


		//void SettAccel(glm::vec3 accel);
	private:
		btDefaultMotionState* mMotionState;
		btRigidBody* mBody;
		iShape* mShape;
		bool bHasCollided;
		std::string mGameObjectName;
		float mMass;
	};

}