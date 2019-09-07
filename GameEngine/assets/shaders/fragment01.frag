#version 420 
//fragment01.glsl




struct sGSOut
{
	vec4 color;		
	vec4 vertPosWorld;	// "World space"
	vec4 vertNormal;	// "Model space"
	vec4 vertUV_x2;		// To the next shader stage
  vec4 viewSpace;     //For fog
  vec4 TangentLightPos; 
  vec4 TangentViewPos;
  vec4 TangentFragPos;
};

in sGSOut gsOuput;



uniform vec4 objectDiffuse;		// becomes objectDiffuse in the shader
uniform vec4 objectSpecular;	// rgb + a, which is the power)
uniform float texTiling;
uniform vec3 eyeLocation;		// This is in "world space"  

// Set this to true (1), and the vertex colour is used
uniform bool useVertexColour;

// If you want this, go ahead and use this.
// You would:
// - Make an "ambient" value that's diffuse * diffuseToAmbientRatio
// - Then, calculate the final colour with ONLY lighting.
// - If this value is LESS than the ambient, use the ambient.
uniform float diffuseToAmbientRatio;		// 0.2 

uniform bool bDontUseLighting;		

// If this is true, then we are using the smoke imposter
// (the shader will be changed a little)
uniform bool bIsParticleImposter;
// Used to fade the particle during "death" (set to 1.0f if not being used)
uniform float ParticleImposterBlackThreshold;
uniform float ParticleImposterAlphaOverride;	


//vec4 gl_FragColor
//out vec4 finalOutputColour;		// Any name, but must be vec4
//out vec4 finalOutputColour[3];		// Any name, but must be vec4
	// Colour was #0
	// Normal was #1
	// Vertex World Location #2
// Or list them in the order that they will be used
out vec4 finalOutputColour;			// GL_COLOR_ATTACHMENT0
out vec4 finalOutputNormal;			// GL_COLOR_ATTACHMENT1
out vec4 finalOutputVertWorldPos;	// GL_COLOR_ATTACHMENT2


struct sLight
{
	vec4 position;			
	vec4 diffuse;	
	vec4 specular;	// rgb = highlight colour, w = power
	vec4 atten;		// x = constant, y = linear, z = quadratic, w = DistanceCutOff
	vec4 direction;	// Spot, directional lights
	vec4 param1;	// x = lightType, y = inner angle, z = outer angle, w = TBD
	                // 0 = pointlight
					// 1 = spot light
					// 2 = directional light
	vec4 param2;	// x = 0 for off, 1 for on
};

const int POINT_LIGHT_TYPE = 0;
const int SPOT_LIGHT_TYPE = 1;
const int DIRECTIONAL_LIGHT_TYPE = 2;

const int NUMBEROFLIGHTS = 20;
uniform sLight theLights[NUMBEROFLIGHTS];  	// 80 uniforms

uniform sampler2D texture00;
uniform sampler2D texture01;
uniform sampler2D texture02;
uniform sampler2D texture03;
uniform sampler2D texture04;
uniform sampler2D texture05;
uniform sampler2D texture06;
uniform sampler2D texture07;

//normal 
uniform sampler2D texNormalMap;

// For the 2 pass rendering
uniform float renderPassNumber;	// 1 = 1st pass, 2nd for offscreen to quad
uniform sampler2D texPass1OutputTexture;
uniform sampler2D texPass1OutputTexture2;
uniform vec2 FBOtexBlendWeights;
uniform float time;


// Cube map texture (NOT a sampler3D)
uniform samplerCube textureSkyBox;
uniform bool useSkyBoxTexture;

// 
uniform bool bAddReflect;		// Add reflection
uniform bool bAddRefract;		// Add refraction
uniform float refractionIndex;
uniform bool bUseHeightMap;	
uniform bool bUseNormalMap;
// This is 4 x 2 floats or 8 floats
uniform vec4 texBlendWeights[2];	// x is 0, y is 1, z is 2

uniform float wholeObjectAlphaTransparency;

//Copying UV to mult by tile value

uniform float FogDensity;
const vec3 fogColor = vec3(0.5, 0.5,0.5);
//const float FogDensity = 0.008;
//const float FogDensity = 0.008;
float ambientAmount = 0.4;
void main()
{

// In from a previous stage (geom shader)
	vec4 color         = gsOuput.color;			                   // in from the vertex shader
	vec4 vertPosWorld  = gsOuput.vertPosWorld;
	vec4 vertUV_x2		 = gsOuput.vertUV_x2;               // Texture coordinates
	vec4 viewSpace     = gsOuput.viewSpace;
	vec4 tiledUV 			 = vertUV_x2;  
  // 
	// 






	tiledUV *= texTiling;
	vec4 vertNormal;
	vec3 normal;

	if ( bUseHeightMap )
	{

		float newTime = time * 0.001;
		tiledUV.s + newTime;
		tiledUV.t + newTime;

		
		normal = texture(texNormalMap, vec2(tiledUV.s + newTime, tiledUV.t + newTime)).rgb;
		vertNormal.rgb  = normalize(normal * 2.0 - 1.0).rgb;

	
	}
	else
	{
	if(bUseNormalMap)
	{
	normal = texture(texNormalMap, tiledUV.st).rgb;
	vertNormal.rgb  = normalize(normal * 2.0 - 1.0).rgb;
	}
	else
	{
		vertNormal = gsOuput.vertNormal;
	}

	}


	




	// output black to all layers
	finalOutputColour = vec4( 0.0f, 0.0f, 0.0f, 1.0f );
	finalOutputNormal = vec4( 0.0f, 0.0f, 0.0f, 1.0f );
	finalOutputVertWorldPos = vertPosWorld;


	// Are we in the 2nd pass? 
	if ( int(renderPassNumber) == 2 )
	{ 
		// float newTime = time * 0.03;
		// vec4 FBO_Texture1 = texture( texPass1OutputTexture, vertUV_x2.st);
		
		
		// vec4 FBO_Texture2 = texture( texture00, vec2(vertUV_x2.s + newTime, vertUV_x2.t) );
		// //finalOutputColour.rgb = texture( texPass1OutputTexture, vertUV_x2.st).rgb;

		
		
		
		// finalOutputColour = (FBO_Texture1 * FBOtexBlendWeights.x) 	 
		//                   + (FBO_Texture2 * FBOtexBlendWeights.y);     
		
		
		// finalOutputColour.a = 1.0f;
		// return;
	}
	
	
	// We are in the 1st (main) pass

	






	
	vec4 materialDiffuse = vec4(0.0f, 0.0f, 0.0f, 1.0f);

	vec4 materialSpecular = objectSpecular;



//Fog**********


float dist = 0;
float fogFactor = 0;
dist = length(viewSpace);





	// is this the skybox texture?
	if (useSkyBoxTexture == true)
	{

		vec3 skyPixelColour = texture( textureSkyBox, gsOuput.vertNormal.xyz ).rgb;
		
		finalOutputColour.rgb = skyPixelColour;
		finalOutputColour.a = 1.0f;
		
		finalOutputNormal.rgb = gsOuput.vertNormal.xyz;
		finalOutputNormal.a = 1.0f;


   fogFactor = 1.0 /exp( (dist * FogDensity)* (dist * FogDensity));
   fogFactor = clamp( fogFactor, 0.1, 1.0 );
 
   finalOutputColour.rgb = mix(fogColor, finalOutputColour.rgb, fogFactor);

		return;
	}
	

	

	// Reflection and refraction if you want to use these later...
	vec3 rgbReflect = vec3(0.0f,0.0f,0.0f);
	vec3 rgbRefract = vec3(0.0f,0.0f,0.0f);
	
	// Add a FULLY reflective object 
	if ( bAddReflect )
	{
		// Calculate ray from vertex to eye
		vec3 viewVector = eyeLocation.xyz - vertPosWorld.xyz;
		viewVector = normalize(viewVector); 
		
		// Calcualte the reflection vector
		vec3 vReflect = reflect(viewVector, vertNormal.xyz);
		
		rgbReflect = texture( textureSkyBox, vReflect ).rgb;
	}//if ( bAddReflect )
	
	if (bAddRefract)
	{
		// Calculate ray from vertex to eye
		vec3 viewVector = eyeLocation.xyz - vertPosWorld.xyz;
		viewVector = normalize(viewVector); 
		
		// Calcualte the reflection vector
		vec3 vRefract = refract(viewVector, vertNormal.xyz, 1.10);
		
		rgbRefract = texture( textureSkyBox, vRefract ).rgb;
	}//if (bAddRefract)	
	
	
	// You could add the reflection and refraction colours to your
	// object's colour, but here I'm going to combine and exit early.
//	if ( bAddReflect || bAddRefract )
//	{
//		float amountOfReflect = 0.5f;
//		float amountOfRefract = 0.5f;
//	
//		finalOutputColour.rgb =   (rgbReflect * amountOfReflect)
//		                        + (rgbRefract * amountOfRefract);
//		finalOutputColour.a = 1.0f;	
//		return;	
//	}
	
	

	if ( useVertexColour )
	{
		//gl_FragColor = vec4(color, 1.0);
		materialDiffuse = color;
	}
	else
	{
		//gl_FragColor = vec4(objectColour, 1.0);
//		materialDiffuse = objectDiffuse;

		vec4 tex0Col = texture( texture00, tiledUV.st ).rgba;
		vec4 tex1Col = texture( texture01, tiledUV.st ).rgba;
		vec4 tex2Col = texture( texture02, tiledUV.st ).rgba;
		vec4 tex3Col = texture( texture03, tiledUV.st ).rgba;
		vec4 tex4Col = texture( texture04, tiledUV.st ).rgba;
		vec4 tex5Col = texture( texture05, tiledUV.st ).rgba;
		vec4 tex6Col = texture( texture06, tiledUV.st ).rgba;
		vec4 tex7Col = texture( texture07, tiledUV.st ).rgba;
		
		materialDiffuse =  objectDiffuse
						  + (tex0Col * texBlendWeights[0].x) 	 // 0
		                  + (tex1Col * texBlendWeights[0].y)     // 1
						  + (tex2Col * texBlendWeights[0].z)     // 2
		                  + (tex3Col * texBlendWeights[0].w)     // 3
		                  + (tex4Col * texBlendWeights[1].x)     // 4
		                  + (tex5Col * texBlendWeights[1].y)     // 5
		                  + (tex6Col * texBlendWeights[1].z)     // 6
		                  + (tex7Col * texBlendWeights[1].w);    // 7
		
	}//if ( useVertexColour )
	
	// Is this being lit? 
	if ( bDontUseLighting )
	{
		// Just exit early
		//finalOutputColour = objectDiffuse;
		finalOutputColour = materialDiffuse;
		
		finalOutputNormal.rgb = vertNormal.xyz;
		finalOutputNormal.a = 1.0f;

		return;
	}
	
	
//theLights[0].position;
//theLights[0].diffuseColour;
//theLights[0].atten;

	vec3 norm = normalize(vertNormal.xyz);

	vec4 finalObjectColour = vec4(0.0,0.0,0.0,1.0);
	
	
	for ( int index = 0; index < NUMBEROFLIGHTS; index++ )
	{	
		// ********************************************************
		// is light "on"
		if ( theLights[index].param2.x == 0.0f )
		{	// it's off
			continue;
		}

		vec3 lightDiffuseContrib;
			

		 // Specular 
		vec3 lightSpecularContrib;

		vec3 LightDirection;
		vec3 eyeVector;
		//if normal map
		if(bUseNormalMap)
		{
			LightDirection = normalize(gsOuput.TangentLightPos - gsOuput.TangentFragPos).rgb;
			eyeVector = normalize(gsOuput.TangentViewPos - gsOuput.TangentFragPos).rgb;
		}
		else
		{
			LightDirection = theLights[index].direction.rgb;
			eyeVector = normalize(eyeLocation.xyz - vertPosWorld.xyz);
		}
		
		// Cast to an int (note with c'tor)
		int intLightType = int(theLights[index].param1.x);
		
		// We will do the directional light here... 
		// (BEFORE the attenuation, since sunlight has no attenuation, really)
		if ( intLightType == DIRECTIONAL_LIGHT_TYPE )		// = 2
		{
			// This is supposed to simulate sunlight. 
			// SO: 
			// -- There's ONLY direction, no position
			// -- Almost always, there's only 1 of these in a scene
			// Cheapest light to calculate. 


			

			// vec3 lightContrib = theLights[index].diffuse.rgb;
			
			// // Get the dot product of the light and normalize
			// float dotProduct = dot(LightDirection.xyz,  
			// 						   normalize(norm.xyz) );	// -1 to 1

			// dotProduct = max( 0.0f, dotProduct );		// 0 to 1
			
			// lightContrib *= dotProduct;		
			
			// finalObjectColour.rgb += (materialDiffuse.rgb * theLights[index].diffuse.rgb * lightContrib); 
			// 						 //+ (materialSpecular.rgb * lightSpecularContrib.rgb);
			// // NOTE: There isn't any attenuation, like with sunlight.
			// // (This is part of the reason directional lights are fast to calculate)

			// // TODO: Still need to do specular, but this gives you an idea
			// finalOutputColour.rgb = finalObjectColour.rgb;
			// finalOutputColour.rgb += materialDiffuse.rgb  * ambientAmount;
			// finalOutputColour.a = wholeObjectAlphaTransparency;

			// Also output the normal to colour #1
			//finalOutputNormal.rgb = vertNormal.xyz;
			//finalOutputNormal.r = 1.0f;
			//finalOutputNormal.a = 1.0f;










		finalObjectColour.rgb += materialDiffuse.rgb;
    // ambient
    // diffuse
    float diff = max(dot(LightDirection.xyz, norm.xyz), 0.0);
    vec3 diffuse = diff * finalObjectColour.rgb;
    // specular
    vec3 viewDir = normalize(gsOuput.TangentViewPos - gsOuput.TangentFragPos).rgb;
    vec3 reflectDir = reflect(-LightDirection.xyz, norm.xyz);
    vec3 halfwayDir = normalize(LightDirection.xyz + viewDir);  
    float spec = pow(max(dot(norm.xyz, halfwayDir), 0.0), 32.0);

		diffuse *= 0.5;
    vec3 specular = vec3(0.2) * spec;
    finalObjectColour = vec4(diffuse + specular, 1.0);

		lightDiffuseContrib = diffuse;
		lightSpecularContrib = specular;






	// // Add any reflection or refraction 
  // 	if ( bAddRefract )
  // 	{
  // 		float amountToAdd = 0.25f;
  // 		// Drop the amount of current colour by a little bit...
  // 		finalOutputColour *= ( 1.0f - amountToAdd );
  // 		// ... and add the refractive colour		
  // 		finalOutputColour.rgb += ( amountToAdd * rgbRefract );
  // 	}
  // 	if ( bAddReflect )
  // 	{
  // 		float amountToAdd = 0.25f;
  // 		// Drop the amount of current colour by a little bit...
  // 		finalOutputColour *= ( 1.0f - amountToAdd );
  // 		// ... and add the reflective colour
  // 		finalOutputColour.rgb += ( amountToAdd * rgbReflect );
  // 	}


	// 				//Apply Fog

  //  	fogFactor = 1.0 /exp( (dist * FogDensity)* (dist * FogDensity));
  //  	fogFactor = clamp( fogFactor, 0.1, 1.0 );
 
  //  	finalOutputColour.rgb = mix(fogColor, finalOutputColour.rgb, fogFactor);


		//	return;		
		}
		else
		{
		
		// Assume it's a point light 
		// // intLightType = 0
		
		// Contribution for this light
		vec3 vLightToVertex = theLights[index].position.xyz - vertPosWorld.xyz;
		float distanceToLight = length(vLightToVertex);	
		vec3 lightVector = normalize(vLightToVertex);
		float dotProduct = dot(lightVector, norm.xyz);	 
		
		dotProduct = max( 0.0f, dotProduct );	
		
		vec3 lightDiffuseContrib = dotProduct * theLights[index].diffuse.rgb;
			

		// Specular 
		vec3 lightSpecularContrib = vec3(0.0f);
			
		vec3 reflectVector = reflect( -lightVector, normalize(norm.xyz) );

		// Get eye or view vector
		// The location of the vertex in the world to your eye
		//vec3 eyeVector = normalize(eyeLocation.xyz - vertPosWorld.xyz);
		

		// To simplify, we are NOT using the light specular value, just the object’s.
		float objectSpecularPower = objectSpecular.w; 
		
		lightSpecularContrib = pow( max(0.0f, dot( eyeVector, reflectVector) ), objectSpecularPower )
			                   * objectSpecular.rgb;	//* theLights[lightIndex].Specular.rgb
					   
		// Attenuation
		float attenuation = 1.0f / 
				( theLights[index].atten.x + 										
				  theLights[index].atten.y * distanceToLight +						
				  theLights[index].atten.z * distanceToLight*distanceToLight );  	
				  
		// total light contribution is Diffuse + Specular
		lightDiffuseContrib *= attenuation;
		lightSpecularContrib *= attenuation;




		// vec3 eyeVector = normalize(gsOuput.TangentViewPos - gsOuput.TangentFragPos).rgb;

		// float objectSpecularPower = objectSpecular.w; 
		// vec3 vLightToVertex = theLights[index].position.xyz - vertPosWorld.xyz;
		// float distance = length(vLightToVertex);	

    // // diffuse shading
    // float diff = max(dot(norm.xyz, LightDirection), 0.0);
    // // specular shading
    // vec3 reflectDir = reflect(-LightDirection, norm.xyz);
    // float spec = pow(max(dot(eyeVector, reflectDir), 0.0), objectSpecularPower);
    // // attenuation
		// float attenuation = 1.0f / 
		// 		( theLights[index].atten.x + 										
		// 		  theLights[index].atten.y * distance +						
		// 		  theLights[index].atten.z * distance*distance );  
    // // combine results
    // lightDiffuseContrib  = theLights[index].diffuse.rgb  * diff;
    // lightSpecularContrib = theLights[index].specular.rgb * spec;















		
		
		// But is it a spot light
		if ( intLightType == SPOT_LIGHT_TYPE )		// = 1
		{	
		

			// Yes, it's a spotlight
			// Calcualate light vector (light to vertex, in world)
			vec3 vertexToLight = vertPosWorld.xyz - theLights[index].position.xyz;

			vertexToLight = normalize(vertexToLight);

			float currentLightRayAngle
					= dot( vertexToLight.xyz, LightDirection.xyz );
					
			currentLightRayAngle = max(0.0f, currentLightRayAngle);

			//vec4 param1;	
			// x = lightType, y = inner angle, z = outer angle, w = TBD

			// Is this inside the cone? 
			float outerConeAngleCos = cos(radians(theLights[index].param1.z));
			float innerConeAngleCos = cos(radians(theLights[index].param1.y));
							
			// Is it completely outside of the spot?
			if ( currentLightRayAngle < outerConeAngleCos )
			{
				// Nope. so it's in the dark
				lightDiffuseContrib = vec3(0.0f, 0.0f, 0.0f);
				lightSpecularContrib = vec3(0.0f, 0.0f, 0.0f);
			}
			else if ( currentLightRayAngle < innerConeAngleCos )
			{
				// Angle is between the inner and outer cone
				// (this is called the penumbra of the spot light, by the way)
				// 
				// This blends the brightness from full brightness, near the inner cone
				//	to black, near the outter cone
				float penumbraRatio = (currentLightRayAngle - outerConeAngleCos) / 
									  (innerConeAngleCos - outerConeAngleCos);
									  
				lightDiffuseContrib *= penumbraRatio;
				lightSpecularContrib *= penumbraRatio;
			}
						
		}// if ( intLightType == 1 )
		
		
					
		 finalObjectColour.rgb += (materialDiffuse.rgb * lightDiffuseContrib.rgb) 
		                          + (materialSpecular.rgb * lightSpecularContrib.rgb);
		// ********************************************************
	}//for(intindex=0...
	
	finalOutputColour.rgb = finalObjectColour.rgb;


	}//else




	finalOutputColour.a = wholeObjectAlphaTransparency;
	
		// Also output the normal to colour #1
	finalOutputNormal.rgb = vertNormal.xyz;
//	finalOutputNormal.r = 1.0f;
	finalOutputNormal.a = 1.0f;

	

	
	// Brigher for the dim projector
//	finalOutputColour.rgb *= 1.25f;

	
	// Add any reflection or refraction 
	if ( bAddRefract )
	{
		float amountToAdd = 0.25f;
		// Drop the amount of current colour by a little bit...
		finalOutputColour *= ( 1.0f - amountToAdd );
		// ... and add the refractive colour		
		finalOutputColour.rgb += ( amountToAdd * rgbRefract );
	}
	if ( bAddReflect )
	{
		float amountToAdd = 0.25f;
		// Drop the amount of current colour by a little bit...
		finalOutputColour *= ( 1.0f - amountToAdd );
		// ... and add the reflective colour
		finalOutputColour.rgb += ( amountToAdd * rgbReflect );
	}
	
	
	// Particle imposter (smoke, fire, water, etc.)
	if ( bIsParticleImposter ) 
	{

		vec4 tex0Col = texture( texture00, vertUV_x2.st ).rgba;	// Imposter texture (smoke, fire)
		float pixelBlackAndWhite = (0.3f * tex0Col.r) + (0.59f * tex0Col.g) + (0.11f * tex0Col.b);
		
		if ( pixelBlackAndWhite < 0.01 )
		{

			discard;
		}	

		finalOutputColour.a = pixelBlackAndWhite * 3.2;
		finalOutputColour.rgb += tex0Col.rgb;
		
		//finalOutputColour.a *= 0.8f;
		
	}



	//Apply ambient
	finalOutputColour.rgb += materialDiffuse.rgb  * ambientAmount;



		//Apply Fog

   fogFactor = 1.0 /exp( (dist * FogDensity)* (dist * FogDensity));
   fogFactor = clamp( fogFactor, 0.1, 1.0 );
 
   finalOutputColour.rgb = mix(fogColor, finalOutputColour.rgb, fogFactor);
	
}