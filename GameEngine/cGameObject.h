#ifndef _cGameObject_HG_
#define _cGameObject_HG_

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#define GLM_ENABLE_EXPERIMENTAL		
#include <glm/gtx/quaternion.hpp>	
#include <string>
#include <vector>
#include <Interfaces/iRigidBody.h>
#include <Interfaces/iSoftBody.h>
#include <Interfaces/iVehicle.h>

#include "DebugRenderer/iDebugRenderer.h"
#include "cAnimationController.h"


class cSimpleAssimpSkinnedMesh;	// Forward declare
class cAnimationState;			// Forward declare 

struct sAnimation
{
	std::string AnimName;
	bool hasExitTime;
	//duaration in sec
	float duration;
};


struct sSphere
{
	sSphere(float theRadius)
	{
		this->radius = theRadius;
	}
	float radius;
};
struct sTriangle
{
	glm::vec3 v[3];
};
struct sAABB
{
	glm::vec3 minXYZ;
	glm::vec3 maxXYZ;
};

struct sAOB
{
	glm::vec3 minXYZ;
	glm::vec3 maxXYZ;
	glm::mat3 matOrientation;
};

struct sTextureInfo
{
	sTextureInfo()
	{
		textureTileMult = 1.0f;
	};
	std::string name;
	int cachedTextureID; 	// Set to -1 by default
	float strength;
	float textureTileMult;
};


struct sCurAnim
{

	std::string name;
	bool hasExitTime;
	bool IsFinished;
};


class cGameObject
{
public:
	cGameObject();

private:
	
public:
	glm::quat m_meshQOrientation;
	glm::quat getQOrientation(void) { return this->m_meshQOrientation; };

	glm::vec3 getMeshOrientationEulerAngles(bool bIsDegrees = false);

	void setMeshOrientationEulerAngles(glm::vec3 newAnglesEuler, bool bIsDegrees = false);
	void setMeshOrientationEulerAngles(float x, float y, float z, bool bIsDegrees = false);

	void adjMeshOrientationEulerAngles(glm::vec3 adjAngleEuler, bool bIsDegrees = false);
	void adjMeshOrientationEulerAngles(float x, float y, float z, bool bIsDegrees = false);

	void adjMeshOrientationQ(glm::quat adjOrientQ);
	void setQOrientation(glm::quat newOrientation){ this->m_meshQOrientation = newOrientation; }


	glm::vec3 getRelPos(glm::vec3 posInModelSpace);

	glm::vec3 getForward(glm::vec3 forwardModelSpace = glm::vec3(0.0f, 0.0f, 1.0f));

	void setUniformScale(float scale);
	glm::vec3 nonUniformScale;
	glm::mat4 matTransform;
	std::string meshName;	// Model to draw

	
	bool bIsWireFrame;	//  = true;  C++
	bool bIsVisible;
	bool bIsDebug;
	bool bHadCollision;
	bool bFBO;
	bool bIsPlayer;
	bool bReflection;
	bool bRefraction;
	bool bExtentsFromMesh;
	bool bSave;
	bool bUseHeighMap;
	bool bIsGhost;
	bool bUseNormalMap;
	bool isDead;
	bool bIsProjectile;
	bool bIsImposter;
	bool bUseLOD;

	// ignore this for now...
	// Around its own axis (NOT the origin)
	//glm::vec3 orientation;

//	glm::vec3 objColour;
	glm::vec4 materialDiffuse;		// RGB+Alpha
	void setDiffuseColour(glm::vec3 newDiffuse);
	void setAlphaTransparency(float newAlpha);
	glm::vec4 materialSpecular;		// RGB+specular power
	void setSpecularColour(glm::vec3 colourRGB);
	void setSpecularPower(float specPower);

	bool bUseVertexColour;		// = false (default)

	bool bDontLight;		// If true, just object colour is used

	// Child objects inside this one
	std::vector< cGameObject* > vec_pChildObjectsToDraw;

	// Things we can use to find this object 
	std::string friendlyName;		// Human "Bob" "Ali" "Player"

	// Things that are updated by physics
	glm::vec3 position;
	glm::vec3 InitPosition;
	glm::vec3 velocity;
	glm::vec3 accel;		// acceleration

	bool bIsUpdatedByPhysics;	// Phsyics "owns" the position	

	nPhysics::iRigidBody* rigidBody;
	nPhysics::iVehicle* Vehicle;
	float mass;
	float inverseMass;
	
	

	// Assume that everything is a sphere...
	enum eShape
	{
		UNKOWN_SHAPE,
		SPHERE,
		TRIANGLE,
		AABB, AOB	// DON'T use these	
		// PLANE, AABB, etc., etc.
	};
	// Allows me to point to anything (any type)
	void* pTheShape;		// Details
	eShape shapeType;		// What shape it is

	//float radius;

	// 
	iDebugRenderer* pDebugRenderer;
	void Update(double deltaTime);

	// In theory, it's faster. 
	// Not likely. Suggestion, not a command
	inline unsigned int getUniqueID(void)
	{
		return this->m_uniqueID;
	}

	nPhysics::iSoftBody* softBody;

	// Textures...
	std::vector<sTextureInfo> vecTextures;
	

	cSimpleAssimpSkinnedMesh*	pSimpleSkinnedMesh;
	// HACK
	std::string currentAnimation;
	std::string prevAnimation;
	cAnimationState*	pAniState;
	cAnimationController* pAnimController;

	// Extent Values for skinned mesh
	// These can be updated per frame, from the "update skinned mesh" call
	glm::vec3 minXYZ_from_SM_Bones;
	glm::vec3 maxXYZ_from_SM_Bones;
	// Store all the bones for this model, being updated per frame
	std::vector< glm::mat4x4 > vecObjectBoneTransformation;


private:
	unsigned int m_uniqueID;			// Number that's unique to this instance
	// static:
	// - There's only 1.
	// - All classes "see" it
	static unsigned int m_NextID; //= 0;	Can't assign as is static

	static const unsigned int STARTING_ID_VALUE = 1000;



};

#endif 
