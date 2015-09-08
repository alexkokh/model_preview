#define FREEGLUT_STATIC
#pragma warning ( disable: 4099 )
#include <stdio.h>
#include <Windows.h>
#include "model.h"
#include "vmath.h"
#include "LoadShaders.h"
#include "DAEReader.h"

using namespace vmath;
using namespace DAE_READER;
using namespace SE_CORE;

#define BUFFER_OFFSET(offset) ((void *)(offset))

GLuint program = 0;

float yaw = 0.0f;
float roll = 0;
float pitch = 0;
int window_width = 1600;
int window_height = 1200;

mat4 model_rotate;
vec4 dir(0, 0, 1, 0);
vec3 position(0, 0, 0);

float xpos = 0.0f;
float zpos = 7.0f;
float cam_yaw = 0.0f;
float cam_pitch = 0.0f;
int mouse_x_prev = 0;
int mouse_y_prev = 0;

float f;
vec3 cam_pos(20, 4, 20);
vec3 cam_lookat(0, 0, 0);

const GLuint numTextures = 2;
enum VAO_IDs { Triangles, NumVAOs };
enum Buffer_IDs { ArrayBuffer, ElementBuffer, NumBuffers };
enum Attrib_IDs { vPosition = 0, vTexCoord };
GLuint Textures[numTextures];
GLuint VAOs[NumVAOs];
GLuint Buffers[NumBuffers];
const GLuint NumVertices = 6;
quaternion orientation(0, 0, 0, 1);
quaternion cam_orientation(0, 0, 0, 1);

model *m = NULL;

mat4 LookAtRH(vec3 eye, vec3 target, vec3 up)
{
	vec3 zaxis = normalize(eye - target);    // The "forward" vector.
	vec3 xaxis = normalize(cross(up, zaxis));// The "right" vector.
	vec3 yaxis = cross(zaxis, xaxis);     // The "up" vector.

	// Create a 4x4 orientation matrix from the right, up, and forward vectors
	// This is transposed which is equivalent to performing an inverse 
	// if the matrix is orthonormalized (in this case, it is).
	mat4 orientation = {
		vec4(xaxis[0], yaxis[0], zaxis[0], 0),
		vec4(xaxis[1], yaxis[1], zaxis[1], 0),
		vec4(xaxis[2], yaxis[2], zaxis[2], 0),
		vec4(0, 0, 0, 1)
	};

	// Create a 4x4 translation matrix.
	// The eye position is negated which is equivalent
	// to the inverse of the translation matrix. 
	// T(v)^-1 == T(-v)
	mat4 translation = {
		vec4(1, 0, 0, 0),
		vec4(0, 1, 0, 0),
		vec4(0, 0, 1, 0),
		vec4(-eye[0], -eye[1], -eye[2], 1)
	};

	// Combine the orientation and translation to compute 
	// the final view matrix
	return (orientation * translation);
}

mat4 FPSViewRH(vec3 pos, vec3 euler_angles)
{
	mat4 cam = rotate(euler_angles[0], vec3(0, 1, 0)); // yaw
	cam *= translate(vec3(-pos[0], 0, -pos[2]));

	return cam;
}

quaternion get_offset(vec3 axis, float angle)
{
	axis = normalize(axis);
	axis *= sin(angle / 2.0f);
	float scalar = cos(angle / 2.0f);
	quaternion offset = quaternion(axis[0], axis[1], axis[2], scalar);

	return offset;
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);

	GLint pos_modelview, pos_projection, pos_normal;
	pos_modelview = glGetUniformLocation(program, "modelview");
	pos_projection = glGetUniformLocation(program, "projection");
	pos_normal = glGetUniformLocation(program, "NormalMatrix");

	quaternion q = quaternion(1, vec3(0, 0, 0));
	model_rotate = orientation.asMatrix();

	mat4 model_translate = translate(position);
	mat4 model, view;


	
	//view = FPSViewRH(vec3(xpos, 0, zpos), vec3(cam_yaw, 0, 0));
	mat4 cam_rotate = cam_orientation.asMatrix();
	view = LookAtRH(cam_pos, cam_lookat, vec3(0, 1, 0));
	model = model_translate * model_rotate;

	mat4 modelview = view * model;
	mat4 projection = perspective(45.0f, (float)window_width / window_height, 1.0f, 200000.0f);

	glUniformMatrix4fv(pos_modelview, 1, false, modelview);
	glUniformMatrix4fv(pos_projection, 1, false, projection);
	glUniformMatrix4fv(pos_normal, 1, false, model);
	int err;
	err = glGetError();

	m->draw();
	err = glGetError();

	glFlush();
}

void init(void)
{
	ShaderInfo shaders[] = {
		{ GL_VERTEX_SHADER, "vs.glsl" },
		{ GL_FRAGMENT_SHADER, "fs.glsl" },
		{ GL_NONE, NULL }
	};

	m = new model("mdl.dae");	
	program = LoadShaders(shaders);
	glUseProgram(program);

	GLint ambientLoc = glGetUniformLocation(program, "Ambient");
	vec4 ambient(.6f, .6f, .6f, 1.f);
	glUniform4fv(ambientLoc, 1, ambient);

	GLint lightDirectionLoc = glGetUniformLocation(program, "LightDirection");
	vec3 lightDirection = normalize(vec3(3, 1, 2));
	glUniform3fv(lightDirectionLoc, 1, lightDirection);
	
	glEnable(GL_DEPTH_TEST);
	glClearColor(.15f, .35f, .75f, 0.f);
	glPointSize(5);
}

void rotateCamera(vec3& camPos, vec3& camLookAt, float xzDelta, float yDelta)
{
	quaternion offset;
	vec3 dif = camPos - camLookAt;
	float delta = abs(.04f * dif[1]); // 4% allowance

	offset = get_offset(vec3(0,1,0), xzDelta);

	// prevent the XZ plane from flipping due camera vertical orientation
	if (abs(dif[0]) < delta && abs(dif[2]) < delta &&
		((dif[1] > 0 && yDelta > 0) ||
		 (dif[1] < 0 && yDelta < 0)))
	{
		yDelta = 0;
	}
	else
	{
		vec3 v = normalize(vec3(camPos[0], 0, camPos[2]));
		vec3 perpendicularAxis = cross(vec3(0, 1, 0), v); // axis, perpendicular to the camera vector in XZ plane

		quaternion yOffset = get_offset(perpendicularAxis, yDelta);
		offset = offset * yOffset;
	}

	mat4 pos;
	pos = offset.asMatrix() * vec4(camPos, 1);
	camPos[0] = pos[0][0];
	camPos[1] = pos[0][1];
	camPos[2] = pos[0][2];
}

void cleanup()
{
	if (m)
		delete m;
}

void keyboard_callback(unsigned char key, int x, int y)
{
	float angle = 0;
	vec3 axis(0, 0, 0);
	mat4 v;
	quaternion offset;

	switch (key)
	{
	case 111:
		v = model_rotate * dir;
		position += vec3(v[0][0], v[0][1], v[0][2]);
		break;

	case 108:
		v = model_rotate * dir;
		position -= vec3(v[0][0], v[0][1], v[0][2]);
		break;

	case 27:
		cleanup();
		glutLeaveMainLoop();
		break;

	case 97:
		axis = vec3(0, 1, 0);
		angle = -.1f;
		offset = get_offset(axis, angle);
		orientation = offset * orientation;
		orientation = normalize(orientation);
		break;

	case 100:
		axis = vec3(0, 1, 0);
		angle = .1f;
		offset = get_offset(axis, angle);
		orientation = offset * orientation;
		orientation = normalize(orientation);
		break;

	case 113:
		axis = vec3(0, 0, 1);
		angle = .1f;
		offset = get_offset(axis, angle);
		orientation = offset * orientation;
		orientation = normalize(orientation);
		break;

	case 101:
		axis = vec3(0, 0, 1);
		angle = -.1f;
		offset = get_offset(axis, angle);
		orientation = offset * orientation;
		orientation = normalize(orientation);
		break;

	case 119:
		axis = vec3(1, 0, 0);
		angle = -.1f;
		offset = get_offset(axis, angle);
		orientation = offset * orientation;
		orientation = normalize(orientation);
		break;

	case 115:
		axis = vec3(1, 0, 0);
		angle = .1f;
		offset = get_offset(axis, angle);
		orientation = offset * orientation;
		orientation = normalize(orientation);
		break;
	
	default:
		break;
	}

	printf("z: %1.1f\n", zpos);
	glutPostRedisplay();
}

void timer_callback(int value)
{
	//obj_yangle += 1;
	glutPostRedisplay();
	glutTimerFunc(30, timer_callback, 0);
}

void mouse_callback(int x, int y, int l0, int l1)
{
	if (x == 3)
	{
		cam_pos -= normalize(cam_pos - cam_lookat);
		glutPostRedisplay();
	}

	if (x == 4)
	{
		cam_pos += normalize(cam_pos - cam_lookat);
		glutPostRedisplay();
	}

}

void motion_callback(int x, int y)
{
	float xDelta = 0;
	float yDelta = 0;
	int xDiff = mouse_x_prev - x;
	int yDiff = mouse_y_prev - y;

	if ((xDiff) > 0)
	{
		xDelta = -.05f;
	}
	else if ((xDiff) < 0)
	{
		xDelta = +.05f;
	}

	if ((yDiff) > 0)
	{
		yDelta = -.05f;
	}
	else if ((yDiff) < 0)
	{
		yDelta = +.05f;
	}

	rotateCamera(cam_pos, cam_lookat, xDelta, yDelta);

	mouse_x_prev = x;
	mouse_y_prev = y;

	glutPostRedisplay();
}

void reshape(int w, int h)
{
	window_width = w;
	window_height = h;
	glViewport(0, 0, w, h);
	glutPostRedisplay();
}

int main(int argc, char* argv[])
{
	SetProcessDPIAware();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(1600, 1200);
	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("Preview");

	if (glewInit())
	{
		printf("error\n");
		exit(EXIT_FAILURE);
	}

	init();

	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard_callback);
	glutMouseFunc(mouse_callback);
	glutMotionFunc(motion_callback);
	glutTimerFunc(30, timer_callback, 0);
	glutMainLoop();

	return 0;
}

