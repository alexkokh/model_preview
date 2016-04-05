#define FREEGLUT_STATIC
#pragma warning ( disable: 4099 )
#include <stdio.h>
#include <Windows.h>
#include "model.h"
#include "font.h"
#include "ray.h"
#include "camera.h"
#include "world.h"
#include "LoadShaders.h"
#include "DAEReader.h"
#include "se_math.h"

using namespace vmath;
using namespace DAE_READER;
using namespace SE_CORE;

uint32_t window = 0;
uint32_t window_width = 1600;
uint32_t window_height = 1200;

mat4 model_rotate;
vec4 dir(0, 0, 1, 0);
vec3 position(0, 0, 0);

uint32_t mouse_x_prev = 0;
uint32_t mouse_y_prev = 0;

quaternion orientation(0, 0, 0, 1);

world *wrld = NULL;
font *textLabel = NULL;

int mouse_buttons[4] = { 0 };

quaternion get_offset(vec3 axis, float angle)
{
	axis = normalize(axis);
	axis *= sin(angle / 2.0f);
	float scalar = cos(angle / 2.0f);
	quaternion offset = quaternion(axis[0], axis[1], axis[2], scalar);

	return offset;
}

void init(void)
{
	ShaderInfo shaders[] = {
		{ GL_VERTEX_SHADER, "vs.glsl" },
		{ GL_FRAGMENT_SHADER, "fs.glsl" },
		{ GL_NONE, NULL }
	};

	wrld = new world();

	float ar = (float)window_width / window_height;
	camera *cam = new camera("camera0", 45.f, ar, 1.f, 200000.f);
	cam->lookAtRH(vec3(0, 20, 50), vec3(0, 0, 0), vec3(0, 1, 0));

	wrld->addCamera(cam);
	wrld->setActiveCamera(cam);

	model *m = new model("model0", shaders, 3, "mdl.dae");

	m->setBoundingVolumeVisible(true);
	wrld->addObject(m);
	wrld->setActiveObject(m);

	if (m)
	{
		GLuint program = m->get_program();
		glUseProgram(program);

		GLint ambientLoc = glGetUniformLocation(program, "Ambient");
		vec4 ambient(.6f, .6f, .6f, 1.f);
		glUniform4fv(ambientLoc, 1, ambient);

		GLint lightDirectionLoc = glGetUniformLocation(program, "LightDirection");
		vec3 lightDirection = normalize(vec3(3, 1, 2));
		glUniform3fv(lightDirectionLoc, 1, lightDirection);	
	}

	
	glEnable(GL_DEPTH_TEST);
	glClearColor(.30f, .45f, .75f, 0.f);
	glPointSize(5);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	textLabel = new font("label0", "SegoeUI.bmp", window_width, window_height);
	//wrld->addBillboard(textLabel);

	/*ray *ry = new ray("ray0", vec3(0, 0, 0), vec3(0, 0, 10), 3, vec4(1, 1, 0, 1));
	wrld->addObject(ry);
	ry->setVisible(false);*/

	ray *xAxis = new ray("X axis", vec3(0, 0, 0), vec3(100, 0, 0), 3, vec4(1, 0, 0, 1));
	ray *yAxis = new ray("Y axis", vec3(0, 0, 0), vec3(0, 100, 0), 3, vec4(0, 1, 0, 1));
	ray *zAxis = new ray("Z axis", vec3(0, 0, 0), vec3(0, 0, 100), 3, vec4(0, 0, 1, 1));

	wrld->addObject(xAxis);
	wrld->addObject(yAxis);
	wrld->addObject(zAxis);
}

int n = 0;
vec3 _vec;

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);

	if (wrld)
	{
		wrld->render();	

		if (textLabel) 
		{
			scene_object *obj = wrld->getActiveObject();
			if (obj)
			{
				char str[128] = { 0 };
				vec3 v = obj->getPosition();
				sprintf(str, "position: %.1f %.1f %.1f", v[0], v[1], v[2]);
				textLabel->setText(str, vec2(10, 10));
				textLabel->render();

				v = obj->getOrientation();
				sprintf(str, "direction: %.2f %.2f %.2f", v[0], v[1], v[2]);
				textLabel->setText(str, vec2(10, 40));
				textLabel->render();
			}

			camera *cam = wrld->getActiveCamera();
			ray *ry = (ray *)wrld->getObject("ray0");

			if (cam && ry)
			{
				char str[128] = { 0 };
				vec3 v = cam->getPos();
				sprintf(str, "cam pos: %.1f %.1f %.1f", v[0], v[1], v[2]);
				textLabel->setText(str, vec2(10, 70));
				textLabel->render();

				v = ry->getOrigin();
				sprintf(str, "ray origin: %.2f %.2f %.2f", v[0], v[1], v[2]);
				textLabel->setText(str, vec2(10, 100));
				textLabel->render();

				v = ry->getDirection();
				sprintf(str, "ray direction: %.2f %.2f %.2f", v[0], v[1], v[2]);
				textLabel->setText(str, vec2(10, 130));
				textLabel->render();
				
				v = _vec;
				sprintf(str, "wor: %.2f %.2f %.2f", v[0], v[1], v[2]);
				textLabel->setText(str, vec2(10, 160));
				textLabel->render();
			}
		}	
	
		glFlush();
	}
}

void cleanup()
{
	if (wrld)
	{
		delete wrld;
		wrld = NULL;
	}

	if (textLabel)
	{
		delete textLabel;
	}

	_CrtDumpMemoryLeaks();
}

void keyboard_callback(unsigned char key, int x, int y)
{
	float angle = 0;
	vec3 axis(0, 0, 0);
	mat4 v;
	quaternion offset;

	switch (key)
	{
	case 27:
		cleanup();
		glutLeaveMainLoop();
		return;
		break;

	default:
		wrld->processKeyboardInput(key);
	}

	glutPostRedisplay();
}

void timer_callback(int value)
{
	glutPostRedisplay();
	glutTimerFunc(30, timer_callback, 0);
}

void mouse_callback(int x, int y, int l0, int l1)
{
	camera *cam = wrld->getActiveCamera();

	if (!y)
		mouse_buttons[x] = 1;
	else
		mouse_buttons[x] = 0;

	if (x == 3)
	{
		cam->moveForward(-1);
		glutPostRedisplay();
	}

	if (x == 4)
	{
		cam->moveForward(1);
		glutPostRedisplay();
	}
}

void motion_callback(int mx, int my)
{
	if (mouse_buttons[0])
	{
		float xDelta = 0;
		float yDelta = 0;
		int xDiff = mouse_x_prev - mx;
		int yDiff = mouse_y_prev - my;

		if ((xDiff) > 0)
		{
			xDelta = -.02f;
		}
		else if ((xDiff) < 0)
		{
			xDelta = +.02f;
		}

		if ((yDiff) > 0)
		{
			yDelta = -.02f;
		}
		else if ((yDiff) < 0)
		{
			yDelta = +.02f;
		}

		camera *cam = wrld->getActiveCamera();
		cam->rotate(xDelta, yDelta);

		mouse_x_prev = mx;
		mouse_y_prev = my;
	}

	glutPostRedisplay();
}

void passive_motion_callback(int mx, int my)
{
	if (!mouse_buttons[0])
	{
		camera *cam = wrld->getActiveCamera();
		//ray *ry = (ray *)wrld->getObject("ray0");

		float x = (2.0f * mx) / window_width - 1.0f;
		float y = 1.0f - (2.0f * my) / window_height;
		float z = 1.0f;
		vec3 ray_nds = vec3(x, y, z);
		vec4 ray_clip = vec4(ray_nds[0], ray_nds[1], -1.0, 1.0);
		mat4 inv = inverseMat4(cam->getProjection());
		mat4 ray_eye = inv * ray_clip;
		vec4 eye(ray_eye[0][0], ray_eye[0][1], -1, 0);

		mat4 ray_wor = (inverseMat4(cam->getMatrix()) * eye);
		vec3 wor(ray_wor[0][0], ray_wor[0][1], ray_wor[0][2]);
		wor = normalize(wor);
		_vec = wor;

		vec3 origin = cam->getPos() + wor * 10.f; // cam->getPos() + normalize(cam->getLookAt() - cam->getPos());
		vec3 direction = origin + wor * 60;
		/*ry->setCoords(origin, direction);
		ry->setVisible(true);*/

		scene_object *mdl = wrld->getObject("model0");
		if (mdl)
		{
			if (mdl->intersects(cam->getPos(), wor))
			{
				mdl->setAlpha(.5f);
			}
			else
				mdl->setAlpha(1);
		}
	} 
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
	window = glutCreateWindow("Preview");

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
	glutPassiveMotionFunc(passive_motion_callback);
	glutTimerFunc(30, timer_callback, 0);
	glutMainLoop();

	return 0;
}

