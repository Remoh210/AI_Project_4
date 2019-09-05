//     ___                 ___ _
//    / _ \ _ __  ___ _ _ / __| |
//   | (_) | '_ \/ -_) ' \ (_ | |__
//    \___/| .__/\___|_||_\___|____|
//         |_|
//
#include "globalOpenGLStuff.h"
#include "globalStuff.h"

#include "Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <iostream>
#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "BehaviourManager.h"
#include "WanderBehaviour.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <Interfaces/iRigidBody.h>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "cGameObject.h"
#include "cShaderManager.h"
#include "cSoundManager.h"
#include "cVAOMeshManager.h"
#include <algorithm>
#include <windows.h>

#include "DebugRenderer/cDebugRenderer.h"
#include "PostEffect.h"
#include "cLightHelper.h"

// TEST
glm::quat RotationBetweenVectors(glm::vec3 start, glm::vec3 dest);

// Dll
HINSTANCE hGetProckDll = 0;
// typedef nPhysics::iPhysicsFactory*(*f_createPhysicsFactory)();
ePhysics physics_library = UNKNOWN;

nPhysics::iPhysicsFactory *gPhysicsFactory = NULL;
nPhysics::iPhysicsWorld *gPhysicsWorld = NULL;

glm::vec3 g_Gravity = glm::vec3(0.0f, -1.0f, 0.0f);

GLuint program;
cDebugRenderer *g_pDebugRendererACTUAL = NULL;
iDebugRenderer *g_pDebugRenderer = NULL;
cSimpleDebugRenderer *g_simpleDubugRenderer = NULL;
cSoundManager *g_pSoundManager = NULL;
cLuaBrain *p_LuaScripts = NULL;
cTextRend *g_textRenderer = NULL;
cCharacterManager *g_pCharacterManager = NULL;

// cCommandGroup sceneCommandGroup;
int cou;
int nbFrames = 0;
int FPS = 0;
int gFogDensity = 10.0f;
int gameCounter = 0;

bool RayHitted = false;
std::vector<cAABB::sAABB_Triangle> vec_cur_AABB_tris;
void UpdateWindowTitle(void);
double currentTime = 0;
double deltaTime = 0;
double FPS_last_Time = 0;
bool bIsDebugMode = false;
bool bFullScreen = false;

// for collision
float time = 0.0f;
bool collided = false;
bool collidedA = false;
bool collidedB = false;
bool collidedC = false;
bool collidedD = false;
// for collision
std::vector<cGameObject *> vec_pObjectsToDraw;
// for physics
std::vector<cGameObject *> vec_controlable;
unsigned int numberOfObjectsToDraw = 0;



unsigned int SCR_WIDTH = 1000;
unsigned int SCR_HEIGHT = 900;
std::string title = "Default";
std::string scene = "Scene1.json";

Camera camera(glm::vec3(0.0f, 0.0f, 0.0f));

bool distToCam(cGameObject *leftObj, cGameObject *rightObj) {
  return glm::distance(leftObj->position, camera.Position) >
         glm::distance(rightObj->position,
                       camera.Position); // here go your sort conditions
}

std::vector<cGameObject *> vec_sorted_drawObj;

glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 g_CameraEye = glm::vec3(0.0, 0.0, 250.0f);

cShaderManager *pTheShaderManager = NULL;
cVAOMeshManager *g_pTheVAOMeshManager = NULL;
cSceneManager *g_pSceneManager = NULL;
cLightManager *LightManager = NULL;

std::vector<cGameObject *> vec_transObj;
std::vector<cGameObject *> vec_non_transObj;

cBasicTextureManager *g_pTheTextureManager = NULL;

static void error_callback(int error, const char *description) {
  fprintf(stderr, "Error: %s\n", description);
}
glm::mat4 portal_view(glm::mat4 orig_view, cGameObject *src, cGameObject *dst);
cAABBHierarchy *g_pTheTerrain = new cAABBHierarchy();

bool loadConfig();
cFBO *g_pFBOMain;

cFBO *FBO_Portal1;
cFBO *FBO_Portal2;

nPhysics::iRigidBody *bodyHit = NULL;

// Set up the off screen textures to draw to
GLuint g_FBO = 0;
GLuint g_FBO_colourTexture = 0;
GLuint g_FBO_depthTexture = 0;
GLint g_FBO_SizeInPixes = 512; // = 512 the WIDTH of the framebuffer, in pixels;




int main(void) {
#pragma region initialization
  loadConfig();

  GLFWwindow *window;

  glfwSetErrorCallback(error_callback);

  if (!glfwInit()) {
    exit(EXIT_FAILURE);
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

  if(bFullScreen)
  {
	  window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, title.c_str(), glfwGetPrimaryMonitor(), NULL);
  }
  else
  {
	  window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, title.c_str(), NULL, NULL);
  }
  
  if (!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetKeyCallback(window, key_callback);

  glfwMakeContextCurrent(window);
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
  glfwSwapInterval(1);

  pTheShaderManager = new cShaderManager();
  pTheShaderManager->setBasePath("assets/shaders/");

  cShaderManager::cShader vertexShader;
  cShaderManager::cShader fragmentShader;
  cShaderManager::cShader geometryShader;

  vertexShader.fileName = "vertex01.vert";
  vertexShader.shaderType = cShaderManager::cShader::VERTEX_SHADER;

  fragmentShader.fileName = "fragment01.frag";
  fragmentShader.shaderType = cShaderManager::cShader::FRAGMENT_SHADER;

  geometryShader.fileName = "geometry.glsl";
  geometryShader.shaderType = cShaderManager::cShader::GEOMETRY_SHADER;

  if (pTheShaderManager->createProgramFromFile(
          "BasicUberShader", 
			vertexShader,
			geometryShader,	
          fragmentShader)) { // Shaders are OK
    std::cout << "Compiled shaders OK." << std::endl;
  } else {
    std::cout << "OH NO! Compile error" << std::endl;
    std::cout << pTheShaderManager->getLastError() << std::endl;
  }

  // Load the uniform location values (some of them, anyway)
  cShaderManager::cShaderProgram *pSP =
      ::pTheShaderManager->pGetShaderProgramFromFriendlyName("BasicUberShader");
  pSP->LoadUniformLocation("texture00");
  pSP->LoadUniformLocation("texture01");
  pSP->LoadUniformLocation("texture02");
  pSP->LoadUniformLocation("texture03");
  pSP->LoadUniformLocation("texture04");
  pSP->LoadUniformLocation("texture05");
  pSP->LoadUniformLocation("texture06");
  pSP->LoadUniformLocation("texture07");
  pSP->LoadUniformLocation("texBlendWeights[0]");
  pSP->LoadUniformLocation("texBlendWeights[1]");

  program = pTheShaderManager->getIDFromFriendlyName("BasicUberShader");

  ::g_pTheVAOMeshManager = new cVAOMeshManager();
  ::g_pTheVAOMeshManager->SetBasePath("assets/models");
  ::g_pTheTextureManager = new cBasicTextureManager();
  ::g_textRenderer = new cTextRend();
  ::g_pSceneManager = new cSceneManager();
  ::g_pSoundManager = new cSoundManager();
  ::g_pSceneManager->setBasePath("scenes");
  ::LightManager = new cLightManager();
  ::g_pFBOMain = new cFBO();

  FBO_Portal1 = new cFBO();
  FBO_Portal2 = new cFBO();

  std::string FBOErrorString;
  if (::g_pFBOMain->init(800, 600, FBOErrorString)) {
    std::cout << "Framebuffer 1 is good to go!" << std::endl;
  } else {
    std::cout << "Framebuffer 1 is NOT complete" << std::endl;
  }

  if (FBO_Portal1->init(800, 600, FBOErrorString)) {
    std::cout << "Framebuffer 2 is good to go!" << std::endl;
  } else {
    std::cout << "Framebuffer 2 is NOT complete" << std::endl;
  }

  if (FBO_Portal2->init(800, 600, FBOErrorString)) {
    std::cout << "Framebuffer 3 is good to go!" << std::endl;
  } else {
    std::cout << "Framebuffer 3 is NOT complete" << std::endl;
  }

  // Set Up FBO
  static const GLenum draw_bufers[] = {GL_COLOR_ATTACHMENT0};
  glDrawBuffers(1, draw_bufers);

  // Check for "completenesss"
  GLenum FBOStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);

  if (FBOStatus == GL_FRAMEBUFFER_COMPLETE) {
    std::cout << "Framebuffer is good to go!" << std::endl;
  } else {
    std::cout << "Framebuffer is NOT complete" << std::endl;
  }

  // Point back to default frame buffer
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // Loading the uniform variables here (rather than the inner draw loop)
  GLint objectColour_UniLoc = glGetUniformLocation(program, "objectColour");
  GLint matModel_location = glGetUniformLocation(program, "matModel");
  GLint matView_location = glGetUniformLocation(program, "matView");
  GLint matProj_location = glGetUniformLocation(program, "matProj");
  GLint eyeLocation_location = glGetUniformLocation(program, "eyeLocation");

  GLint fogDensity = glGetUniformLocation(program, "FogDensity");

  ::g_pDebugRendererACTUAL = new cDebugRenderer();
  ::g_pDebugRenderer = (iDebugRenderer *)::g_pDebugRendererACTUAL;

  if (!::g_pDebugRendererACTUAL->initialize()) {
    std::cout << "Warning: couldn't init the debug renderer." << std::endl;
    std::cout << "\t" << ::g_pDebugRendererACTUAL->getLastError() << std::endl;
  } else {
    std::cout << "Debug renderer is OK" << std::endl;
  }
  glm::mat4x4 matProjection = glm::mat4(1.0f);
  glm::mat4x4 matView = glm::mat4(1.0f);

  // Physics Initialization
  hGetProckDll = LoadLibraryA("BulletPhysics.dll");
  physics_library = BULLET;
  f_createPhysicsFactory CreatePhysicsFactory =
      (f_createPhysicsFactory)GetProcAddress(hGetProckDll, "CreateFactory");
  gPhysicsFactory = CreatePhysicsFactory();
  gPhysicsWorld = gPhysicsFactory->CreatePhysicsWorld();
  gPhysicsWorld->SetGravity(g_Gravity);

  LoadSkinnedMeshModel(::vec_pObjectsToDraw, program);
  g_pSoundManager->InitFmod();

  LoadModelTypes(::g_pTheVAOMeshManager, program);
  ::g_pSceneManager->loadScene(scene);
  ::LightManager->LoadUniformLocations(program);
  g_pSoundManager->loadSounds("scenes/sound.json");
  LoadModelsIntoScene(::vec_pObjectsToDraw);
  g_simpleDubugRenderer =
      new cSimpleDebugRenderer(findObjectByFriendlyName("DebugCapsule"),
                               findObjectByFriendlyName("DebugSphere"),
                               findObjectByFriendlyName("DebugCube"), program);
  g_pCharacterManager = new cCharacterManager(vec_pObjectsToDraw);
  g_pCharacterManager->setActiveChar("chan");


 
 
  // for (unsigned int objIndex = 0;
  //	objIndex != (unsigned int)vec_pObjectsToDraw.size();
  //	objIndex++)
  //{
  //	cGameObject* pCurrentMesh = vec_pObjectsToDraw[objIndex];
  //	if (pCurrentMesh->materialDiffuse.a < 1.0f) {
  // vec_transObj.push_back(pCurrentMesh); } 	else {
  // vec_non_transObj.push_back(pCurrentMesh); }

  //}//for ( unsigned int objIndex = 0;

  double lastTime = glfwGetTime();

  cLightHelper *pLightHelper = new cLightHelper();

  // Lua
  //::p_LuaScripts = new cLuaBrain();
  //::p_LuaScripts->SetObjectVector(&(::vec_pObjectsToDraw));
  //::p_LuaScripts->LoadScriptFile("example.lua");

  // FBO
  int renderPassNumber = 1;
  // 1 = 1st pass (the actual scene)
  // 2 = 2nd pass (rendering what we drew to the output)
  GLint renderPassNumber_UniLoc =
      glGetUniformLocation(program, "renderPassNumber");

#pragma endregion
  using namespace PostEffect;
  ScreenQuad screen_quad(SCR_WIDTH, SCR_HEIGHT);

  const float bloom_downscale_factor = 0.35f;
  BloomEffect bloom_effect(screen_quad, bloom_downscale_factor);

  ChromAberrationEffect ChromAbEffect(screen_quad);


  RenderTarget scene_texture(SCR_WIDTH, SCR_HEIGHT);
  TextureRenderTarget bloom_result(SCR_WIDTH, SCR_HEIGHT);
  TextureRenderTarget ChromAberrationEffect(SCR_WIDTH, SCR_HEIGHT);




  float bloom_strength = 10.0f;
  int bloom_blur_iterations = 5;
  float bloom_threshold = 0.58f;



  for (int i = 0; i < vec_pObjectsToDraw.size(); i++) 
  {
	  cGameObject* go = vec_pObjectsToDraw[i];

	  go->InitPosition = go->position;

	  if (go->bSave)
	  {
		  vec_controlable.push_back(go);
	  }
	  
  }










  BehaviourManager* behavManager = new BehaviourManager();

  for (int i = 0; i < vec_pObjectsToDraw.size(); i++)
  {
	  if (vec_pObjectsToDraw[i]->rigidBody != NULL)
	  {
		  if (vec_pObjectsToDraw[i]->rigidBody->GetMass() > 0.0f)
		  {
			  //std::rand() % 100 + 50;
			  //WanderBehaviour* wander1 = new WanderBehaviour(vec_pObjectsToDraw[i], 60 /*(float)(std::rand() % 90 + 40)*/, 220.2f, 0.0f, glm::vec3(0.0f), 100.0f, -100.0f); //(Agent, Target, maxSpeed, WanderOrigin , UpLimit, DownLimit)
			  //behavManager->SetBehaviour(vec_pObjectsToDraw[i], wander1);
		  }
	  }
  }







  
  // Draw the "scene" (run the program)
  while (!glfwWindowShouldClose(window)) {
	  ::pTheShaderManager->useShaderProgram("BasicUberShader");

     


     
#pragma region main render
	  
		 

		 glUniform1f(fogDensity, gFogDensity/10000.0f);

		 RenderTarget::State render_target_state(scene_texture);
        // Set for the 1st pass

        //**********************************************************
        // Clear the offscreen frame buffer
        glViewport(0, 0, g_FBO_SizeInPixes, g_FBO_SizeInPixes);
        GLfloat zero = 0.0f;
        GLfloat one = 1.0f;
        glClearBufferfv(GL_COLOR, 0, &zero);
        glClearBufferfv(GL_DEPTH, 0, &one);
        //**********************************************************



		float ratio;
		int width, height;;

        matProjection = glm::mat4(1.0f);
        matView = glm::mat4(1.0f);

        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float)height;
        glViewport(0, 0, width, height);

        glEnable(GL_DEPTH);      // Enables the KEEPING of the depth information
        glEnable(GL_DEPTH_TEST); // When drawing, checked the existing depth
        glEnable(GL_CULL_FACE);  // Discared "back facing" triangles

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        matProjection = glm::perspective(1.0f,      // FOV
                                         ratio,     // Aspect ratio
                                         0.1f,      // Near clipping plane
                                         15000.0f); // Far clipping plane


		glUniform1f(renderPassNumber_UniLoc, 1.0f);


        matView = camera.GetViewMatrix();

        glUniform3f(eyeLocation_location, camera.Position.x, camera.Position.y,
                    camera.Position.z);

        // matView = glm::lookAt( g_CameraEye,	// Eye
        //	                    g_CameraAt,		// At
        //	                    glm::vec3( 0.0f, 1.0f, 0.0f ) );// Up

        glUniformMatrix4fv(matView_location, 1, GL_FALSE,
                           glm::value_ptr(matView));
        glUniformMatrix4fv(matProj_location, 1, GL_FALSE,
                           glm::value_ptr(matProjection));
        LightManager->CopyLightValuesToShader();


		
        DrawScene_Simple(vec_pObjectsToDraw, program, 1);
        
		for (std::vector<sLight *>::iterator it = LightManager->vecLights.begin();
			it != LightManager->vecLights.end(); ++it) {

			sLight *CurLight = *it;
			if (CurLight->AtenSphere == true) {

				cGameObject *pDebugSphere = findObjectByFriendlyName("DebugSphere");
				pDebugSphere->bIsVisible = true;
				pDebugSphere->bDontLight = true;
				glm::vec4 oldDiffuse = pDebugSphere->materialDiffuse;
				glm::vec3 oldScale = pDebugSphere->nonUniformScale;
				pDebugSphere->setDiffuseColour(
					glm::vec3(255.0f / 255.0f, 105.0f / 255.0f, 180.0f / 255.0f));
				pDebugSphere->bUseVertexColour = false;
				pDebugSphere->position = glm::vec3(CurLight->position);
				glm::mat4 matBall(1.0f);

				pDebugSphere->materialDiffuse = oldDiffuse;
				pDebugSphere->setUniformScale(0.1f); // Position
				DrawObject(pDebugSphere, matBall, program);

				const float ACCURACY_OF_DISTANCE = 0.0001f;
				const float INFINITE_DISTANCE = 10000.0f;

				float distance90Percent = pLightHelper->calcApproxDistFromAtten(
					0.90f, ACCURACY_OF_DISTANCE, INFINITE_DISTANCE, CurLight->atten.x,
					CurLight->atten.y, CurLight->atten.z);

				pDebugSphere->setUniformScale(distance90Percent); // 90% brightness
				// pDebugSphere->objColour = glm::vec3(1.0f,1.0f,0.0f);
				pDebugSphere->setDiffuseColour(glm::vec3(1.0f, 1.0f, 0.0f));
				DrawObject(pDebugSphere, matBall, program);



				//			pDebugSphere->objColour = oldColour;
				pDebugSphere->materialDiffuse = oldDiffuse;
				pDebugSphere->nonUniformScale = oldScale;
				pDebugSphere->bIsVisible = false;
			}
		}


#pragma endregion
      
    
    // Post VFX
    {
      

	  bloom_effect.execute(bloom_result, scene_texture.color, bloom_strength, bloom_blur_iterations, bloom_threshold);

	  ChromAbEffect.execute(ChromAberrationEffect, bloom_result.texture);
      
      
  
    
	  //Draw main screen quad
      screen_quad.draw_with_texture(ChromAberrationEffect.texture);
    }
    


#pragma region text_rendering



        double FPS_currentTime = glfwGetTime();
        nbFrames++;
        if (FPS_currentTime - FPS_last_Time >=
            1.0) { // If last prinf() was more than 1 sec ago
          // printf and reset timer
          // printf("%f ms/frame\n", 1000.0 / double(nbFrames));
          FPS = nbFrames * 1;
          nbFrames = 0;
          FPS_last_Time += 1.0;
        }
        g_textRenderer->drawText(screen_quad.width, screen_quad.height,
                                 ("FPS: " + std::to_string(FPS)).c_str());

        //switch (physics_library) {
        //case SIMPLE:
        //  g_textRenderer->drawText(screen_quad.width, screen_quad.height,
        //                           ("Physics: My crappy physics"), 100.0f);
        //  break;
        //case BULLET:
        //  g_textRenderer->drawText(screen_quad.width, screen_quad.height, ("Physics: Bullet"), 100.0f);
        //  break;
        //case UNKNOWN:
        //  break;
        //default:
        //  break;
        //}
        //g_textRenderer->drawText(
        //    screen_quad.width, screen_quad.height,
        //    ("Gravity: " + std::to_string((int)g_Gravity.y)).c_str(), 150.0f);

        std::string strhited;
       /* if (bodyHit)
          strhited = "Ray Hit: " + bodyHit->GetGOName();
        else
          strhited = "Ray Hit: Nothing";*/
		std::string artCount = "Artifacts found : " + std::to_string(gameCounter) + " out of 3";
        //g_textRenderer->drawText(screen_quad.width, screen_quad.height, strhited.c_str(), 100.0f);
		g_textRenderer->drawText(screen_quad.width, screen_quad.height, artCount.c_str(), 150.0f);
		if (gameCounter > 3) {
			g_textRenderer->drawText(screen_quad.width, screen_quad.height, ("Conradulations! You Win!"), 200.0f);
		}
        // g_textRenderer->drawText(width, height,"Ray hit: " + RayHitted ?
        // "no": "yes" , 200.0f);

#pragma endregion

    {
      glm::mat4x4 view_matrix = camera.GetViewMatrix();
      glm::mat4x4 projection_matrix =  glm::perspective(1.0f, screen_quad.width / (float) screen_quad.height, 0.1f, 15000.0f);
      
      ::g_pDebugRendererACTUAL->RenderDebugObjects(view_matrix, projection_matrix,
      deltaTime);
    }

#pragma region Update physics
    // High res timer (likely in ms or ns)
    currentTime = glfwGetTime();
    deltaTime = currentTime - lastTime;

    double MAX_DELTA_TIME = 0.1; // 100 ms
    if (deltaTime > MAX_DELTA_TIME) {
      deltaTime = MAX_DELTA_TIME;
    }
    // update the "last time"
    lastTime = currentTime;

    for (unsigned int objIndex = 0;
         objIndex != (unsigned int)vec_pObjectsToDraw.size(); objIndex++) {
      cGameObject *pCurrentMesh = vec_pObjectsToDraw[objIndex];

      pCurrentMesh->Update(deltaTime);

    } // for ( unsigned int objIndex = 0;



    // New Dll physics
    gPhysicsWorld->Update(deltaTime);


		


	std::string PairF = gPhysicsWorld->GetLastColPair().first;
	std::string PairS = gPhysicsWorld->GetLastColPair().second;

	if (!collidedA) {
		if (PairF == "artifact_1" && PairS == "ghostSphere1" ||
			PairS == "artifact_1" && PairF == "ghostSphere1") {

			collidedA = true;
			cGameObject* artifact = findObjectByFriendlyName("artifact_1");
			artifact->bIsVisible = false;
			gPhysicsWorld->RemoveBody(artifact->rigidBody);
			artifact->rigidBody->~iRigidBody();
			artifact->rigidBody = NULL;
			gameCounter += 1;
		}
	}

	if (!collidedB) {
		if (PairF == "artifact_2" && PairS == "ghostSphere1" ||
			PairS == "artifact_2" && PairF == "ghostSphere1") {

			collidedB = true;
			cGameObject* artifact = findObjectByFriendlyName("artifact_2");
			artifact->bIsVisible = false;
			gPhysicsWorld->RemoveBody(artifact->rigidBody);
			artifact->rigidBody->~iRigidBody();
			artifact->rigidBody = NULL;
			gameCounter += 1;
		}
	}

	if (!collidedC) {
		if (PairF == "artifact_3" && PairS == "ghostSphere1" ||
			PairS == "artifact_3" && PairF == "ghostSphere1") {

			collidedC = true;
			cGameObject* artifact = findObjectByFriendlyName("artifact_3");
			artifact->bIsVisible = false;
			gPhysicsWorld->RemoveBody(artifact->rigidBody);
			artifact->rigidBody->~iRigidBody();
			artifact->rigidBody = NULL;
			gameCounter += 1;
		}
	}

	if (!collidedD) {
		if (PairF == "artifact_4" && PairS == "ghostSphere1" ||
			PairS == "artifact_4" && PairF == "ghostSphere1") {

			collidedD = true;
			cGameObject* artifact = findObjectByFriendlyName("artifact_4");
			
			artifact->bIsVisible = false;
			gPhysicsWorld->RemoveBody(artifact->rigidBody);
			artifact->rigidBody->~iRigidBody();
			artifact->rigidBody = NULL;
			gameCounter += 1;
		}
	}





	if (PairF == "mutant-1" && PairS == "chan" ||
		PairS == "mutant-1" && PairF == "chan") {

		for (int i = 0; i < vec_pObjectsToDraw.size(); i++) {
			vec_pObjectsToDraw[i]->position = vec_pObjectsToDraw[i]->InitPosition;
		}
		gameCounter = 0;
	}
	if (PairF == "mutant-2" && PairS == "chan" ||
		PairS == "mutant-2" && PairF == "chan") {

		for (int i = 0; i < vec_pObjectsToDraw.size(); i++) {
			vec_pObjectsToDraw[i]->position = vec_pObjectsToDraw[i]->InitPosition;
		}

		gameCounter = 0;
	}
	if (PairF == "mutant-3" && PairS == "chan" ||
		PairS == "mutant-3" && PairF == "chan") {

		for (int i = 0; i < vec_pObjectsToDraw.size(); i++) {
			if (vec_pObjectsToDraw[i]->rigidBody != NULL && vec_pObjectsToDraw[i]->rigidBody->GetMass() != 0.0f)
			{
				vec_pObjectsToDraw[i]->rigidBody->SetPosition(vec_pObjectsToDraw[i]->InitPosition);
			}
			
		}
		gameCounter = 0;
	}





    for (int i = 0; i < vec_pObjectsToDraw.size(); i++) {
      cGameObject *curMesh = vec_pObjectsToDraw[i];

      if (curMesh->rigidBody != NULL &&
          curMesh->rigidBody->GetMass() !=
              0.0f) 
	  {
			if (curMesh->Vehicle != NULL)
			{
				//curMesh->Vehicle->AddEngineForce(300.0f);
				//curMesh->Vehicle->
				glm::mat4 mtiden(1.f);
				cGameObject* dbg = findObjectByFriendlyName("brick_wall0");
				dbg->setUniformScale(0.2);
				dbg->setDiffuseColour(glm::vec3(0.6f, 0.f, 0.f));
				dbg->bDontLight = true;
				dbg->position = curMesh->Vehicle->GetWheelPosition(0);

				cGameObject* dbg1 = findObjectByFriendlyName("brick_wall1");
				dbg1->setUniformScale(0.2);
				dbg1->setDiffuseColour(glm::vec3(0.6f, 0.f, 0.f));
				dbg1->bDontLight = true;
				dbg1->position = curMesh->Vehicle->GetWheelPosition(1);

				cGameObject* dbg2 = findObjectByFriendlyName("brick_wall2");
				dbg2->setUniformScale(0.2);
				dbg2->setDiffuseColour(glm::vec3(0.6f, 0.f, 0.f));
				dbg2->bDontLight = true;
				dbg2->position = curMesh->Vehicle->GetWheelPosition(2);

				cGameObject* dbg3 = findObjectByFriendlyName("brick_wall3");
				dbg3->setUniformScale(0.2);
				dbg3->setDiffuseColour(glm::vec3(0.6f, 0.f, 0.f));
				dbg3->bDontLight = true;
				dbg3->position = curMesh->Vehicle->GetWheelPosition(3);


				curMesh->m_meshQOrientation = curMesh->Vehicle->GetChassisRotation();
				curMesh->position = curMesh->Vehicle->GetChassisPositon();
				g_simpleDubugRenderer->drawSphere(glm::vec3(0.f), 80);
			}

			else if (curMesh->rigidBody->GetShape()->GetShapeType() ==
			    nPhysics::SHAPE_TYPE_CAPSULE) 
			{
			  float Totalheight;
			  Totalheight =
			      curMesh->rigidBody->GetShape()->GetCapsuleRadius() + 3.8f;
			  curMesh->position = curMesh->rigidBody->GetPosition();
			  curMesh->position.y =
			      curMesh->rigidBody->GetPosition().y - Totalheight;

			} 
			else {
			  curMesh->position = curMesh->rigidBody->GetPosition();
			  curMesh->m_meshQOrientation =
			      glm::mat4(curMesh->rigidBody->GetMatRotation());
			}
      }
    }
	



    //if (bIsDebugMode) {
    //  // Call the debug renderer
    //  for (int i = 0; i < vec_pObjectsToDraw.size(); i++) {
    //    cGameObject *curObj = vec_pObjectsToDraw[i];
    //    // curObj->bIsVisible = false;
    //    curObj->bDontLight = true;
    //    if (curObj->rigidBody != NULL) {
    //      if (curObj->rigidBody->GetShape()->GetShapeType() ==
    //          nPhysics::SHAPE_TYPE_SPHERE) {

    //        float rad;
    //        curObj->rigidBody->GetShape()->GetSphereRadius(rad);
    //        g_simpleDubugRenderer->drawCube(curObj->rigidBody->GetPosition(),
    //                                        rad);
    //      }

    //      // if (curObj->rigidBody->GetShape()->GetShapeType() ==
    //      // nPhysics::SHAPE_TYPE_PLANE) {
    //      //	//curObj->bIsWireFrame = true;
    //      //	//curObj->bIsVisible = true;
    //      //	//glm::mat4 matIden = glm::mat4(1.0f);
    //      //	//DrawObject(curObj, matIden, program);
    //      //}
    //    }
    //    if (curObj->softBody != NULL) {

    //      glm::vec3 max;
    //      glm::vec3 min;
    //      glm::vec3 center;
    //      curObj->softBody->GetAABB(min, max);
    //      center = (min + max) / 2.0f;
    //      float size = glm::distance(min, max) / 2.0f;
    //      g_simpleDubugRenderer->drawCube(center, size);
    //    }
    //  }
    //} else {
    //  for (int i = 0; i < vec_pObjectsToDraw.size(); i++) {
    //    cGameObject *curObj = vec_pObjectsToDraw[i];
    //    if (!curObj->bIsDebug) {
    //      curObj->bIsVisible = true;
    //      curObj->bIsWireFrame = false;
    //      curObj->bDontLight = false;
    //    }
    //  }
    //}

#pragma endregion
#pragma region whatever






	//Updaters
	//::p_LuaScripts->UpdateCG(deltaTime);
	//::p_LuaScripts->Update(deltaTime);
	behavManager->update(deltaTime);
	g_pSoundManager->Update();



#pragma endregion
    UpdateWindowTitle();
    glfwSwapBuffers(window); // Shows what we drew
    glfwPollEvents();

    ProcessAsynKeys(window);
  } 
  

  // Delete stuff
  delete pTheShaderManager;
  delete ::g_pTheVAOMeshManager;
  delete ::g_pTheTextureManager;

  //
  delete ::g_pDebugRenderer;

  glfwDestroyWindow(window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
}

glm::mat4 portal_view(glm::mat4 orig_view, cGameObject *src, cGameObject *dst) {
  orig_view = glm::translate(orig_view, -camera.Position);
  glm::mat4 mv = orig_view * src->matTransform;
  glm::mat4 portal_cam =
      // 3. transformation from source portal to the camera - it's the
      //    first portal's ModelView matrix:
      mv

      // 2. object is front-facing, the camera is facing the other way:
      * glm::rotate(glm::mat4(1.0), glm::radians(180.0f),
                    glm::vec3(0.0, 1.0, 0.0))
      // 1. go the destination portal; using inverse, because camera
      //    transformations are reversed compared to object
      //    transformations:
      * glm::inverse(dst->matTransform);
  return portal_cam;
}

void UpdateWindowTitle(void) { return; }

cGameObject *findObjectByFriendlyName(std::string theNameToFind) {
  for (unsigned int index = 0; index != vec_pObjectsToDraw.size(); index++) {
    if (vec_pObjectsToDraw[index]->friendlyName == theNameToFind) {
      return vec_pObjectsToDraw[index];
    }
  }

  // Didn't find it.
  return NULL; // 0 or nullptr
}

cGameObject *findObjectByUniqueID(unsigned int ID_to_find) {
  for (unsigned int index = 0; index != vec_pObjectsToDraw.size(); index++) {
    if (vec_pObjectsToDraw[index]->getUniqueID() == ID_to_find) {
      return vec_pObjectsToDraw[index];
    }
  }

  // Didn't find it.
  return NULL; // 0 or nullptr
}

bool loadConfig() {
  rapidjson::Document doc;
  FILE *fp = fopen("config/config.json", "rb"); // non-Windows use "r"
  char readBuffer[65536];
  rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
  doc.ParseStream(is);
  fclose(fp);
  rapidjson::Value Window(rapidjson::kObjectType);
  Window = doc["Window"];

  SCR_WIDTH = Window["Width"].GetInt();
  SCR_HEIGHT = Window["Height"].GetInt();

  title = Window["Title"].GetString();
  if (doc.HasMember("Scene")) {
    scene = doc["Scene"].GetString();
  }
  if (doc.HasMember("Gravity")) {
    const rapidjson::Value &grArray = doc["Gravity"];
    for (int i = 0; i < 3; i++) {
      g_Gravity[i] = grArray[i].GetFloat();
    }

	if (Window.HasMember("FullScreen"))
	{
		bFullScreen = Window["FullScreen"].GetBool();
	}
  }

  return true;

  // std::string language = doc["Language"].GetString();
  // ASSERT_NE(language, "");
  // if (language == "English") { TextRend.setLang(ENGLISH); }
  // else if (language == "Spanish") { TextRend.setLang(SPANISH); }
  // else if (language == "Japanese") { TextRend.setLang(JAPANESE); }
  // else if (language == "Ukrainian") { TextRend.setLang(UKRAINAN); }
  // else if (language == "Polish") { TextRend.setLang(POLSKA); }
}

glm::quat RotationBetweenVectors(glm::vec3 start, glm::vec3 dest) {
  start = glm::normalize(start);
  dest = glm::normalize(dest);

  float cosTheta = dot(start, dest);
  glm::vec3 rotationAxis;

  if (cosTheta < -1 + 0.001f) {
    // special case when vectors in opposite directions:
    // there is no "ideal" rotation axis
    // So guess one; any will do as long as it's perpendicular to start
    rotationAxis = glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), start);
    if (glm::length2(rotationAxis) <
        0.01) // bad luck, they were parallel, try again!
      rotationAxis = glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), start);

    rotationAxis = normalize(rotationAxis);
    return glm::angleAxis(glm::radians(180.0f), rotationAxis);
  }

  rotationAxis = cross(start, dest);

  float s = sqrt((1 + cosTheta) * 2);
  float invs = 1 / s;

  return glm::quat(s * 0.5f, rotationAxis.x * invs, rotationAxis.y * invs,
                   rotationAxis.z * invs);
}
