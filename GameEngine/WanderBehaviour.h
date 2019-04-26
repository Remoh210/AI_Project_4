#ifndef WANDER_BEHAVIOUR_H
#define WANDER_BEHAVIOUR_H

#include "Behaviour.h"
#include "cGameObject.h"

class WanderBehaviour : public Behaviour {
public:
	WanderBehaviour(cGameObject* agent, float maxSpeed, float maxForce, float timeToWait, glm::vec3 RelPos, float upLim, float dwLim);
	virtual ~WanderBehaviour(void);

	virtual void update(float dt);

private:
	//cMeshObject* mTarget;
	cGameObject* mAgent;
	float mMaxSpeed;
	float mMaxForce; //steering speed
	float mUpLim;
	float mDwnLim;
	float mTimeToWait;
	bool pusuePlayer;
	float mTimeWaitedSoFar;
	bool mStart;
	glm::vec3 mCurTarget;
	glm::vec3 mRelPos;
};

#endif
