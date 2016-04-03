#version 330 core


// Output data ; will be interpolated for each fragment.
//out vec4 Fragment_color;
//out vec3 Position_worldspace;
//out vec3 Normal_cameraspace;
//out vec3 EyeDirection_cameraspace;
//out vec3 LightDirection_cameraspace;

// Interpolated values from the vertex shaders
in vec3 Fragment_color;
in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;

// Ouput data
out vec3 color;

//�e�N�X�`�����g��Ȃ��̂�TextureSampler�͎g��Ȃ�
// Values that stay constant for the whole mesh.
//uniform sampler2D myTextureSampler;
uniform mat4 MV;
uniform vec3 LightPosition_worldspace;

void main(){

	// Light emission properties
	// You probably want to put them as uniforms
	vec3 LightColor = vec3(1,1,1);
	float LightPower = 1200.0f;
	
	// Material properties
	//use fragment color as DiffuseColor from Vertex shader
	vec3 MaterialDiffuseColor = Fragment_color;
	vec3 MaterialAmbientColor = vec3(0.5, 0.5, 0.5) * MaterialDiffuseColor;
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
	
	vec3 cc = 
		// Ambient : simulates indirect lighting
		MaterialAmbientColor +
		// Diffuse : "color" of the object
		MaterialDiffuseColor * LightColor * LightPower * cosTheta / (distance * distance) +
		// Specular : reflective highlight, like a mirror
		MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha,5) / (distance * distance);

	//color = 
	//	// Ambient : simulates indirect lighting
	//	MaterialAmbientColor +
	//	// Diffuse : "color" of the object
	//	MaterialDiffuseColor * LightColor * LightPower * cosTheta / (distance*distance) +
	//	// Specular : reflective highlight, like a mirror
	//	MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha,5) / (distance*distance);
	color = cc * 1.3; //���傢�Ɩ��邭����K��

}