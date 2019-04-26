#pragma once
#include <glm/glm.hpp>
#include "iShape.h"
#include <glm\mat4x4.hpp>
#include <string>
namespace nPhysics
{
	class iRigidBody
	{	
	public:
		virtual ~iRigidBody() {};
		virtual iShape* GetShape() = 0;

		virtual glm::mat4 GetTransform() = 0;
		virtual glm::vec3 GetPosition() = 0;
		virtual glm::vec3 GetEulerRotation() = 0;
		virtual glm::mat4 GetMatRotation() = 0;
		virtual float GetMass() = 0;
		virtual glm::vec3 GetVelocity() = 0;
		virtual glm::vec3 GetAngulatVelocity() = 0;
		virtual glm::vec3 GetAccel() = 0;
		virtual bool GetCollision() = 0;
		virtual std::string GetGOName() = 0;
		
		


		//virtual void SetTransform(glm::mat4 transform) = 0;
		//virtual void ApplyForce(const glm::vec3& force) = 0;
		virtual void SetCollision(bool coll) = 0;
		virtual void SetPosition(glm::vec3 position) = 0;
		virtual void SetEulerRotation(glm::vec3 rotation) = 0;
		virtual void SetMatRotation(glm::mat4 rotation) = 0;
		virtual void SetQuatRotation(glm::quat rotation) = 0;
		virtual void SetMass(float mass) = 0;
		virtual void SetVelocity(glm::vec3 velocity) = 0;
		virtual void SettAccel(glm::vec3 accel) = 0;
		//virtual void GetAABB(glm::vec3& minBoundsOut, glm::vec3& maxBoundsOut) = 0;

		virtual void ApplyForce(glm::vec3 force, glm::vec3 relPos = glm::vec3(0.0f)) = 0;
		virtual void ApplyImpulse(glm::vec3 force, glm::vec3 relPos = glm::vec3(0.0f)) = 0;
		virtual void SetGravity(glm::vec3 gravity) = 0;


	protected:
		iRigidBody(){}
		iRigidBody(const iRigidBody& other){}
		iRigidBody& operator=(const iRigidBody& other) { return *this; }


	};
}