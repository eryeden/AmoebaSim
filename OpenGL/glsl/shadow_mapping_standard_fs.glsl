#version 330 core


// Interpolated values from the vertex shaders
in vec3 Fragment_color;
in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;
in vec4 FragmentPosition_lightspace;

// Ouput data
out vec3 color;

//テクスチャを使わないのでTextureSamplerは使わない
// Values that stay constant for the whole mesh.
uniform vec3 LightPosition_worldspace;

//Depthmap
uniform sampler2D shadowMap;

float ShadowCalculation(vec4 fragPosLightSpace)
{
	float bias = 0.0002;
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    // Get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // Check whether current frag pos is in shadow
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
		
	//テクスチャ外だったら影でないとする
	if(projCoords.z > 1.0){
        shadow = 0.0;
	}

    return shadow;
}

float ShadowCalculation_depth(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    // Get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // Check whether current frag pos is in shadow
    float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;
    return closestDepth;
}


void main(){
    // Light emission properties
	// You probably want to put them as uniforms
	vec3 LightColor = vec3(1,1,1);
    float LightPower = 4000.0f;
    // Material properties
	//use fragment color as DiffuseColor from Vertex shader
	vec3 MaterialDiffuseColor = Fragment_color;
    vec3 MaterialAmbientColor = vec3(0.2, 0.2, 0.2) * MaterialDiffuseColor;
    //vec3(0.1,0.1,0.1) * MaterialDiffuseColor;
	vec3 MaterialSpecularColor = vec3(0.3,0.3,0.3);
    // Distance to the light
	float distance = length( LightPosition_worldspace - Position_worldspace );
    // Normal of the computed fragment, in camera space
	vec3 n = normalize( Normal_cameraspace );
    // Direction of the light (from the fragment to the light)
	vec3 l = normalize( LightDirection_cameraspace );
    // Cosine of the angle between the normal and the light direction, 
	// clamped above 0
	//  - light is at the vertical of the triangle -> 1
	//  - light is perpendicular to the triangle -> 0
	//  - light is behind the triangle -> 0
	float cosTheta = clamp( dot( n,l ), 0,1 );
    // Eye vector (towards the camera)
	vec3 E = normalize(EyeDirection_cameraspace);
    // Direction in which the triangle reflects the light
	vec3 R = reflect(-l,n);
    // Cosine of the angle between the Eye vector and the Reflect vector,
	// clamped to 0
	//  - Looking into the reflection -> 1
	//  - Looking elsewhere -> < 1
	float cosAlpha = clamp( dot( E,R ), 0,1 );

	//Shadow
    float shadow = ShadowCalculation(FragmentPosition_lightspace);

  //  vec3 cc = 
		//// Ambient : simulates indirect lighting
		//MaterialAmbientColor +
		//// Diffuse : "color" of the object
		//MaterialDiffuseColor * LightColor * LightPower * cosTheta / (distance * distance) +
		//// Specular : reflective highlight, like a mirror
		//MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha,5) / (distance * distance);
  
    vec3 cc = 

		// Ambient : simulates indirect lighting
		MaterialAmbientColor +

		(1.0 - shadow) * 
		(
		// Diffuse : "color" of the object
		MaterialDiffuseColor * LightColor * LightPower * cosTheta / (distance * distance) +
		// Specular : reflective highlight, like a mirror
		MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha,5) / (distance * distance)
		);

	//vec3 cc = vec3(ShadowCalculation_depth(FragmentPosition_lightspace));
  
    //color = 
	//	// Ambient : simulates indirect lighting
	//	MaterialAmbientColor +
	//	// Diffuse : "color" of the object
	//	MaterialDiffuseColor * LightColor * LightPower * cosTheta / (distance*distance) +
	//	// Specular : reflective highlight, like a mirror
	//	MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha,5) / (distance*distance);
	if(shadow == 0.0){
		color = cc * 1.1;
	}else{
		color = cc * 0.9;
	}
    //ちょいと明るくする適当

}