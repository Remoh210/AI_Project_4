#include "globalOpenGLStuff.h"
#include "globalStuff.h"
#include <string>
#include "Camera.h"
#include "cSceneManager.h"
#include "cAnimationState.h"
#include "GlobalCharacterControlls.h"
//#include <vector>
#include <iostream>
//include "TextureCTextureFromBMP.h"
float speed = 50.0f;
int index = 0;
int SphIndex = 0;
extern sLight* pTheOneLight;	//  = NULL;
extern cLightManager* LightManager;
int lightIndex = 0;
bool firstMouse = true;

void switchVis(cGameObject* obj) {obj->bIsVisible = !obj->bIsVisible;}

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
void SwapThePhysics();
std::string Answer;
void ManageScene(GLFWwindow* window);

bool bIsPicked = false;
cGameObject* closedModel;
bool bMouseInWindow = false;
bool bIsRunning = false;

bool IsPicked = false;
void commandsInterface();
int contrMode = 2;
bool IsShiftDown(GLFWwindow* window);


cGameObject* cloesetObj;
cGameObject* controlledObj;






void pickUP(cGameObject* player)
{
	glm::vec3 from = player->position + glm::vec3(0.0f, 3.0f, 0.0f);
	glm::vec3 to = player->getForward();
	to *= 20.0f;
	to = to + player->position;
	to.y += 8.0f;
	//			to.y -= camera.Position.y;
	if (bIsDebugMode) {
		g_pDebugRendererACTUAL->addLine(from, to, glm::vec3(1.0f, 1.0f, 0.0f));
	}
	nPhysics::iRigidBody* hitRb = gPhysicsWorld->RayCastGetObject(from, to);

	if (hitRb && hitRb->GetMass() < 3000.0f)
	{
		//Get Direction
		//to.y = 20.0f;
		glm::vec3 dir = to - hitRb->GetPosition();
		dir = glm::normalize(dir);


		if (glm::distance(hitRb->GetPosition(), to) > 2.0f)
		{
			hitRb->SetVelocity(dir * 35.0f);

		}
		else
		{
			hitRb->SetVelocity(glm::vec3(0.0f));
		}

	}
}



void pickBow(cGameObject* player){
	cGameObject* bow = findObjectByFriendlyName("bow");
	if (glm::distance(bow->position, player->position) < 30.0f)
	{
		gPhysicsWorld->RemoveBody(bow->rigidBody);
		//bow->rigidBody->~iRigidBody();
		bow->rigidBody = NULL;

	}
}

void dropBow(cGameObject* player)
{
	cGameObject* bow = findObjectByFriendlyName("bow");

	if(bow->rigidBody == NULL)
	{
		sModelDrawInfo bowInfo;
		bowInfo.meshFileName = "character/bow.ply";
		g_pTheVAOMeshManager->FindDrawInfoByModelName(bowInfo);
		glm::vec3 halfExtents = glm::vec3(bowInfo.maxX * bow->nonUniformScale.x, bowInfo.maxY * bow->nonUniformScale.x,
			bowInfo.maxZ * bow->nonUniformScale.x);

		nPhysics::iShape* CurShape = NULL;
		nPhysics::sRigidBodyDef def;
		//in Radians
		def.Position = bow->position;
		def.Mass = 200.0f;
		def.quatOrientation = bow->m_meshQOrientation;
		def.GameObjectName = bow->friendlyName;

		CurShape = gPhysicsFactory->CreateBoxShape(halfExtents);

		nPhysics::iRigidBody* rigidBody = gPhysicsFactory->CreateRigidBody(def, CurShape);
		bow->rigidBody = rigidBody;
		gPhysicsWorld->AddBody(rigidBody);
		
	}

	
}


void SwitchToWireFrame(std::vector<cGameObject*> models);

void setVelZ(cGameObject* sm, float vel)
{
	glm::vec3 velocity = sm->rigidBody->GetVelocity();
	velocity.z = vel;
	sm->rigidBody->SetVelocity(velocity);
}
void setVelX(cGameObject* sm, float vel)
{
	glm::vec3 velocity = sm->rigidBody->GetVelocity();
	velocity.x = vel;
	sm->rigidBody->SetVelocity(velocity);
}
void setVelY(cGameObject* sm, float vel)
{
	glm::vec3 velocity = sm->rigidBody->GetVelocity();
	velocity.y = vel;
	sm->rigidBody->SetVelocity(velocity);
}
void SwitchToSolid(std::vector<cGameObject*> models);


void key_callback( GLFWwindow* window, 
						  int key, 
						  int scancode, 
						  int action, 
						  int mods)
{

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		
        glfwSetWindowShouldClose(window, GLFW_TRUE);
	}

	if (glfwGetKey(window, GLFW_KEY_SLASH))
	{

		ManageScene(window);
		
	}


	//SAVE MODELS
	if (key == GLFW_KEY_G && action == GLFW_PRESS)
	{
		//for (int i = 0; i < vec_controlable.size(); i++) {
		//	vec_controlable[i]->rigidBody->SetVelocity(vec_controlable[i]->rigidBody->GetVelocity() + glm::vec3(0.0f, 40.0f, 0.0f));
		//}
	}

	//LOAD MODELS
	if (key == GLFW_KEY_H && action == GLFW_PRESS)
	{
		//contrMode = 1;
		//SwitchToWireFrame(vec_pObjectsToDraw);
	}
	

	if (glfwGetKey(window, GLFW_KEY_K))
	{

		
	}

	if (glfwGetKey(window, GLFW_KEY_1))
	{
		contrMode = 1;
		//SwitchToSolid(vec_pObjectsToDraw);
		//findObjectByFriendlyName("Cloth")->softBody->SwitchWind();

	}
	if (glfwGetKey(window, GLFW_KEY_2))
	{
		contrMode = 2;
		//SwitchToSolid(vec_pObjectsToDraw);
		//findObjectByFriendlyName("Cloth")->softBody->SwitchWind();

	}


	if (glfwGetKey(window, GLFW_KEY_4))
	{



	}
	if (glfwGetKey(window, GLFW_KEY_5))
	{
		//contrMode = 2;
		//SwitchToSolid(vec_pObjectsToDraw);
		//findObjectByFriendlyName("Cloth")->softBody->SwitchWind();

	}
	

	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
	{


	}




	//if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	//{
	//	vec_pSpheres[SphIndex]->rigidBody->SetVelocity(vec_pSpheres[SphIndex]->rigidBody->GetVelocity() + glm::vec3(0.0f, 40.0f, 0.0f));
	//	//::g_pSceneManager->saveScene("physics.json");
	//	//g_pDebugRenderer->addDebugSphere(glm::vec3(0.0f), glm::vec3(1.0f, 0.0f, 0.0f), 20, 100.0f);
	//	//::g_pSceneManager->loadScene("output.json");
	//	//CreateModels("Models.txt", g_pTheVAOMeshManager, program);

	//}

	if (glfwGetKey(window, GLFW_KEY_BACKSPACE))
	{
		//SwapThePhysics();
		::g_pSceneManager->saveScene("output.json");
		//::g_pSceneManager->loadScene("scene1.json");
		//CreateModels("Models.txt", g_pTheVAOMeshManager, program);

	}
	//if (glfwGetKey(window, GLFW_KEY_UP))
	//{
	//	g_Gravity.y += 5.0f;
	//	gPhysicsWorld->SetGravity(g_Gravity);

	//}
	//if (glfwGetKey(window, GLFW_KEY_DOWN))
	//{
	//	g_Gravity.y -= 5.0f;
	//	gPhysicsWorld->SetGravity(g_Gravity);

	//}


	//if (glfwGetKey(window, GLFW_KEY_R))
	//{
	//
	//	vec_controlable[index]->nonUniformScale *= 1.2;
	//
	//}
	//if (glfwGetKey(window, GLFW_KEY_Y))
	//{
	//
	//	vec_controlable[index]->nonUniformScale /= 1.2;
	//
	//}
	if (key == GLFW_KEY_T && action == GLFW_PRESS)
	{
		if(vec_controlable[index]->vecTextures.size() > 1)
		{
			vec_controlable[index]->vecTextures[0].strength = 1.0f;
			vec_controlable[index]->vecTextures[1].strength = 0.0f;
		}


		if (index < (vec_controlable.size() - 1)) {
			index = index + 1;
		}
		else { index = 0; }

		if (vec_controlable[index]->vecTextures.size() > 1)
		{
			//vec_pSpheres.at(SphIndex)->vecTextures[0].strength = 0.0f;
			vec_controlable[index]->vecTextures[0].strength = 0.0f;
			vec_controlable[index]->vecTextures[1].strength = 1.0f;
		}


	}



	//cGameObject* pSM = findObjectByFriendlyName("chan");


	//glm::vec3 velocity = pSM->rigidBody->GetVelocity();
	//pSM->currentAnimation = "Idle";
	//velocity.x = 0.0f;
	//velocity.z = 0.0f;

	//pSM->rigidBody->SetVelocity(velocity);
	if (glfwGetKey(window, GLFW_KEY_1))
	{
		//pSM->currentAnimation = "Idle";
		//pSM->currentAnimation =  "assets/modelsFBX/RPG-Character_Unarmed-Idle(FBX2013).fbx";


	}
	if (glfwGetKey(window, GLFW_KEY_2))
	{
		//pSM->currentAnimation = "Walk-forward";
		//setVelZ(pSM, 13.f);
		//pSM->rigidBody->SetVelocity(glm::vec3(0.f, 0.f,20.0f ));

		//pSM->currentAnimation = "Unarmed-Attack-Kick-L1";
		////pSM->currentAnimation = "assets/modelsFBX/RPG-Character_Unarmed-Attack-Kick-L1(FBX2013).FBX";

		//// Add an animation state
		//cAnimationState* pKick = new cAnimationState();
		//pKick->defaultAnimation.name = "Unarmed-Attack-Kick-L1";

	}
	if (glfwGetKey(window, GLFW_KEY_3))
	{
		//pSM->currentAnimation = "Walk-backward";
		//setVelZ(pSM, -13.f);
		//pSM->rigidBody->SetVelocity(glm::vec3(0.f, 0.f, -20.f));
		//pSM->currentAnimation = "assets/modelsFBX/RPG-Character_Unarmed-Fall(FBX2013).fbx";
	}

	

	if (glfwGetKey(window, GLFW_KEY_4))
	{
		
		//pSM->currentAnimation = "Action1";
	}


	if (glfwGetKey(window, GLFW_KEY_7))
	{
		gFogDensity += 1.0f;
	}

	if (glfwGetKey(window, GLFW_KEY_8))
	{
		gFogDensity -= 1.0f;
	}



	//Chose the model
	if (key == GLFW_KEY_M && action == GLFW_PRESS)
	{
		if (index < (vec_controlable.size() - 1)) {
			
			index = index + 1;
		}
		else { index = 0; }
		std::cout << "Model " << vec_controlable.at(index)->friendlyName << " is Chosen" << std::endl;
	}


	//Chose the light
	if (key == GLFW_KEY_N && action == GLFW_PRESS)
	{
		if (lightIndex < (LightManager->vecLights.size() - 1)) {

			lightIndex = lightIndex + 1;
		}
		else { lightIndex = 0; }
		std::cout << "Light " << LightManager->vecLights.at(lightIndex)->lightName << " is Chosen" << std::endl;
	}


	if (key == GLFW_KEY_4 && action == GLFW_PRESS)
	{

		g_pCharacterManager->setActiveChar("chan");
		camera.setThirdPerson(g_pCharacterManager->getActiveChar());

	}

	if (key == GLFW_KEY_5 && action == GLFW_PRESS)
	{

		g_pCharacterManager->setActiveChar("Devil-Mage");
		camera.setThirdPerson(g_pCharacterManager->getActiveChar());
	}
	
	//TURN ON The Light

	if (key == GLFW_KEY_9 && action == GLFW_PRESS)
	{
		LightManager->vecLights.at(lightIndex)->param2.x = 1.0f;
	}
	//TURN OFF The Light
	if (key == GLFW_KEY_0 && action == GLFW_PRESS)
	{
		LightManager->vecLights.at(lightIndex)->param2.x = 0.0f;
	}







	//TURN ON AND OFF SPHERES THAT SHOW LIGHT ATTENUATION

	if (glfwGetKey(window, GLFW_KEY_O))
	{	
		LightManager->vecLights.at(lightIndex)->AtenSphere = true;
	}
	if (glfwGetKey(window, GLFW_KEY_P))
	{
		LightManager->vecLights.at(lightIndex)->AtenSphere = false;
	}


	
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{


		//LightManager->vecLights.at(lightIndex)->AtenSphere = false;
	}


	return;
}


void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
		bMouseInWindow = true;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	if(bMouseInWindow)
	{
		camera.ProcessMouseMovement(xoffset, yoffset);
	}

}


bool IsShiftDown(GLFWwindow* window)
{
	if ( glfwGetKey( window, GLFW_KEY_LEFT_SHIFT ) )	{ return true; }
	if ( glfwGetKey( window, GLFW_KEY_RIGHT_SHIFT ) )	{ return true; }
	// both are up
	return false;
}



bool IsMBLDown(GLFWwindow* window)
{
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)) { return true; }
	return false;
}

bool IsCtrlDown(GLFWwindow* window)
{
	if ( glfwGetKey( window, GLFW_KEY_LEFT_CONTROL ) )	{ return true; }
	if ( glfwGetKey( window, GLFW_KEY_RIGHT_CONTROL ) )	{ return true; }
	// both are up
	return false;
}

bool IsAltDown(GLFWwindow* window)
{
	if ( glfwGetKey( window, GLFW_KEY_LEFT_ALT ) )	{ return true; }
	if ( glfwGetKey( window, GLFW_KEY_RIGHT_ALT ) )	{ return true; }
	// both are up
	return false;
}

bool AreAllModifiersUp(GLFWwindow* window)
{
	if (IsShiftDown(window)) { return false; }

	if ( IsCtrlDown(window) )	{ return false;	} 
	if ( IsAltDown(window) )	{ return false; }

	if (glfwGetKey(window, GLFW_KEY_UP)) { 
		

	}

	return true;
}

void ProcessAsynKeys(GLFWwindow* window)
{
	cGameObject* GO = findObjectByFriendlyName("delorean1");


	if (glfwJoystickPresent(GLFW_JOYSTICK_1))
	{
		int count;
		const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &count);
		const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &count);

		if (std::abs(axes[2]) > 0.2f || std::abs(axes[5]) > 0.2f)
		{
			camera.ProcessJoystickMovement(axes[2] * deltaTime, axes[5] * deltaTime);
		}

		//if (axes[0] > 0.2f)
		//{

		//}


		if (buttons[14] == GLFW_PRESS)
		{
			gSuspensionStiffness++;
			GO->Vehicle->SetSuspensionStiffness(gSuspensionStiffness);
		}

		if (buttons[16] == GLFW_PRESS)
		{
			if (gSuspensionStiffness > 0) {
				gSuspensionStiffness--;
			}
			GO->Vehicle->SetSuspensionStiffness(gSuspensionStiffness);
		}

		GO->Vehicle->SetSteering(-axes[0] * 0.6);

		if (buttons[1] == GLFW_PRESS)
		{
			GO->Vehicle->SetBrakes(20.6);
		}
		else
		{
			GO->Vehicle->SetBrakes(0);
		}

		if (buttons[2] == GLFW_PRESS)
		{
			GO->rigidBody->SetPosition(GO->position + glm::vec3(0.0f, 1.5f, 0.f));
			GO->rigidBody->SetQuatRotation(GO->InitRotaion);
			GO->rigidBody->SetVelocity(glm::vec3(0.0f));
			GO->rigidBody->SetAngularVelocity(glm::vec3(0.0f));
		}

		if (buttons[3] == GLFW_PRESS)
		{
			GO->rigidBody->SetPosition(GO->InitPosition);
			GO->rigidBody->SetQuatRotation(GO->InitRotaion);
			GO->rigidBody->SetVelocity(glm::vec3(0.0f));
			GO->rigidBody->SetAngularVelocity(glm::vec3(0.0f));
		}

		if (axes[3] > 0.0f)
		{
			//std::cout << (axes[3] * 1000) << std::endl;
			//GO->Vehicle->SetBrakes(axes[3] * 1000);
			GO->Vehicle->AddEngineForce(-axes[3] * 600);
		}

		if (axes[4] > 0.0f)
		{

			GO->Vehicle->AddEngineForce(axes[4] * 1000);

		}
		else if (axes[3] > 0.0f)
		{
			//std::cout << (axes[3] * 1000) << std::endl;
			//GO->Vehicle->SetBrakes(axes[3] * 1000);
			GO->Vehicle->AddEngineForce(-axes[3] * 800);
		}
		else
		{
			GO->Vehicle->AddEngineForce(0.f);
		}
	}

	


	const float CAMERA_SPEED_SLOW = 100.0f;
	const float CAMERA_SPEED_FAST = 300.0f;


	float cameraSpeed = CAMERA_SPEED_SLOW;
	if ( glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS  )
	{
		cameraSpeed = CAMERA_SPEED_FAST;
	}

	camera.MovementSpeed = cameraSpeed;
	

	if ( AreAllModifiersUp(window) )
	{


			if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
				camera.ProcessKeyboard(FORWARD, deltaTime);
			if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
				camera.ProcessKeyboard(BACKWARD, deltaTime);
			if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
				camera.ProcessKeyboard(LEFT, deltaTime);
			if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
				camera.ProcessKeyboard(RIGHT, deltaTime);
			if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
				camera.ProcessKeyboard(UP, deltaTime);
			if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
				camera.ProcessKeyboard(DOWN, deltaTime);
	}



	//LIGHT CONTROL*********************************************************************************************************
	if ( IsCtrlDown(window) )
	{
//		if (glfwGetKey(window, GLFW_KEY_W)) {
//
//			//glm::vec3 CamDir = glm::vec3(camera.Front.x, 0.0f, camera.Front.z);
//			//CamDir = glm::normalize(CamDir);
//			glm::vec3 velVec = vec_pSpheres[SphIndex]->rigidBody->GetVelocity();
//			////lets add some speed
//			glm::vec3 CamDir = camera.Front;
//			CamDir = glm::normalize(CamDir);
//			CamDir.y = 0.0f;
//			velVec += CamDir * 200.0f * (float)deltaTime;
//			//velVec.y = 5.0f;
//			vec_pSpheres[SphIndex]->rigidBody->SetVelocity(velVec);
//		}
//		if (glfwGetKey(window, GLFW_KEY_S)) {
//			//glm::vec3 CamDir = glm::vec3(camera.Front.x, 0.0f, camera.Front.z);
//			//CamDir = glm::normalize(CamDir);
//			glm::vec3 velVec = vec_pSpheres[SphIndex]->rigidBody->GetVelocity();
//			////lets add some speed
//			glm::vec3 CamDir = camera.Front;
//			CamDir = glm::normalize(CamDir);
//			CamDir.y = 0.0f;
//			velVec += -CamDir * 200.0f * (float)deltaTime;
//			//velVec.y = 5.0f;
//			vec_pSpheres[SphIndex]->rigidBody->SetVelocity(velVec);
//		}
//
//		if (glfwGetKey(window, GLFW_KEY_D)) {
//			//glm::vec3 CamDir = glm::vec3(camera.Front.x, 0.0f, camera.Front.z);
//			//CamDir = glm::normalize(CamDir);
//			glm::vec3 velVec = vec_pSpheres[SphIndex]->rigidBody->GetVelocity();
//			////lets add some speed
//			glm::vec3 CamDir = camera.Right;
//			CamDir = glm::normalize(CamDir);
//			CamDir.y = 0.0f;
//			velVec += CamDir * 200.0f * (float)deltaTime;
//			//velVec.y = 5.0f;
//			vec_pSpheres[SphIndex]->rigidBody->SetVelocity(velVec);
//		}
//		if (glfwGetKey(window, GLFW_KEY_A)) {
//			//glm::vec3 CamDir = glm::vec3(camera.Front.x, 0.0f, camera.Front.z);
////CamDir = glm::normalize(CamDir);
//			glm::vec3 velVec = vec_pSpheres[SphIndex]->rigidBody->GetVelocity();
//			////lets add some speed
//			glm::vec3 CamDir = camera.Right;
//			CamDir = glm::normalize(CamDir);
//			CamDir.y = 0.0f;
//			velVec += - CamDir * 200.0f * (float)deltaTime;
//			//velVec.y = 5.0f;
//			vec_pSpheres[SphIndex]->rigidBody->SetVelocity(velVec);
//		}
		
		if ( glfwGetKey( window, GLFW_KEY_W ) )	{	LightManager->vecLights.at(lightIndex)->position.z += cameraSpeed * deltaTime;}
		if ( glfwGetKey( window, GLFW_KEY_S ) )	{	LightManager->vecLights.at(lightIndex)->position.z -= cameraSpeed * deltaTime;}
		if ( glfwGetKey( window, GLFW_KEY_A ) )	{	LightManager->vecLights.at(lightIndex)->position.x -= cameraSpeed * deltaTime;}
		if ( glfwGetKey( window, GLFW_KEY_D ) ) {	LightManager->vecLights.at(lightIndex)->position.x += cameraSpeed * deltaTime;}
		if ( glfwGetKey( window, GLFW_KEY_Q ) )	{	LightManager->vecLights.at(lightIndex)->position.y += cameraSpeed * deltaTime;}
		if ( glfwGetKey( window, GLFW_KEY_E ) )	{	LightManager->vecLights.at(lightIndex)->position.y -= cameraSpeed * deltaTime;}





		//change colour of the light
		if ( glfwGetKey( window, GLFW_KEY_Z ) )	{   LightManager->vecLights.at(lightIndex)->diffuse = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f); 	}//Red
		if ( glfwGetKey( window, GLFW_KEY_X ) )	{   LightManager->vecLights.at(lightIndex)->diffuse = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);	}//Green
		if ( glfwGetKey( window, GLFW_KEY_C ) )	{   LightManager->vecLights.at(lightIndex)->diffuse = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);    }//Blue
		if ( glfwGetKey( window, GLFW_KEY_V ) )	{	LightManager->vecLights.at(lightIndex)->diffuse = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);	}
		if ( glfwGetKey( window, GLFW_KEY_B ) )	{	LightManager->vecLights.at(lightIndex)->diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);	}


		//change lightype
		if (glfwGetKey(window, GLFW_KEY_1)) { LightManager->vecLights.at(lightIndex)->SetLightType(sLight::POINT_LIGHT); }//
		if (glfwGetKey(window, GLFW_KEY_2)) { LightManager->vecLights.at(lightIndex)->SetLightType(sLight::SPOT_LIGHT); }//

		//Change Light LookAt
		if (glfwGetKey(window, GLFW_KEY_R)) { LightManager->vecLights.at(lightIndex)->SetRelativeDirectionByLookAt(vec_controlable.at(index)->position); }



		//Change SpotCone
		if (glfwGetKey(window, GLFW_KEY_T))
		{
			LightManager->vecLights.at(lightIndex)->param1.y -= 0.1f;

		}


		if (glfwGetKey(window, GLFW_KEY_Y))
		{
			LightManager->vecLights.at(lightIndex)->param1.y += 0.1f;

		}


		if (glfwGetKey(window, GLFW_KEY_U))
		{
			LightManager->vecLights.at(lightIndex)->param1.z -= 0.1f;
		}


		if (glfwGetKey(window, GLFW_KEY_I))
		{
			LightManager->vecLights.at(lightIndex)->param1.z += 0.1f;
		}




		//change attenuation:

		//Linear
		if ( glfwGetKey( window, GLFW_KEY_LEFT)  )	{ LightManager->vecLights.at(lightIndex)->atten.y *= 1.05f; }
		if ( glfwGetKey( window, GLFW_KEY_RIGHT) )  { LightManager->vecLights.at(lightIndex)->atten.y *= 0.95f; }

		//Quadratic
		if ( glfwGetKey( window, GLFW_KEY_DOWN) )   { LightManager->vecLights.at(lightIndex)->atten.z *= 0.95f; }
		if ( glfwGetKey( window, GLFW_KEY_UP)   )	{ LightManager->vecLights.at(lightIndex)->atten.z *= 1.05f; }
		

	}



	//OBJECT CONTROL***********************************************************
	if ( IsAltDown(window) )
	{	//Object Postiton

		//if ( glfwGetKey( window, GLFW_KEY_W	) )	{ vec_controlable.at(index)->position.z -= cameraSpeed * deltaTime; }
		//if ( glfwGetKey( window, GLFW_KEY_S ) )	{ vec_controlable.at(index)->position.z += cameraSpeed * deltaTime; }
		//if ( glfwGetKey( window, GLFW_KEY_A ) )	{ vec_controlable.at(index)->position.x -= cameraSpeed * deltaTime; }
		//if ( glfwGetKey( window, GLFW_KEY_D ) ) { vec_controlable.at(index)->position.x += cameraSpeed * deltaTime; }
		//if ( glfwGetKey( window, GLFW_KEY_Q ) )	{ vec_controlable.at(index)->position.y -= cameraSpeed * deltaTime; }
		//if ( glfwGetKey( window, GLFW_KEY_E ) )	{ vec_controlable.at(index)->position.y += cameraSpeed * deltaTime; }

		//////Object Rotation
		//if (glfwGetKey(window, GLFW_KEY_RIGHT))		{ vec_controlable.at(index)->adjMeshOrientationEulerAngles(0.0f, 1.0f * deltaTime, 0.0f, false); }
		//if (glfwGetKey(window, GLFW_KEY_LEFT))		{ vec_controlable.at(index)->adjMeshOrientationEulerAngles(0.0f, -1.0f * deltaTime, 0.0f, false);}
		//if ( glfwGetKey( window, GLFW_KEY_UP ) )	{ vec_controlable.at(index)->adjMeshOrientationEulerAngles(0.0f, 0.0f, 1.0f * deltaTime, false); }
		//if (glfwGetKey(window, GLFW_KEY_DOWN))		{ vec_controlable.at(index)->adjMeshOrientationEulerAngles(0.0f, 0.0f, -1.0f * deltaTime, false); }
		//if ( glfwGetKey( window, GLFW_KEY_X ) )		{ vec_controlable.at(index)->adjMeshOrientationEulerAngles(1.0f * deltaTime, 0.0f, 0.0f, false); }
		//if (glfwGetKey(window, GLFW_KEY_C))			{ vec_controlable.at(index)->adjMeshOrientationEulerAngles(-1.0f * deltaTime, 0.0f, 0.0f, false); }

		//if (glfwGetKey(window, GLFW_KEY_V)) { vec_controlable.at(index)->nonUniformScale += cameraSpeed/100 * deltaTime; }
		//if (glfwGetKey(window, GLFW_KEY_B)) { vec_controlable.at(index)->nonUniformScale -= cameraSpeed/100 * deltaTime; }

		
		if (glfwGetKey(window, GLFW_KEY_W)) { GO->Vehicle->AddEngineForce(600.f); }
		else if (glfwGetKey(window, GLFW_KEY_S)) { GO->Vehicle->AddEngineForce(-600.f); }
		else
		{
			GO->Vehicle->AddEngineForce(0.f);

		}
		if (glfwGetKey(window, GLFW_KEY_A)) { GO->Vehicle->SetSteering(0.2f); }
		else if (glfwGetKey(window, GLFW_KEY_D)) { GO->Vehicle->SetSteering(-0.2f); }
		else
		{
			GO->Vehicle->SetSteering(0.0f);
		}
		//if (glfwGetKey(window, GLFW_KEY_Q)) { vec_controlable.at(index)->position.y -= cameraSpeed * deltaTime; }
		//if (glfwGetKey(window, GLFW_KEY_E)) { vec_controlable.at(index)->position.y += cameraSpeed * deltaTime; }
		if (glfwGetKey(window, GLFW_KEY_F)) 
		{ 
			GO->rigidBody->SetPosition(GO->InitPosition); 
			GO->rigidBody->SetQuatRotation(GO->InitRotaion);
			GO->rigidBody->SetVelocity(glm::vec3(0.0f));
			GO->rigidBody->SetAngularVelocity(glm::vec3(0.0f));
		}

		if (glfwGetKey(window, GLFW_KEY_R))
		{
			GO->rigidBody->SetPosition(GO->position + glm::vec3(0.0f, 2.5f, 0.f));
			GO->rigidBody->SetQuatRotation(GO->InitRotaion);
			GO->rigidBody->SetVelocity(glm::vec3(0.0f));
			GO->rigidBody->SetAngularVelocity(glm::vec3(0.0f));
		}
	}
	
	return;
}


void SwitchToWireFrame(std::vector<cGameObject*> models)
{
	for (std::vector<cGameObject*>::iterator it = models.begin(); it != models.end(); ++it)
	{
		cGameObject* CurModel = *it;
		CurModel->bIsWireFrame = true;
	}
}

void SwitchToSolid(std::vector<cGameObject*> models)
{
	for (std::vector<cGameObject*>::iterator it = models.begin(); it != models.end(); ++it)
	{
		cGameObject* CurModel = *it;
		CurModel->bIsWireFrame = false;

	}
}


void ManageScene(GLFWwindow* window)
{
	
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	bMouseInWindow = false;
	std::cout << "Save or Load? -s for save, -l for load, -c for cancel." << std::endl;
	std::cout << "-m to move selected object: " << vec_controlable[index]->friendlyName << std::endl;
	std::cout << "Enter Answer: " << std::endl;
	std::cout << "->";
	std::cin >> Answer;
	if (Answer == "-s" || Answer == "-S") {
		std::cout << "Which Scene to save? 1, 2?" << std::endl;
		std::cout << "->";
		std::cin >> Answer;
		if (Answer == "1") {
			g_pSceneManager->saveScene("scene1.json");
		}
		else if (Answer == "2") {
			g_pSceneManager->saveScene("scene2.json");
		}
		else if (Answer == "3") {
			saveModelInfo("Models3.txt", vec_controlable);
			saveLightInfo("lights3.txt", LightManager->vecLights);
			saveCameraInfo("camera3.txt");
		}
		else{ std::cout << "cancelling.." << std::endl; }
	}
	else if (Answer == "-l") {
		std::cout << "Which Scene to load? 1, 2 or 3?" << std::endl;
		std::cout << "->";
		std::cin >> Answer;
		if (Answer == "1") {
			loadModels("Models.txt", vec_controlable);
			loadLights("lights.txt", LightManager->vecLights);
			loadCameraInfo("camera.txt");
		}
		else if (Answer == "2") {
			loadModels("Models2.txt", vec_controlable);
			loadLights("lights2.txt", LightManager->vecLights);
			loadCameraInfo("camera2.txt");
		}
		else if (Answer == "3") {
			loadModels("Models3.txt", vec_controlable);
			loadLights("lights3.txt", LightManager->vecLights);
			loadCameraInfo("camera3.txt");
		}
		else{ std::cout << "cancelling.." << std::endl; }
	}
	else if (Answer == "-m") {
		commandsInterface();
	}
	else {
		std::cout << Answer << std::endl;
		std::cout << "Cancelling.." << std::endl;
	}
	std::cout << "Click on Progrm Window to Continue" << std::endl;


	bMouseInWindow = true;
	firstMouse = true;
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	
};



void commandsInterface()
{
	//cCommandGroup* pMoveToCG = new cCommandGroup();

	//cMoveToCommand*  pMoveTo = new cMoveToCommand();

	//std::vector<sNVPair> vecInitValues;

	//sNVPair ObjectToMove;	ObjectToMove.pMeshObj = vec_controlable[index];	

	std::cout << "Move object" << std::endl;
	std::cout << "1 - Move to object" << std::endl;
	std::cout << "2 - Move to coordinates" << std::endl;
	std::cout << "Enter Answer: " << std::endl;
	std::cout << "->";
	std::cin >> Answer;

	if(Answer == "1")
	{
		for (int i = 0; i < vec_controlable.size(); i++) { std::cout << vec_controlable[i]->friendlyName << std::endl; }
		std::cout << "Enter The name of the object: " << std::endl;
		std::cout << "->";
		std::cin >> Answer;
		//sNVPair Destination;	Destination.v3Value = findObjectByFriendlyName(Answer)->position;
		std::cout << "Enter Time in seconds: " << std::endl;
		std::cout << "->";
		std::cin >> Answer;
		//sNVPair Time;			Time.fValue = (float)atoi(Answer.c_str());
		//vecInitValues.push_back(ObjectToMove);	
		//vecInitValues.push_back(Destination);
		//vecInitValues.push_back(Time);
		

	}

	else if (Answer == "2")
	{
	//	glm::vec3 vec3MoveTo;
		std::cout << "Enter x: " << std::endl;
		std::cout << "->";
		std::cin >> Answer;
	//	vec3MoveTo.x = (float)atoi(Answer.c_str());
		std::cout << "Enter y: " << std::endl;
		std::cout << "->";
		std::cin >> Answer;
		//vec3MoveTo.y = (float)atoi(Answer.c_str());
		std::cout << "Enter z: " << std::endl;
		std::cout << "->";
		std::cin >> Answer;
		//vec3MoveTo.z = (float)atoi(Answer.c_str());
		
		std::cout << "Enter Time in seconds: " << std::endl;
		std::cout << "->";
		std::cin >> Answer;
		//sNVPair Destination;	Destination.v3Value = findObjectByFriendlyName(Answer)->position;
		//sNVPair Time;			Time.fValue = (float)atoi(Answer.c_str());
		//vecInitValues.push_back(ObjectToMove);	// idealCameraLocation
	//	vecInitValues.push_back(Destination);
	//	vecInitValues.push_back(Time);

	}

	//pMoveTo->Initialize(vecInitValues);

	//pMoveToCG->vecCommands.push_back(pMoveTo);

//	sceneCommandGroup.vecCommandGroups.push_back(pMoveToCG);
}