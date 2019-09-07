#ifndef _globalStuff_HG_
#define _globalStuff_HG_


#include "globalOpenGLStuff.h"
#include "Camera.h"
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include "cAABB.h"
#include "cTextRend.h"

#include <string>

#include "sPlyVertex.h"
#include "sPlyTriangle.h"
#include "cGameObject.h"
#include "sModelDrawInfo.h"
#include "cVAOMeshManager.h"
#include "cLuaBrain.h"
#include "cLightManager.h"
#include "cSceneManager.h"
#include <Interfaces/iPhysicsFactory.h>

#include "Error/CErrorLog.h"
#include "TextureManager/cBasicTextureManager.h"
#include "DebugRenderer/iDebugRenderer.h"
#include "DebugRenderer/cDebugRenderer.h"
#include "cCharacterManager.h"
// The commands
#include "cCommandGroup.h"
// And the commands
#include "cFollowObjectCommand.h"
#include "cMoveToCommand.h"
#include "cMoveToEase.h"
#include "cMoveToEaseIO.h"
#include "cSimpleDebugRenderer.h"
#include "cSoundManager.h"

#include "cSimpleAssimpSkinnedMeshLoader_OneMesh.h"
// ... and more
extern GLuint program;

extern HINSTANCE hGetProckDll;
typedef nPhysics::iPhysicsFactory*(*f_createPhysicsFactory)();

extern nPhysics::iPhysicsFactory* gPhysicsFactory;
extern nPhysics::iPhysicsWorld* gPhysicsWorld;


enum ePhysics
{
	SIMPLE,
	BULLET,
	UNKNOWN
};
extern ePhysics physics_library;

extern nPhysics::iPhysicsFactory* gPhysicsFactory;
extern nPhysics::iPhysicsWorld* gPhysicsWorld;

extern bool bMouseInWindow;
extern bool bIsDebugMode;
extern cBasicTextureManager* g_pTheTextureManager;

//extern cCommandGroup* luaCommandGroup;
extern cCommandGroup sceneCommandGroup;
extern std::vector<cAABB::sAABB_Triangle> vec_cur_AABB_tris;
//extern std::vector<cGameObject*> vec_pObjectsToDraw;

extern glm::vec3 g_CameraEye;	// = glm::vec3( 0.0, 0.0, +10.0f );
//extern glm::vec3 g_CameraAt;	// = glm::vec3( 0.0, 0.0, 0.0f );
extern double deltaTime;
//Global screen size
extern unsigned int SCR_WIDTH;
extern unsigned int SCR_HEIGHT;
extern unsigned int gSuspensionStiffness;
extern int gFogDensity;
extern glm::vec3 g_Gravity;

extern cFBO* g_pFBOMain;
extern cSoundManager* g_pSoundManager;
extern cVAOMeshManager* g_pTheVAOMeshManager;
extern std::vector< cGameObject* > vec_pObjectsToDraw;
extern std::vector< cGameObject* > vec_controlable;
extern cSceneManager* g_pSceneManager;
extern cLightManager* LightManager;
extern cTextRend* g_textRenderer;
extern cSimpleDebugRenderer* g_simpleDubugRenderer;
extern cDebugRenderer* g_pDebugRendererACTUAL;
extern cCharacterManager* g_pCharacterManager;

extern cSimpleAssimpSkinnedMesh* g_pRPGSkinnedMesh; //= NULL;
// 

// Signature for the ply loader function
bool LoadPlyFileData( std::string fileName );

void LoadModelTypes( cVAOMeshManager* pTheVAOMeshManager, GLuint shaderProgramID );
void CreateModels(std::string filename, cVAOMeshManager* pTheVAOMeshManager, GLuint shaderProgramID);
void LoadModelsIntoScene( std::vector<cGameObject*> &vec_pObjectsToDraw );


void loadCameraInfo(std::string filename);
void saveCameraInfo(std::string filename);

cGameObject* findObjectByFriendlyName(std::string theNameToFind);
cGameObject* findObjectByUniqueID( unsigned int IDToFind );
extern cLuaBrain* p_LuaScripts;

void saveLightInfo(std::string filename, std::vector<sLight*> lights);

void saveModelInfo(std::string filename, std::vector<cGameObject*> models);

void loadModels(std::string filename, std::vector<cGameObject*> models);

void loadLights(std::string filename, std::vector<sLight*> lights);

void DrawObject( cGameObject* pCurrentMesh,
				 glm::mat4x4 &matModel, 
				 GLuint shaderProgramID, cFBO* fbo = NULL);
void DrawScene_Simple(std::vector<cGameObject*> vec_pMeshSceneObjects,
	GLuint shaderProgramID,
	unsigned int passNumber, cFBO* fbo = NULL);

void LoadSkinnedMeshModel(std::vector<cGameObject*> &vec_pObjectsToDraw,
	unsigned int shaderProgramID);


// *****************************************************************
// This is part of the physics stuff
// (really should be moved to the physics area, not here)
struct sClosestPointData
{
	glm::vec3 thePoint;
	unsigned int triangleIndex;
};
void CalculateClosestPointsOnMesh( sModelDrawInfo theMeshDrawInfo, 
								   glm::vec3 pointToTest, 
								   std::vector<sClosestPointData> &vecPoints );
// *****************************************************************


//https://stackoverflow.com/questions/9878965/rand-between-0-and-1
template <class T>
T getRandBetween0and1(void)
{
	return (T) ((double) rand() / (RAND_MAX)) ;
}

// Inspired by: https://stackoverflow.com/questions/686353/c-random-float-number-generation
template <class T>
T getRandInRange( T min, T max )
{
	double value = 
		min + static_cast <double> (rand()) 
		/ ( static_cast <double> (RAND_MAX/(static_cast<double>(max-min) )));
	return static_cast<T>(value);
}

extern iDebugRenderer* g_pDebugRenderer;

#endif	// _globalStuff_HG_
