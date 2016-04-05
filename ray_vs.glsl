#version 330 core
layout(location = 0) in vec4 vPosition;

uniform mat4 modelview;
uniform mat4 projection;

void
main()
{
	gl_Position = projection * modelview * vPosition;
}