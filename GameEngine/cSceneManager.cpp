#include "cSceneManager.h"
#include "globalStuff.h"
#include <cstdio>
#include <fstream>
#include <iostream>
#include "cAnimationState.h"

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>
#include <Interfaces/GL_Triangle.h>

#include <rapidjson/writer.h>
#include <cstdio>


cSceneManager::cSceneManager() {};

bool AssimpSM_to_VAO_Converter(cSimpleAssimpSkinnedMesh* pTheAssimpSM,
	unsigned int shaderProgramID);

void cSceneManager::setBasePath(std::string basepath)
{
	this->m_basePath = basepath;
	return;
}


bool cSceneManager::saveScene(std::string filename) {

	rapidjson::Document doc;
	rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
	doc.SetObject();
	rapidjson::Value MeshArray(rapidjson::kArrayType);
	
	//Camera Output
	rapidjson::Value CameraObj(rapidjson::kObjectType);
	rapidjson::Value CameraSpeed(camera.MovementSpeed);
	rapidjson::Value CameraPosArray(rapidjson::kArrayType);

	for (int i = 0; i < 3; i++)
	{
		rapidjson::Value temp(camera.Position[i]);
		CameraPosArray.PushBack(temp, allocator);
	}
	CameraObj.AddMember("Speed", CameraSpeed, allocator);
	CameraObj.AddMember("Position", CameraPosArray, allocator);


	//GameObjects
	/*or rapidjson::Value myArray; ;
	     myArray.SetArray() */
	for (std::vector<cGameObject*>::iterator it = vec_pObjectsToDraw.begin(); it != vec_pObjectsToDraw.end(); ++it) {
		cGameObject* CurModel = *it;
		if (!CurModel->bIsDebug && CurModel->bSave) {
			rapidjson::Value ObjValue(rapidjson::kObjectType);
			rapidjson::Value FriendlyName(CurModel->friendlyName.c_str(), allocator);
			rapidjson::Value MeshName(CurModel->meshName.c_str(), allocator);
			rapidjson::Value PositionArray(rapidjson::kArrayType);
			rapidjson::Value DiffuseRGBArray(rapidjson::kArrayType);
			rapidjson::Value SpecularRGBArray(rapidjson::kArrayType);
			rapidjson::Value Rotation(rapidjson::kArrayType);
			rapidjson::Value Scale(rapidjson::kArrayType);
			rapidjson::Value Visible(CurModel->bIsVisible);
			rapidjson::Value UseHeightMap(CurModel->bUseHeighMap);
			rapidjson::Value UseNormalMap(CurModel->bUseNormalMap);
			rapidjson::Value UsePhysics(CurModel->bIsUpdatedByPhysics);
			rapidjson::Value WireFrame(CurModel->bIsWireFrame);



			for (int i = 0; i < 3; i++) {
				rapidjson::Value temp(CurModel->position[i]);
				PositionArray.PushBack(temp, allocator);
			}

			for (int i = 0; i < 4; i++) {
				rapidjson::Value temp(CurModel->materialDiffuse[i]);
				DiffuseRGBArray.PushBack(temp, allocator);
			}

			for (int i = 0; i < 4; i++) {
				rapidjson::Value temp(CurModel->materialSpecular[i]);
				SpecularRGBArray.PushBack(temp, allocator);
			}

			for (int i = 0; i < 3; i++) {
				glm::vec3 rot = CurModel->getMeshOrientationEulerAngles(true);
				rapidjson::Value temp(rot[i]);
				Rotation.PushBack(temp, allocator);
			}
			for (int i = 0; i < 3; i++) {
				rapidjson::Value temp(CurModel->nonUniformScale[i]);
				Scale.PushBack(temp, allocator);
			}

			ObjValue.AddMember("Name", FriendlyName, allocator);

			if(CurModel->pSimpleSkinnedMesh == NULL)
			{
				ObjValue.AddMember("Mesh", MeshName, allocator);
			}
			else
			{
				rapidjson::Value SkinnedMesh(CurModel->meshName.c_str(), allocator);
				ObjValue.AddMember("SkinnedMesh", MeshName, allocator);

				rapidjson::Value AnimObj(rapidjson::kObjectType);
				for (std::map<std::string, sAnimationInfo>::iterator it = CurModel->pSimpleSkinnedMesh->mapAnimationFriendlyNameTo_pScene.begin();
					it != CurModel->pSimpleSkinnedMesh->mapAnimationFriendlyNameTo_pScene.end(); ++it)
				{
					it->first;
					rapidjson::Value key(it->first.c_str(), allocator);
					rapidjson::Value val(it->second.fileName.c_str(), allocator);
					AnimObj.AddMember(key, val, allocator);
				}
				ObjValue.AddMember("Animation", AnimObj, allocator);
			}

			ObjValue.AddMember("Visible", Visible, allocator);
			ObjValue.AddMember("Use_Physics", UsePhysics, allocator);
			if(CurModel->bUseHeighMap){
				ObjValue.AddMember("Use_Height_Map", UseHeightMap, allocator);
			}
			if (CurModel->bUseNormalMap) {
				ObjValue.AddMember("Use_Normal_Map", UseNormalMap, allocator);
			}
			
			ObjValue.AddMember("Wireframe", WireFrame, allocator);
			ObjValue.AddMember("Position", PositionArray, allocator);
			ObjValue.AddMember("DiffuseRGB_Alpha", DiffuseRGBArray, allocator);
			ObjValue.AddMember("SpecularRGB_Alpha", SpecularRGBArray, allocator);
			ObjValue.AddMember("Rotation", Rotation, allocator);
			ObjValue.AddMember("Scale", Scale, allocator);

			//Textures
			if (CurModel->vecTextures.size() > 0) {
				rapidjson::Value TexArray(rapidjson::kArrayType);
				for (int i = 0; i < CurModel->vecTextures.size(); i++) {
					rapidjson::Value TexObjValue(rapidjson::kObjectType);
					rapidjson::Value TexName(CurModel->vecTextures[i].name.c_str(), allocator);
					rapidjson::Value TexStrength(CurModel->vecTextures[i].strength);
					rapidjson::Value TexTile(CurModel->vecTextures[i].textureTileMult);
					TexObjValue.AddMember("Name", TexName, allocator);
					TexObjValue.AddMember("Strength", TexStrength, allocator);
					TexObjValue.AddMember("Tile", TexTile, allocator);
					TexArray.PushBack(TexObjValue, allocator);
				}
				ObjValue.AddMember("Textures", TexArray, allocator);




				if (CurModel->rigidBody != NULL) {

					rapidjson::Value PhysObjValue(rapidjson::kObjectType);
					switch (CurModel->rigidBody->GetShape()->GetShapeType())
					{
					case nPhysics::SHAPE_TYPE_SPHERE:
					{
						PhysObjValue.AddMember("Type", "SPHERE", allocator);
						float r = 0.0f;
						CurModel->rigidBody->GetShape()->GetSphereRadius(r);
						PhysObjValue.AddMember("Radius", r, allocator);

						
						if (CurModel->bIsGhost)
						{
							rapidjson::Value isGhost(CurModel->bIsGhost);
							PhysObjValue.AddMember("IsGhost", isGhost, allocator);
						}
						
					}
						break;
					case nPhysics::SHAPE_TYPE_PLANE:
					{

						PhysObjValue.AddMember("Type", "PLANE", allocator);
						float planeConst = 0.0f;
						CurModel->rigidBody->GetShape()->GetPlaneConstant(planeConst);
						PhysObjValue.AddMember("Constant", planeConst, allocator);

						rapidjson::Value NormalArray(rapidjson::kArrayType);
						glm::vec3 planeNormal = glm::vec3(0.0f);
						CurModel->rigidBody->GetShape()->GetPlaneNormal(planeNormal);

						for (int i = 0; i < 3; i++) {
							rapidjson::Value temp(planeNormal[i]);
							NormalArray.PushBack(temp, allocator);
						}
						PhysObjValue.AddMember("Normal", NormalArray, allocator);
					}
						break;
					case nPhysics::SHAPE_TYPE_BOX:
					{

						PhysObjValue.AddMember("Type", "BOX", allocator);

						if (!CurModel->bExtentsFromMesh)
						{
							rapidjson::Value halfExtentsArray(rapidjson::kArrayType);
							glm::vec3 halfExtents = CurModel->rigidBody->GetShape()->GetHalfExtents();
							for (int i = 0; i < 3; i++) {
								rapidjson::Value temp(halfExtents[i]);
								halfExtentsArray.PushBack(temp, allocator);
							}
							PhysObjValue.AddMember("HalfExtents", halfExtentsArray, allocator);
						}

					}
						break;
					case nPhysics::SHAPE_TYPE_CAPSULE:
					{

						PhysObjValue.AddMember("Type", "CAPSULE", allocator);


						int Axis = CurModel->rigidBody->GetShape()->GetCapsuleAxis();
						float heigth = CurModel->rigidBody->GetShape()->GetCapsuleHeight();
						float radius = CurModel->rigidBody->GetShape()->GetCapsuleRadius();
						PhysObjValue.AddMember("Axis", Axis, allocator);
						PhysObjValue.AddMember("Radius", radius, allocator);
						PhysObjValue.AddMember("Height", heigth, allocator);

						if (CurModel->bIsPlayer == true)
						{
							PhysObjValue.AddMember("IsPlayer", true, allocator);
						}



					}
						break;
					case nPhysics::SHAPE_TYPE_MESH:
					{

						PhysObjValue.AddMember("Type", "MESH_COLLIDER", allocator);

					}
						break;
					default:
						break;
					}



					PhysObjValue.AddMember("Mass", CurModel->rigidBody->GetMass(), allocator);
					ObjValue.AddMember("RigidBody", PhysObjValue, allocator);
				
				}


				


			}



			MeshArray.PushBack(ObjValue, allocator);

		}
	}



	//Lights
	rapidjson::Value LightsArray(rapidjson::kArrayType);
	for (std::vector<sLight*>::iterator it = LightManager->vecLights.begin(); it != LightManager->vecLights.end(); ++it) {
		sLight* CurLight = *it;
		rapidjson::Value ObjValue(rapidjson::kObjectType);
		rapidjson::Value FriendlyName(CurLight->lightName.c_str(), allocator);
		rapidjson::Value LightType(CurLight->GetLightType_str().c_str(), allocator);
		rapidjson::Value PositionArray(rapidjson::kArrayType);
		rapidjson::Value AttenArray(rapidjson::kArrayType);
		rapidjson::Value DiffuseArray(rapidjson::kArrayType);
		rapidjson::Value DirectionArray(rapidjson::kArrayType);
		rapidjson::Value Turned("OFF");
		
	

		//cone Angles
		

		//light position

		for (int i = 0; i < 3; i++) {
			rapidjson::Value temp(CurLight->position[i]);
			PositionArray.PushBack(temp, allocator);
		}
		for (int i = 0; i < 4; i++) {
			rapidjson::Value temp(CurLight->diffuse[i]);
			DiffuseArray.PushBack(temp, allocator);
		}
		for (int i = 0; i < 4; i++) {
			rapidjson::Value temp(CurLight->atten[i]);
			AttenArray.PushBack(temp, allocator);
		}
		

		if (CurLight->param2.x == 1.0f) {
			rapidjson::Value temp("ON");
			Turned = temp;
		}
	

		ObjValue.AddMember("Name", FriendlyName, allocator);
		ObjValue.AddMember("Type", LightType, allocator);

		if (CurLight->GetLightType_str() != "POINT_LIGHT") {
			rapidjson::Value AnglesArray(rapidjson::kArrayType);
			for (int i = 1; i < 3; i++) {
				rapidjson::Value temp(CurLight->param1[i]);
				AnglesArray.PushBack(temp, allocator);
			}
			ObjValue.AddMember("Angles", AnglesArray, allocator);

			// Direction Or LookAt object
			if (CurLight->ObjectLookAt == NULL ) {
				for (int i = 0; i < 3; i++) {
					rapidjson::Value temp(CurLight->direction[i]);
					DirectionArray.PushBack(temp, allocator);
				}
				ObjValue.AddMember("Direction", DirectionArray, allocator);
			}
			else {
				rapidjson::Value temp(CurLight->ObjectLookAt->friendlyName.c_str(), allocator);
				ObjValue.AddMember("ObjectLookAt", temp, allocator);
			}
		}
		ObjValue.AddMember("Position", PositionArray, allocator);
		ObjValue.AddMember("Attenuation", AttenArray, allocator);
		ObjValue.AddMember("DiffuseRGB_Alpha", DiffuseArray, allocator);
		ObjValue.AddMember("Turned", Turned, allocator);

		LightsArray.PushBack(ObjValue, allocator);

	}











	doc.AddMember("Camera", CameraObj, allocator);
	doc.AddMember("GameObjects", MeshArray, allocator);
	doc.AddMember("Lights", LightsArray, allocator);

	
	
	std::string fileToLoadFullPath = this->m_basePath + "/" + filename;

	FILE* fp = fopen(fileToLoadFullPath.c_str(), "wb"); // non-Windows use "w"
	char writeBuffer[65536];
	rapidjson::FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
	rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(os);
	doc.Accept(writer);
	fclose(fp);
	return true;
}




bool cSceneManager::loadScene(std::string filename) {


	std::string fileToLoadFullPath = this->m_basePath + "/" + filename;

	//vec_pObjectsToDraw.clear();
	//LightManager->vecLights.clear();
	::g_pTheTextureManager->SetBasePath("assets/textures");
	rapidjson::Document doc;
	FILE* fp = fopen(fileToLoadFullPath.c_str(), "rb"); // non-Windows use "r"
	char readBuffer[65536];
	rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
	doc.ParseStream(is);
	fclose(fp);


	//Camera
	float Speed;
	glm::vec3 CameraPos;
	rapidjson::Value& CameraPosArray = doc["Camera"]["Position"];

	camera.MovementSpeed = doc["Camera"]["Speed"].GetFloat();
	for (rapidjson::SizeType i = 0; i < CameraPosArray.Size(); i++) {
		CameraPos[i] = CameraPosArray[i].GetFloat();
		std::cout << "CameraPos: [ " << CameraPos[i] << " ]" << std::endl;
	}
	camera.Position = CameraPos;

	

	//Game Objects
	const rapidjson::Value& GameObject = doc["GameObjects"];
	const rapidjson::Value& LightObject = doc["Lights"];


	for (rapidjson::SizeType i = 0; i < GameObject.Size(); i++) {

		cGameObject *CurModel = new cGameObject();
		sModelDrawInfo curModelInfo;

		CurModel->friendlyName = GameObject[i]["Name"].GetString();	
		CurModel->bIsVisible = GameObject[i]["Visible"].GetBool();
		CurModel->bIsUpdatedByPhysics = GameObject[i]["Use_Physics"].GetBool();
		CurModel->bIsWireFrame = GameObject[i]["Wireframe"].GetBool();

		if (GameObject[i].HasMember("FBO"))
		{
			CurModel->bFBO = GameObject[i]["FBO"].GetBool();
		}

		if (GameObject[i].HasMember("Reflection"))
		{
			CurModel->bRefraction = GameObject[i]["Reflection"].GetBool();
		}

		if (GameObject[i].HasMember("Refraction"))
		{
			CurModel->bRefraction = GameObject[i]["Refraction"].GetBool();
		}
		if (GameObject[i].HasMember("Use_Height_Map"))
		{
			CurModel->bUseHeighMap = GameObject[i]["Use_Height_Map"].GetBool();
		}

		if (GameObject[i].HasMember("Use_Normal_Map"))
		{
			CurModel->bUseNormalMap = GameObject[i]["Use_Normal_Map"].GetBool();
		}

		//if ply;
		if(GameObject[i].HasMember("Mesh"))
		{
			CurModel->meshName = GameObject[i]["Mesh"].GetString();
			curModelInfo.meshFileName = GameObject[i]["Mesh"].GetString();
			g_pTheVAOMeshManager->LoadModelIntoVAO(curModelInfo, program);
		}
		//if Skinned Mesh;
		if (GameObject[i].HasMember("SkinnedMesh"))
		{
			CurModel->meshName = GameObject[i]["SkinnedMesh"].GetString();
			cSimpleAssimpSkinnedMesh* curSkinnedMesh = new cSimpleAssimpSkinnedMesh();
			if (!curSkinnedMesh->LoadMeshFromFile(CurModel->friendlyName, CurModel->meshName))
			{
				std::cout << "Error: problem loading the skinned mesh" << std::endl;
			}
			if (GameObject[i].HasMember("Animation"))
			{
				std::string Idle = (GameObject[i]["Animation"].HasMember("Idle")) ? GameObject[i]["Animation"]["Idle"].GetString() : "";
				if (Idle != "") {
					curSkinnedMesh->LoadMeshAnimation("Idle", Idle);
				}

				std::string Forward = (GameObject[i]["Animation"].HasMember("Walk-forward")) ? GameObject[i]["Animation"]["Walk-forward"].GetString() : "";
				if (Forward != "") {
					curSkinnedMesh->LoadMeshAnimation("Walk-forward", Forward);
				}

				std::string Run = (GameObject[i]["Animation"].HasMember("Run-forward")) ? GameObject[i]["Animation"]["Run-forward"].GetString() : "";
				if (Run != "") {
					curSkinnedMesh->LoadMeshAnimation("Run-forward", Run);
				}

				std::string Backward = (GameObject[i]["Animation"].HasMember("Walk-backward")) ? GameObject[i]["Animation"]["Walk-backward"].GetString() : "";
				if (Backward != "") {
					curSkinnedMesh->LoadMeshAnimation("Walk-backward", Backward);
				}

				std::string StrafeL = (GameObject[i]["Animation"].HasMember("Strafe-left")) ? GameObject[i]["Animation"]["Strafe-left"].GetString() : "";
				if (StrafeL != "") {
					curSkinnedMesh->LoadMeshAnimation("Strafe-left", StrafeL);
				}

				std::string StrafeR = (GameObject[i]["Animation"].HasMember("Strafe-right")) ? GameObject[i]["Animation"]["Strafe-right"].GetString() : "";
				if (StrafeR != "") {
					curSkinnedMesh->LoadMeshAnimation("Strafe-right", StrafeR);
				}

				std::string RollL = (GameObject[i]["Animation"].HasMember("Roll-left")) ? GameObject[i]["Animation"]["Roll-left"].GetString() : "";
				if (RollL != "") {
					curSkinnedMesh->LoadMeshAnimation("Roll-left", RollL);
				}

				std::string RollR = (GameObject[i]["Animation"].HasMember("Roll-right")) ? GameObject[i]["Animation"]["Roll-right"].GetString() : "";
				if (RollR != "") {
					curSkinnedMesh->LoadMeshAnimation("Roll-right", RollR);
				}

				std::string TurnL = (GameObject[i]["Animation"].HasMember("Turn-Left")) ? GameObject[i]["Animation"]["Turn-Left"].GetString() : "";
				if (TurnL != "") {
					curSkinnedMesh->LoadMeshAnimation("Turn-Left", TurnL);
				}

				std::string TurnR = (GameObject[i]["Animation"].HasMember("Turn-Right")) ? GameObject[i]["Animation"]["Turn-Right"].GetString() : "";
				if (TurnR != "") {
					curSkinnedMesh->LoadMeshAnimation("Turn-Right", TurnR);
				}

				std::string RunJump = (GameObject[i]["Animation"].HasMember("Run-jump")) ? GameObject[i]["Animation"]["Run-jump"].GetString() : "";
				if (RunJump != "") {
					curSkinnedMesh->LoadMeshAnimation("Run-jump", RunJump, true);
				}

				std::string Jump1 = (GameObject[i]["Animation"].HasMember("Jump1")) ? GameObject[i]["Animation"]["Jump1"].GetString() : "";
				if (Jump1 != "") {
					curSkinnedMesh->LoadMeshAnimation("Jump1", Jump1);
				}

				std::string Jump2 = (GameObject[i]["Animation"].HasMember("Jump2")) ? GameObject[i]["Animation"]["Jump2"].GetString() : "";
				if (Jump2 != "") {
					curSkinnedMesh->LoadMeshAnimation("Jump2", Jump2);
				}

				std::string Action1 = (GameObject[i]["Animation"].HasMember("Action1")) ? GameObject[i]["Animation"]["Action1"].GetString() : "";
				if (Action1 != "") {
					curSkinnedMesh->LoadMeshAnimation("Action1", Action1);
				}

				std::string Action2 = (GameObject[i]["Animation"].HasMember("Action2")) ? GameObject[i]["Animation"]["Action2"].GetString() : "";
				if (Action2 != "") {
					curSkinnedMesh->LoadMeshAnimation("Action2", Action2);
				}

				std::string Action3 = (GameObject[i]["Animation"].HasMember("Action3")) ? GameObject[i]["Animation"]["Action3"].GetString() : "";
				if (Action3 != "") {
					curSkinnedMesh->LoadMeshAnimation("Action3", Action3);
				}

				std::string Action4 = (GameObject[i]["Animation"].HasMember("Action4")) ? GameObject[i]["Animation"]["Action4"].GetString() : "";
				if (Action4 != "") {
					curSkinnedMesh->LoadMeshAnimation("Action4", Action4);
				}

				std::string Action5 = (GameObject[i]["Animation"].HasMember("Action5")) ? GameObject[i]["Animation"]["Action5"].GetString() : "";
				if (Action5 != "") {
					curSkinnedMesh->LoadMeshAnimation("Action5", Action5);
				}

				std::string Action6 = (GameObject[i]["Animation"].HasMember("Action6")) ? GameObject[i]["Animation"]["Action6"].GetString() : "";
				if (Action6 != "") {
					curSkinnedMesh->LoadMeshAnimation("Action6", Action6);
				}

				std::string Action7 = (GameObject[i]["Animation"].HasMember("Action7")) ? GameObject[i]["Animation"]["Action7"].GetString() : "";
				if (Action7 != "") {
					curSkinnedMesh->LoadMeshAnimation("Action7", Action7);
				}

				std::string Action8 = (GameObject[i]["Animation"].HasMember("Action8")) ? GameObject[i]["Animation"]["Action8"].GetString() : "";
				if (Action8 != "") {
					curSkinnedMesh->LoadMeshAnimation("Action8", Action8);
				}

				std::string Action9 = (GameObject[i]["Animation"].HasMember("Action9")) ? GameObject[i]["Animation"]["Action9"].GetString() : "";
				if (Action9 != "") {
					curSkinnedMesh->LoadMeshAnimation("Action9", Action9);
				}

				std::string Fall = (GameObject[i]["Animation"].HasMember("Fall")) ? GameObject[i]["Animation"]["Fall"].GetString() : "";
				if (Fall != "") {
					curSkinnedMesh->LoadMeshAnimation("Fall", Fall);
				}

				std::string ActionWalkBack = (GameObject[i]["Animation"].HasMember("Action-walk-backward")) ? GameObject[i]["Animation"]["Action-walk-backward"].GetString() : "";
				if (ActionWalkBack != "") {
					curSkinnedMesh->LoadMeshAnimation("Action-walk-backward", ActionWalkBack);
				}

				
				

				//cAnimationState* pAniState;
				CurModel->pSimpleSkinnedMesh = curSkinnedMesh;
				CurModel->pAniState = new cAnimationState();
				CurModel->pAniState->defaultAnimation.name = "Idle";
				CurModel->currentAnimation = "Idle";

			}
			if (!AssimpSM_to_VAO_Converter(curSkinnedMesh, program))
			{
				std::cout << "Error: Didn't copy the skinned mesh into the VAO format." << std::endl;
			}

		}

		
		
		

		const rapidjson::Value& PositionArray = GameObject[i]["Position"];
		for (rapidjson::SizeType i = 0; i < PositionArray.Size(); i++) {

			CurModel->position[i] = PositionArray[i].GetFloat();
			
		}

		const rapidjson::Value& DiffuseArray = GameObject[i]["DiffuseRGB_Alpha"];
		for (rapidjson::SizeType i = 0; i < DiffuseArray.Size(); i++) {

			CurModel->materialDiffuse[i] = DiffuseArray[i].GetFloat();

		}

		const rapidjson::Value& SpecularArray = GameObject[i]["SpecularRGB_Alpha"];
		for (rapidjson::SizeType i = 0; i < SpecularArray.Size(); i++) {

			CurModel->materialSpecular[i] = SpecularArray[i].GetFloat();

		}

		const rapidjson::Value& RotationArray = GameObject[i]["Rotation"];
		//for (rapidjson::SizeType i = 0; i < RotationArray.Size(); i++) {
			//In Degrees
	    glm::vec3 rot(RotationArray[0].GetFloat(), RotationArray[1].GetFloat(), RotationArray[2].GetFloat());
		CurModel->setMeshOrientationEulerAngles(rot, true);

			//CurModel->m_meshQOrientation[i] = RotationArray[i].GetFloat();

		//}

		const rapidjson::Value& ScaleArray = GameObject[i]["Scale"];
		for (rapidjson::SizeType i = 0; i < ScaleArray.Size(); i++) {

			CurModel->nonUniformScale[i] = ScaleArray[i].GetFloat();

		}

		if (GameObject[i].HasMember("Textures")) {
			const rapidjson::Value& TexArray = GameObject[i]["Textures"];
			for (rapidjson::SizeType i = 0; i < TexArray.Size(); i++)
			{
				sTextureInfo CurModelTex;
				CurModelTex.name = TexArray[i]["Name"].GetString();
				CurModelTex.strength = TexArray[i]["Strength"].GetFloat();
				if (TexArray[i].HasMember("Tile"))
				{
					CurModelTex.textureTileMult = TexArray[i]["Tile"].GetFloat();
				}
				CurModel->vecTextures.push_back(CurModelTex);
				//Creating Texture even if there is alreade same textue NEED FIX
				::g_pTheTextureManager->Create2DTextureFromBMPFile(CurModelTex.name, true);
			}
		}

		if (GameObject[i].HasMember("IsProjectile"))
		{
			CurModel->bIsProjectile = true;
		}


		if (GameObject[i].HasMember("IsImposter"))
		{
			CurModel->bIsImposter = GameObject[i]["IsImposter"].GetBool();
		}

		if (GameObject[i].HasMember("Use_LOD"))
		{
			CurModel->bUseLOD = GameObject[i]["Use_LOD"].GetBool();
		}
		

		if (GameObject[i].HasMember("Props")) {
			const rapidjson::Value& PopsArray = GameObject[i]["Props"];
			for (rapidjson::SizeType i = 0; i < PopsArray.Size(); i++)
			{
				std::string ObjName;
				ObjName = PopsArray[i].GetString();
				cGameObject* prop = findObjectByFriendlyName(ObjName);
				if(prop)
				CurModel->vec_pChildObjectsToDraw.push_back(prop);
			}
		}


		if(GameObject[i].HasMember("RigidBody"))
		{
			std::string type = GameObject[i]["RigidBody"]["Type"].GetString();
			if (type == "CLOTH")
			{
				////nPhysics::iSoftBody = new
				//nPhysics::sSoftBodyDef def;
				//def.SpringConstant = GameObject[i]["RigidBody"]["SpringConstant"].GetFloat();
				//glm::vec3 cornerA;
				//glm::vec3 cornerB;
				//glm::vec3 cornerC;

				//const rapidjson::Value& jsonCornerA = GameObject[i]["RigidBody"]["CornerA"];
				//for (int i = 0; i < 3; i++)
				//{
				//	cornerA[i] = jsonCornerA[i].GetFloat();
				//}
				//const rapidjson::Value& jsonCornerB = GameObject[i]["RigidBody"]["CornerB"];
				//for (int i = 0; i < 3; i++)
				//{
				//	cornerB[i] = jsonCornerB[i].GetFloat();
				//}
				//const rapidjson::Value& jsonCornerC = GameObject[i]["RigidBody"]["CornerC"];
				//for (int i = 0; i < 3; i++)
				//{
				//	cornerC[i] = jsonCornerC[i].GetFloat();
				//}


				//int numNodesAtoB = GameObject[i]["RigidBody"]["NumNodesAB"].GetInt();
				//int numNodesAtoC = GameObject[i]["RigidBody"]["NumNodesAC"].GetInt();
				//glm::vec3 position(0.f);
				//glm::vec3 test = cornerB - cornerA;
				//glm::vec3 sepAtoB((cornerB - cornerA) / (float)(numNodesAtoB - 1));
				//glm::vec3 sepAtoC((cornerC - cornerA) / (float)(numNodesAtoC - 1));

				//for (int idxB = 0; idxB < numNodesAtoC; idxB++)
				//{
				//	
				//	for (int idxA = 0; idxA < numNodesAtoB; idxA++)
				//	{
				//		
				//		def.Nodes.push_back(nPhysics::sSoftBodyNodeDef(cornerA + sepAtoB * float(idxA) + sepAtoC * float(idxB), 1.0f));
				//		position.x += 5.0f;
				//	}
				//	position.y += 5.0f;
				//	position.x = 0.f;
				//}

				//for (int indexm = 0; indexm < numNodesAtoB; indexm++)
				//{
				//	def.Nodes[indexm].Mass = 0;

				//}


				//for (int idxB = 0; idxB < numNodesAtoC - 1; idxB++)
				//{

				//	for (int idxA = 0; idxA < numNodesAtoB - 1; idxA++)
				//	{
				//		int index = idxA + idxB * numNodesAtoB;
				//		def.Springs.push_back(std::make_pair(index, index + 1));
				//		def.Springs.push_back(std::make_pair(index, index + numNodesAtoB));
				//	}

				//}
				//int  numNodes = numNodesAtoB * numNodesAtoC;
				//for (int idxA = 0; idxA < numNodesAtoB - 1; idxA++)
				//{
				//	def.Springs.push_back(std::make_pair(numNodes - idxA - 1, numNodes - idxA - 2));
				//}

				//for (int idxB = 1; idxB < numNodesAtoC; idxB++)
				//{
				//	int index = (numNodesAtoB * idxB) - 1;
				//	def.Springs.push_back(std::make_pair(index, index + numNodesAtoB));
				//}


				//nPhysics::iSoftBody* curSoftBody = gPhysicsFactory->CreateSoftBody(def);
				//CurModel->softBody = curSoftBody;
				//gPhysicsWorld->AddBody(curSoftBody);

			}




			if (type == "MESH_COLLIDER")
			{
				nPhysics::iShape* CurShape = NULL;
				nPhysics::sRigidBodyDef def;
				def.quatOrientation = CurModel->m_meshQOrientation;
				def.Position = CurModel->position;
				def.GameObjectName = CurModel->friendlyName;

				nPhysics::GL_Triangle* GLTriangle = new nPhysics::GL_Triangle[curModelInfo.pMeshData->numberOfTriangles];
				for (int i = 0; i < curModelInfo.pMeshData->numberOfTriangles; i++)
				{

					GLTriangle[i].vertex1[0] = curModelInfo.pMeshData->pVertices[curModelInfo.pMeshData->pTriangles[i].vertex_ID_0].x;
					GLTriangle[i].vertex1[1] = curModelInfo.pMeshData->pVertices[curModelInfo.pMeshData->pTriangles[i].vertex_ID_0].y;
					GLTriangle[i].vertex1[2] = curModelInfo.pMeshData->pVertices[curModelInfo.pMeshData->pTriangles[i].vertex_ID_0].z;

					GLTriangle[i].vertex2[0] = curModelInfo.pMeshData->pVertices[curModelInfo.pMeshData->pTriangles[i].vertex_ID_1].x;
					GLTriangle[i].vertex2[1] = curModelInfo.pMeshData->pVertices[curModelInfo.pMeshData->pTriangles[i].vertex_ID_1].y;
					GLTriangle[i].vertex2[2] = curModelInfo.pMeshData->pVertices[curModelInfo.pMeshData->pTriangles[i].vertex_ID_1].z;

					GLTriangle[i].vertex3[0] = curModelInfo.pMeshData->pVertices[curModelInfo.pMeshData->pTriangles[i].vertex_ID_2].x;
					GLTriangle[i].vertex3[1] = curModelInfo.pMeshData->pVertices[curModelInfo.pMeshData->pTriangles[i].vertex_ID_2].y;
					GLTriangle[i].vertex3[2] = curModelInfo.pMeshData->pVertices[curModelInfo.pMeshData->pTriangles[i].vertex_ID_2].z;

				}

				CurShape = gPhysicsFactory->CreateMeshCollider(GLTriangle, curModelInfo.pMeshData->numberOfTriangles);

				def.Scale = glm::vec3(CurModel->nonUniformScale.x, CurModel->nonUniformScale.y, CurModel->nonUniformScale.z);
				nPhysics::iRigidBody* rigidBody = gPhysicsFactory->CreateRigidBody(def, CurShape);
				CurModel->rigidBody = rigidBody;
				gPhysicsWorld->AddBody(rigidBody);

			}

			
			if (type == "SPHERE")
			{
				nPhysics::iShape* CurShape = NULL;
				nPhysics::sRigidBodyDef def;
				def.quatOrientation = CurModel->m_meshQOrientation;
				def.Position = CurModel->position;
				def.GameObjectName = CurModel->friendlyName;

				float radius = GameObject[i]["RigidBody"]["Radius"].GetFloat();
				CurShape = gPhysicsFactory->CreateSphereShape(radius);
				def.Mass = GameObject[i]["RigidBody"]["Mass"].GetFloat();


				if (GameObject[i]["RigidBody"].HasMember("IsGhost"))
				{
					def.isGhostShape = GameObject[i]["RigidBody"]["IsGhost"].GetBool();
					CurModel->bIsGhost = def.isGhostShape;
				}

				nPhysics::iRigidBody* rigidBody = gPhysicsFactory->CreateRigidBody(def, CurShape);
				CurModel->rigidBody = rigidBody;
				gPhysicsWorld->AddBody(rigidBody);
				
			}

			if (type == "SPHERE_TO_POINT")
			{
				nPhysics::iShape* CurShape = NULL;
				nPhysics::sRigidBodyDef def;
				def.quatOrientation = CurModel->m_meshQOrientation;
				def.Position = CurModel->position;
				def.GameObjectName = CurModel->friendlyName;

				float radius = GameObject[i]["RigidBody"]["Radius"].GetFloat();
				CurShape = gPhysicsFactory->CreateSphereShape(radius);
				def.Mass = GameObject[i]["RigidBody"]["Mass"].GetFloat();

				const rapidjson::Value& pivotArray = GameObject[i]["RigidBody"]["Pivot"];
				glm::vec3 pivot;
				for (int i = 0; i < 3; i++)
				{
					pivot[i] = pivotArray[i].GetFloat();
				}


				nPhysics::iRigidBody* rigidBody = gPhysicsFactory->CreateRigidBody(def, CurShape);
				CurModel->rigidBody = rigidBody;
				gPhysicsWorld->AddBody(rigidBody);
				nPhysics::iConstraint* costr = gPhysicsFactory->CreatePointToPointConstraint(rigidBody, pivot);
				gPhysicsWorld->AddConstraint(costr);

			}

			if (type == "SPHERE_TO_SPHERE")
			{
				nPhysics::iShape* CurShapeA = NULL;
				nPhysics::iShape* CurShapeB = NULL;
				nPhysics::sRigidBodyDef defA;
				nPhysics::sRigidBodyDef defB;



				float radius = GameObject[i]["RigidBody"]["Radius"].GetFloat();
				
				defA.Mass = GameObject[i]["RigidBody"]["Mass"].GetFloat();
				defB.Mass = GameObject[i]["RigidBody"]["Mass"].GetFloat();


				const rapidjson::Value& offsetArray = GameObject[i]["RigidBody"]["Offset"];
				glm::vec3 offset;
				for (int i = 0; i < 3; i++)
				{
					offset[i] = offsetArray[i].GetFloat();
				}

				const rapidjson::Value& pivotArrayA = GameObject[i]["RigidBody"]["PivotA"];
				glm::vec3 pivotA;
				for (int i = 0; i < 3; i++)
				{
					pivotA[i] = pivotArrayA[i].GetFloat();
				}

				const rapidjson::Value& pivotArrayB = GameObject[i]["RigidBody"]["PivotB"];
				glm::vec3 pivotB;
				for (int i = 0; i < 3; i++)
				{
					pivotB[i] = pivotArrayB[i].GetFloat();
				}

				CurShapeA = gPhysicsFactory->CreateSphereShape(radius);
				CurShapeB = gPhysicsFactory->CreateSphereShape(radius);

				defA.Orientation = CurModel->getMeshOrientationEulerAngles();
				defB.Orientation = CurModel->getMeshOrientationEulerAngles();
				defA.Position = CurModel->position;
				defA.GameObjectName = CurModel->friendlyName;
				defB.GameObjectName = CurModel->friendlyName + "second";
				defB.Position = CurModel->position + offset;


				cGameObject* CurModel2 = new cGameObject();
				CurModel2->position = defB.Position;
				CurModel2->friendlyName = CurModel->friendlyName + "second";
				CurModel2->meshName = CurModel->meshName;
				CurModel2->setSpecularPower(CurModel->materialSpecular.a);
				CurModel2->setDiffuseColour(glm::vec3(0.f));
				CurModel2->vecTextures = CurModel->vecTextures;
				CurModel2->nonUniformScale = CurModel->nonUniformScale;

				nPhysics::iRigidBody* rigidBodyA = gPhysicsFactory->CreateRigidBody(defA, CurShapeA);
				nPhysics::iRigidBody* rigidBodyB = gPhysicsFactory->CreateRigidBody(defB, CurShapeB);
				CurModel->rigidBody = rigidBodyA;
				CurModel2->rigidBody = rigidBodyB;

				gPhysicsWorld->AddBody(rigidBodyA);
				gPhysicsWorld->AddBody(rigidBodyB);

				nPhysics::iConstraint* constr = gPhysicsFactory->CreatePointToPointConstraint(rigidBodyA, rigidBodyB, pivotA, pivotB);
				gPhysicsWorld->AddConstraint(constr);

				vec_pObjectsToDraw.push_back(CurModel2);


			}

			if (type == "VEHICLE")
			{
				nPhysics::iShape* CurShape = NULL;
				nPhysics::sRigidBodyDef def;
				def.Scale = glm::vec3(CurModel->nonUniformScale.x, CurModel->nonUniformScale.y, CurModel->nonUniformScale.x);
				//in Radians
				def.Position = CurModel->position;
				def.Mass = GameObject[i]["RigidBody"]["Mass"].GetFloat();
				def.GameObjectName = CurModel->friendlyName;
				def.quatOrientation = CurModel->m_meshQOrientation;

				glm::vec3 halfExtents;
				if (GameObject[i]["RigidBody"].HasMember("HalfExtents"))
				{
					CurModel->bExtentsFromMesh = false;
					const rapidjson::Value& ExtentsArray = GameObject[i]["RigidBody"]["HalfExtents"];
					for (int i = 0; i < 3; i++)
					{
						halfExtents[i] = ExtentsArray[i].GetFloat();
					}
					halfExtents.x * CurModel->nonUniformScale.x;
					halfExtents.y * CurModel->nonUniformScale.y;
					halfExtents.z * CurModel->nonUniformScale.z;
				}
				else
				{
					halfExtents = glm::vec3(curModelInfo.maxX * CurModel->nonUniformScale.x, curModelInfo.maxY * CurModel->nonUniformScale.y,
						curModelInfo.maxZ * CurModel->nonUniformScale.z);

				}
				CurShape = gPhysicsFactory->CreateBoxShape(halfExtents);

				nPhysics::iRigidBody* rigidBody = gPhysicsFactory->CreateRigidBody(def, CurShape);
				CurModel->rigidBody = rigidBody;
				gPhysicsWorld->AddBody(rigidBody);

				nPhysics::iVehicle* vehicle = gPhysicsFactory->CreateVehicle(rigidBody, gPhysicsWorld);
				gPhysicsWorld->AddVehicle(vehicle);
				CurModel->Vehicle = vehicle;
			}

			else if (type == "PLANE")
			{
				
				nPhysics::iShape* CurShape = NULL;
				nPhysics::sRigidBodyDef def;

				//in Radians
				def.Position = CurModel->position;
				def.GameObjectName = CurModel->friendlyName;
				float planeConst = GameObject[i]["RigidBody"]["Constant"].GetFloat();


				const rapidjson::Value& NormArray = GameObject[i]["RigidBody"]["Normal"];
				glm::vec3 norm;
				for (int i = 0; i < 3; i++)
				{
					norm[i] = NormArray[i].GetFloat();
				}
				CurShape = gPhysicsFactory->CreatePlaneShape(norm, planeConst);

				nPhysics::iRigidBody* rigidBody = gPhysicsFactory->CreateRigidBody(def, CurShape);
				CurModel->rigidBody = rigidBody;
				gPhysicsWorld->AddBody(rigidBody);

			}
			else if (type == "CYLINDER")
			{

				nPhysics::iShape* CurShape = NULL;
				nPhysics::sRigidBodyDef def;

				//in Radians
				def.Position = CurModel->position;
				def.Mass = GameObject[i]["RigidBody"]["Mass"].GetFloat();
				def.GameObjectName = CurModel->friendlyName;
				def.quatOrientation = CurModel->m_meshQOrientation;

				glm::vec3 halfExtents;
				if (GameObject[i]["RigidBody"].HasMember("HalfExtents"))
				{
					CurModel->bExtentsFromMesh = false;
					const rapidjson::Value& ExtentsArray = GameObject[i]["RigidBody"]["HalfExtents"];
					for (int i = 0; i < 3; i++)
					{
						halfExtents[i] = ExtentsArray[i].GetFloat();
					}
					halfExtents.x * CurModel->nonUniformScale.x;
					halfExtents.y * CurModel->nonUniformScale.y;
					halfExtents.z * CurModel->nonUniformScale.z;
				}
				else
				{
					halfExtents = glm::vec3(curModelInfo.maxX * CurModel->nonUniformScale.x, curModelInfo.maxY * CurModel->nonUniformScale.y,
						curModelInfo.maxZ * CurModel->nonUniformScale.z);

				}
				CurShape = gPhysicsFactory->CreateBoxShape(halfExtents);

				int axis = GameObject[i]["RigidBody"]["Axis"].GetInt();

				CurShape = gPhysicsFactory->CreateCylinderShape(halfExtents, axis);

				nPhysics::iRigidBody* rigidBody = gPhysicsFactory->CreateRigidBody(def, CurShape);
				CurModel->rigidBody = rigidBody;
				gPhysicsWorld->AddBody(rigidBody);

			}

			if (type == "CYLINDER_TO_POINT")
			{
				nPhysics::iShape* CurShape = NULL;
				nPhysics::sRigidBodyDef def;
				def.quatOrientation = CurModel->m_meshQOrientation;
				//in Radians
				def.Position = CurModel->position;
				def.Mass = GameObject[i]["RigidBody"]["Mass"].GetFloat();
				def.GameObjectName = CurModel->friendlyName;
				const rapidjson::Value& ExtentsArray = GameObject[i]["RigidBody"]["HalfExtents"];
				glm::vec3 hE;
				for (int i = 0; i < 3; i++)
				{
					hE[i] = ExtentsArray[i].GetFloat();
				}
				int axis = GameObject[i]["RigidBody"]["Axis"].GetInt();

				CurShape = gPhysicsFactory->CreateCylinderShape(hE, axis);

				const rapidjson::Value& pivotArray = GameObject[i]["RigidBody"]["Pivot"];
				glm::vec3 pivot;
				for (int i = 0; i < 3; i++)
				{
					pivot[i] = pivotArray[i].GetFloat();
				}


				nPhysics::iRigidBody* rigidBody = gPhysicsFactory->CreateRigidBody(def, CurShape);
				CurModel->rigidBody = rigidBody;
				gPhysicsWorld->AddBody(rigidBody);
				nPhysics::iConstraint* costr = gPhysicsFactory->CreatePointToPointConstraint(rigidBody, pivot);
				gPhysicsWorld->AddConstraint(costr);

			}
			else if (type == "BOX")
			{

				nPhysics::iShape* CurShape = NULL;
				nPhysics::sRigidBodyDef def;
				def.Scale = glm::vec3(CurModel->nonUniformScale.x, CurModel->nonUniformScale.y, CurModel->nonUniformScale.x);
				//in Radians
				def.Position = CurModel->position;
				def.Mass = GameObject[i]["RigidBody"]["Mass"].GetFloat();
				def.GameObjectName = CurModel->friendlyName;
				def.quatOrientation = CurModel->m_meshQOrientation;

				glm::vec3 halfExtents;
				if (GameObject[i]["RigidBody"].HasMember("HalfExtents"))
				{
					CurModel->bExtentsFromMesh = false;
					const rapidjson::Value& ExtentsArray = GameObject[i]["RigidBody"]["HalfExtents"];
					for (int i = 0; i < 3; i++)
					{
						halfExtents[i] = ExtentsArray[i].GetFloat();
					}
					halfExtents.x * CurModel->nonUniformScale.x;
					halfExtents.y * CurModel->nonUniformScale.y;
					halfExtents.z * CurModel->nonUniformScale.z;
				}
				else
				{
					halfExtents = glm::vec3(curModelInfo.maxX * CurModel->nonUniformScale.x, curModelInfo.maxY * CurModel->nonUniformScale.y,
						curModelInfo.maxZ * CurModel->nonUniformScale.z);

				}
				CurShape = gPhysicsFactory->CreateBoxShape(halfExtents);

				nPhysics::iRigidBody* rigidBody = gPhysicsFactory->CreateRigidBody(def, CurShape);
				CurModel->rigidBody = rigidBody;
				gPhysicsWorld->AddBody(rigidBody);

			}

			else if (type == "BOX_HINGE")
			{

				nPhysics::iShape* CurShape = NULL;
				nPhysics::sRigidBodyDef def;

				//in Radians
				def.Position = CurModel->position;
				def.Mass = GameObject[i]["RigidBody"]["Mass"].GetFloat();
				def.GameObjectName = CurModel->friendlyName;
				
				glm::vec3 halfExtents;
				if (GameObject[i]["RigidBody"].HasMember("HalfExtents"))
				{
					const rapidjson::Value& ExtentsArray = GameObject[i]["RigidBody"]["HalfExtents"];
					for (int i = 0; i < 3; i++)
					{
						halfExtents[i] = ExtentsArray[i].GetFloat();
					}
				}
				else
				{
					halfExtents = glm::vec3(curModelInfo.maxX * CurModel->nonUniformScale.x, curModelInfo.maxY * CurModel->nonUniformScale.y,
						curModelInfo.maxZ * CurModel->nonUniformScale.z);

				}



				const rapidjson::Value& pivotAray = GameObject[i]["RigidBody"]["Pivot"];
				glm::vec3 pivot;
				for (int i = 0; i < 3; i++)
				{
					pivot[i] = pivotAray[i].GetFloat();
				}

				const rapidjson::Value& axisArray = GameObject[i]["RigidBody"]["Axis"];
				glm::vec3 axis;
				for (int i = 0; i < 3; i++)
				{
					axis[i] = axisArray[i].GetFloat();
				}

				CurShape = gPhysicsFactory->CreateBoxShape(halfExtents);

				nPhysics::iRigidBody* rigidBody = gPhysicsFactory->CreateRigidBody(def, CurShape);
				CurModel->rigidBody = rigidBody;
				gPhysicsWorld->AddBody(rigidBody);

				nPhysics::iConstraint* costr = gPhysicsFactory->CreatHingeConstraint(rigidBody, pivot, axis);
				gPhysicsWorld->AddConstraint(costr);
			}

			else if (type == "BOX_TO_BOX_HINGE")
			{

			nPhysics::iShape* CurShapeA = NULL;
			nPhysics::iShape* CurShapeB = NULL;
			nPhysics::sRigidBodyDef defA;
			nPhysics::sRigidBodyDef defB;






			const rapidjson::Value& ExtentsArray = GameObject[i]["RigidBody"]["HalfExtents"];
			glm::vec3 hE;
			for (int i = 0; i < 3; i++)
			{
				hE[i] = ExtentsArray[i].GetFloat();
			}

			const rapidjson::Value& pivotA_Aray = GameObject[i]["RigidBody"]["PivotA"];
			glm::vec3 pivotA;
			for (int i = 0; i < 3; i++)
			{
				pivotA[i] = pivotA_Aray[i].GetFloat();
			}

			const rapidjson::Value& pivotB_Aray = GameObject[i]["RigidBody"]["PivotB"];
			glm::vec3 pivotB;
			for (int i = 0; i < 3; i++)
			{
				pivotB[i] = pivotB_Aray[i].GetFloat();
			}

			const rapidjson::Value& axisArray = GameObject[i]["RigidBody"]["Axis"];
			glm::vec3 axis;
			for (int i = 0; i < 3; i++)
			{
				axis[i] = axisArray[i].GetFloat();
			}

			const rapidjson::Value& offsetArray = GameObject[i]["RigidBody"]["Offset"];
			glm::vec3 offset;
			for (int i = 0; i < 3; i++)
			{
				offset[i] = offsetArray[i].GetFloat();
			}
			//in Radians
			defA.Position = CurModel->position;
			defA.Mass = GameObject[i]["RigidBody"]["Mass"].GetFloat();
			defA.GameObjectName = CurModel->friendlyName;
			defB.Position = CurModel->position + offset;
			defB.GameObjectName = CurModel->friendlyName + "second";
			defB.Mass = defA.Mass;

			CurShapeA = gPhysicsFactory->CreateBoxShape(hE);
			CurShapeB = gPhysicsFactory->CreateBoxShape(hE);

			cGameObject* CurModel2 = new cGameObject();
			CurModel2->position = defB.Position;
			CurModel2->friendlyName = CurModel->friendlyName + "second";
			CurModel2->meshName = CurModel->meshName;
			CurModel2->setSpecularPower(CurModel->materialSpecular.a);
			CurModel2->setDiffuseColour(glm::vec3(0.f));
			CurModel2->vecTextures = CurModel->vecTextures;
			CurModel2->nonUniformScale = CurModel->nonUniformScale;

			nPhysics::iRigidBody* rigidBodyA = gPhysicsFactory->CreateRigidBody(defA, CurShapeA);
			nPhysics::iRigidBody* rigidBodyB = gPhysicsFactory->CreateRigidBody(defB, CurShapeB);
			CurModel->rigidBody = rigidBodyA;
			CurModel2->rigidBody = rigidBodyB;
	
			gPhysicsWorld->AddBody(rigidBodyA);
			gPhysicsWorld->AddBody(rigidBodyB);

			nPhysics::iConstraint* constr = gPhysicsFactory->CreatHingeConstraint(rigidBodyA, rigidBodyB,
				pivotA, pivotB, axis, axis);
			gPhysicsWorld->AddConstraint(constr);

			vec_pObjectsToDraw.push_back(CurModel2);
			}


			else if (type == "CAPSULE")
			{

				nPhysics::iShape* CurShape = NULL;
				nPhysics::sRigidBodyDef def;

				//in Radians
				def.Position = CurModel->position;
				def.Mass = GameObject[i]["RigidBody"]["Mass"].GetFloat();
				def.GameObjectName = CurModel->friendlyName;

				float radius = GameObject[i]["RigidBody"]["Radius"].GetFloat();
				float height = GameObject[i]["RigidBody"]["Height"].GetFloat();

				int axis = GameObject[i]["RigidBody"]["Axis"].GetInt();

				CurShape = gPhysicsFactory->CreateCapsuleShape(height, radius, axis);

				if (GameObject[i]["RigidBody"].HasMember("IsPlayer")) 
				{
					if (GameObject[i]["RigidBody"]["IsPlayer"].GetBool() == true) 
					{
						def.isPlayer = true;
						CurModel->bIsPlayer = true;
					}
				}

				nPhysics::iRigidBody* rigidBody = gPhysicsFactory->CreateRigidBody(def, CurShape);
				CurModel->rigidBody = rigidBody;
				gPhysicsWorld->AddBody(rigidBody);

			}

		}

		vec_pObjectsToDraw.push_back(CurModel);

	
	}



	for (rapidjson::SizeType i = 0; i < LightObject.Size(); i++) {

		sLight* CurLight = new sLight();

		CurLight->lightName = LightObject[i]["Name"].GetString();
		CurLight->SetLightType(LightObject[i]["Type"].GetString());
		if (LightObject[i].HasMember("Angles"))	{
			const rapidjson::Value& AngleArray = LightObject[i]["Angles"];
			CurLight->SetSpotConeAngles(AngleArray[0].GetFloat(), AngleArray[1].GetFloat());
		}

		if (LightObject[i].HasMember("Direction")) {
			const rapidjson::Value& DirectionArray = LightObject[i]["Direction"];
			for (rapidjson::SizeType i = 0; i < DirectionArray.Size(); i++) {
				CurLight->direction[i] = DirectionArray[i].GetFloat();
			}
		}

		
		const rapidjson::Value& PositionArray = LightObject[i]["Position"];
		for (rapidjson::SizeType i = 0; i < PositionArray.Size(); i++) {
			CurLight->position[i] = PositionArray[i].GetFloat();
		}
		if (LightObject[i].HasMember("ObjectLookAt")) {
			cGameObject* LookAtObj = findObjectByFriendlyName(LightObject[i]["ObjectLookAt"].GetString());
			if (LookAtObj != NULL) {
				CurLight->SetRelativeDirectionByLookAt(LookAtObj);
			}
			else {
				std::cout << "LookAt obj not found: " << LightObject[i]["ObjectLookAt"].GetString() << std::endl;
			}
		}
		const rapidjson::Value& AttenArray = LightObject[i]["Attenuation"];
		for (rapidjson::SizeType i = 0; i < AttenArray.Size(); i++) {
			CurLight->atten[i] = AttenArray[i].GetFloat();
		}
		const rapidjson::Value& DiffuseArray = LightObject[i]["DiffuseRGB_Alpha"];
		for (rapidjson::SizeType i = 0; i < DiffuseArray.Size(); i++) {
			CurLight->diffuse[i] = DiffuseArray[i].GetFloat();
		}
		std::string turn = LightObject[i]["Turned"].GetString();
		if (turn == "ON") {
			CurLight->param2.x = 1.0f;
		}
		else{
			CurLight->param2.x = 0.0f;
		}

		LightManager->vecLights.push_back(CurLight);
		
	}

	


	return true;
}