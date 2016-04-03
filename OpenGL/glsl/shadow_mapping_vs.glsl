#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec3 vertexNormal_modelspace;

// Output data ; will be interpolated for each fragment.
//out vec3 FragmentColor;

out VS_OUT{
	vec3 FragPos;
    vec3 Normal;
    vec3 FragmentColor;
    vec4 FragPosLightSpace;
} vs_out;

// Values that stay constant for the whole mesh.
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform mat4 Matrix_Worldspace_to_Lightspace;

void main(){
    gl_Position = projection * view * model * vec4(vertexPosition_modelspace, 1.0f);
    vs_out.FragPos = vec3(model * vec4(vertexPosition_modelspace, 1.0));
    //#TODO Ç»Ç∫ãtçsóÒÇì]ínÇ∑ÇÈÇÃÇ©í≤Ç◊ÇÈ
	vs_out.Normal = transpose(inverse(mat3(model))) * vertexNormal_modelspace;
	//vs_out.Normal = (inverse(mat3(model))) * vertexNormal_modelspace;
    vs_out.FragmentColor = vertexColor;
    vs_out.FragPosLightSpace = Matrix_Worldspace_to_Lightspace * vec4(vs_out.FragPos, 1.0);	
}
