#version 330 core

uniform sampler2D tex0;
uniform sampler2D tex1;
uniform vec4 Ambient;
uniform vec3 LightDirection;

in vec2 tex_coord;
in vec3 normal;

layout (location = 0) out vec4 color;

void main(void)
{
	vec2 uv = vec2(0,0);
	vec4 texColor = texture(tex0, tex_coord);
	float intensity = max(0.2, dot(normal, LightDirection));
	color = min(intensity * Ambient, vec4(1.0));
	//color = .5*texture(tex0, tex_coord).rgba + .5*texture(tex1, tex_coord).rgba;
}