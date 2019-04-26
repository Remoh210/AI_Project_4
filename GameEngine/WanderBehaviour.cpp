#include "WanderBehaviour.h"
//#include <GLFW/glfw3.h>
#include <iostream>
#include "globalStuff.h"

float map(float value, float minA, float maxA, float minB, float maxB) {
	return (1 - ((value - minA) / (maxA - minA))) * minB + ((value - minA) / (maxA - minA)) * maxB;
}
float RandomFloat(float a, float b);
WanderBehaviour::WanderBehaviour(cGameObject* agent, float maxSpeed, float maxForce, float timeToWait, glm::vec3 RelPos, float upLim, float dwLim)
	: mAgent(agent)
	//, mTarget(target)
	, mMaxSpeed(maxSpeed)
	, mMaxForce(maxForce)
	, mTimeToWait(timeToWait)
	, mRelPos(RelPos)
	, mUpLim(upLim)
	, mDwnLim(dwLim)
{
	mStart = true;
	mCurTarget = glm::vec3(mRelPos.x + RandomFloat(mDwnLim, mUpLim), 0.0f, mRelPos.z + RandomFloat(mDwnLim, mUpLim));
	mTimeWaitedSoFar = 0.0f;
}

WanderBehaviour::~WanderBehaviour(void)
{
}

void WanderBehaviour::update(float dt)
{
	//float curTime = 3.1f;
	if (mAgent == 0) return;

	glm::vec3 fromMesh = mAgent->position;
	//glm::vec3 to = mAgent->position + mAgent->getForward() * 20.0f;
	glm::vec3 toGround = glm::vec3(mAgent->position.x, mAgent->position.y - 5.0f, mAgent->position.z);
	if (gPhysicsWorld->RayCast(fromMesh, toGround))
	{
		mAgent->rigidBody->SetPosition(glm::vec3(mAgent->position.x, mAgent->position.y += 0.1f, mAgent->position.z));
	}
	
	g_pDebugRendererACTUAL->addLine(fromMesh, toGround, glm::vec3(1.f, 0.f, 0.f));



	//LeftSensors
	glm::vec3 fromLeft = mAgent->position;
	//glm::vec3 to = mAgent->position + mAgent->getForward() * 20.0f;
	glm::vec3 toLeft = mAgent->getRelPos(glm::vec3(12.0f, 0.0f, 15.0f));
	
	g_pDebugRendererACTUAL->addLine(fromLeft, toLeft, glm::vec3(1.f, 0.f, 0.f));


	//LeftSensors
	glm::vec3 fromRight = mAgent->position;
	//glm::vec3 to = mAgent->position + mAgent->getForward() * 20.0f;
	glm::vec3 toRight = mAgent->getRelPos(glm::vec3(-12.0f, 0.0f, 15.0f));
	
	g_pDebugRendererACTUAL->addLine(fromRight, toRight, glm::vec3(1.f, 0.f, 0.f));



	float factor = 1.0f;
	//mCurTarget = mAgent->getForward() * 20.0f;
	glm::vec3 desired = mCurTarget - mAgent->position;
	if (gPhysicsWorld->RayCast(fromRight, toRight))
	{
		//desired = toLeft - mAgent->position;
		//factor = 2.0f;
		mAgent->adjMeshOrientationEulerAngles(0.0f, 0.01f, 0.0f);
	}
	else if (gPhysicsWorld->RayCast(fromLeft, toLeft))
	{
		mAgent->adjMeshOrientationEulerAngles(0.0f, -0.01f, 0.0f);
		//desired = toRight - mAgent->position;
		//factor = 2.0f;
	}
	//else
	//{
	//	
	//}

	desired = mAgent->getForward() * 20.0f;
	//normalize it and scale by mMaxSpeed
	desired = glm::normalize(desired) * mMaxSpeed;
	float d = glm::length(desired);
	if (d < 10.0f) {
		float m = map(d, 0, 10, 0, mMaxSpeed);
		desired = glm::normalize(desired) * m;
	}
	else {
		desired = glm::normalize(desired) * mMaxSpeed;
	}

	glm::vec3 steering = desired - mAgent->rigidBody->GetVelocity();

	if (glm::length(steering) > mMaxForce) {
		steering = glm::normalize(steering) * mMaxForce;
	}
	//steering = glm::normalize(desired) * mMaxForce;
	//mAgent->rigidBody->SetVelocity(steering);

	mAgent->rigidBody->ApplyImpulse(steering * factor);

	//glm::mat4 rot = glm::inverse(glm::lookAt(mAgent->position, mAgent->position + mAgent->rigidBody->GetVelocity(), glm::vec3(0.0f, 1.0f, 0.0f)));
	//mAgent->rigidBody->SetMatRotation(rot);
	//
	




}


float RandomFloat(float a, float b) {
	//srand(glfwGetTime());
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = b - a;
	float r = random * diff;
	return a + r;
}
