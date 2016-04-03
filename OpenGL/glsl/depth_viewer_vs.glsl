#version 330 core
layout (location = 0) in vec3 vertexPosition_modelspace;

uniform mat4 Matrix_Worldspace_to_Lightspace;
uniform mat4 model;

/*
	Simple depth shader
	We use the simple shader that onky transfroms the vertices to light space from model space.
	we don't require any processing in a fragment buffer, as we have no color buffer, so we use an empty fragment buffer.
*/

void main()
{
    gl_Position = Matrix_Worldspace_to_Lightspace * model * vec4(vertexPosition_modelspace, 1.0f);
} 