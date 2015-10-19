#version 330 core
layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec2 vTexCoord;

out vec2 tex_coord;

uniform mat4 modelview;
uniform mat4 projection;
uniform mat4 NormalMatrix;

void
main()
{
	gl_Position = projection * modelview * vPosition;
	tex_coord = vTexCoord;
}