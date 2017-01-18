/*******************************************************************
Multi-Part Model Construction and Manipulation
********************************************************************/

#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <utility>
#include <vector>
#include "VECTOR3D.h"
#include "CubeMesh.h"
#include "QuadMesh.h"

void initOpenGL(int w, int h);
void display(void);
void reshape(int w, int h);
void mouse(int button, int state, int x, int y);
void mouseMotionHandler(int xMouse, int yMouse);
void keyboard(unsigned char key, int x, int y);
void functionKeys(int key, int x, int y);
void mouseMove(int x, int y);
VECTOR3D GetOGLPos(int x, int y);
void checkMouse(void);
void updateMesh(void);

static int currentButton;
static unsigned char currentKey;


GLfloat light_position0[] = { -6.0,  12.0, 0.0,1.0 };
GLfloat light_position1[] = { 6.0,  12.0, 0.0,1.0 };
GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_ambient[] = { 0.2, 0.2, 0.2, 1.0 };


// Set up lighting/shading and material properties for submarine - upcoming lecture - just copy for now
GLfloat submarine_mat_ambient[] = { 0.4, 0.2, 0.0, 1.0 };
GLfloat submarine_mat_specular[] = { 0.1, 0.1, 0.0, 1.0 };
GLfloat submarine_mat_diffuse[] = { 0.9, 0.5, 0.0, 1.0 };
GLfloat submarine_mat_shininess[] = { 0.0 };

//camera positions
GLfloat camX = 0.0;
GLfloat camY = 6.0;
GLfloat camZ = 22.0;

//blob variables
GLfloat bHeight = 2.0;
GLfloat bWidth = 1.0;

//mouse variables
GLfloat midX = 0.0;
GLfloat midY = 0.0;
GLfloat spinX = 0.0;
GLfloat spinY = 0.0;

//screenToWorld coordinates
POINT mousePos;
GLfloat winX, winY, winZ;
HWND hWnd ;

//camera control
boolean cameraMode = false;
int counter = 0;
GLfloat cameraSpin = 0.2;
GLfloat meshSpin = 2.0;

//shade control
int shadeCounter = 1;


QuadMesh *groundMesh = NULL;

struct BoundingBox
{
	VECTOR3D min;
	VECTOR3D max;
} BBox;

// Default Mesh Size
int meshSize = 64;

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(250, 100);
	glutCreateWindow("CPS511 Ass2");

	initOpenGL(500, 500);

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMotionHandler);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(functionKeys);
	glutPassiveMotionFunc(mouseMove);
	


	glutMainLoop();
	return 0;
}



// Setup openGL */
void initOpenGL(int w, int h)
{
	// Set up viewport, projection, then change to modelview matrix mode - 
	// display function will then set up camera and modeling transforms
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, 1.0, 0.2, 80.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Set up and enable lighting
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);

	glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position1);
	glShadeModel(GL_FLAT);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	//glEnable(GL_LIGHT1);

	// Other OpenGL setup
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glClearColor(0.6, 0.6, 0.6, 0.0);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	// This one is important - renormalize normal vectors 
	glEnable(GL_NORMALIZE);

	//Nice perspective.
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	// Set up ground quad mesh
	VECTOR3D origin = VECTOR3D(-8.0f, 0.0f, 8.0f);
	VECTOR3D dir1v = VECTOR3D(1.0f, 0.0f, 0.0f);
	VECTOR3D dir2v = VECTOR3D(0.0f, 0.0f, -1.0f);
	groundMesh = new QuadMesh(meshSize, 16.0);
	groundMesh->InitMesh(meshSize, origin, 16.0, 16.0, dir1v, dir2v);

	VECTOR3D ambient = VECTOR3D(0.0f, 0.05f, 0.0f);
	VECTOR3D diffuse = VECTOR3D(0.4f, 0.8f, 0.4f);
	VECTOR3D specular = VECTOR3D(0.04f, 0.04f, 0.04f);
	float shininess = 0.2;
	groundMesh->SetMaterial(ambient, diffuse, specular, shininess);

	// Set up the bounding box of the scene
	// Currently unused. You could set up bounding boxes for your objects eventually.
	BBox.min.Set(-8.0f, 0.0, -8.0);
	BBox.max.Set(8.0f, 6.0, 8.0);

}



void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	// Set up the camera
	gluLookAt(camX, camY, camZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	//spin world and object
	glRotatef(spinX, 1.0, 0.0, 0.0);
	glRotatef(spinY, 0.0, 1.0, 0.0);
	
	// Draw ground
	groundMesh->DrawMesh(meshSize);
	glutSwapBuffers();
}


// Called at initialization and whenever user resizes the window */
void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, 1.0, 0.2, 40.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	

}

VECTOR3D pos = VECTOR3D(0, 0, 0);

// Mouse button callback - use only if you want to 
void mouse(int button, int state, int x, int y)
{
	currentButton = button;

	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN)
		{
			
			//glutIdleFunc(updateMesh);
			updateMesh();
		}

		else if (state == GLUT_UP)
		{
			//updateMesh();
		}
		break;
	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN)
		{
			;
		}
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

// Mouse motion callback - use only if you want to 
void mouseMotionHandler(int xMouse, int yMouse)
{
	

	if (currentButton == GLUT_LEFT_BUTTON)
	{
		checkMouse();
	}
	glutPostRedisplay();
}

void mouseMove(int x, int y) {
	
	//checkMouse();
	
}

//mouse handler for moving screen
void checkMouse(void) {
	//mouse coords
	GetCursorPos(&mousePos);
	hWnd = GetForegroundWindow();
	ScreenToClient(hWnd, &mousePos);
	winX = (float)mousePos.x;
	winY = (float)mousePos.y;
	
	//orient camera
	if (cameraMode == true) {
		//orient camera around y-axis when mouse goes in x-axis
		if (winX > midX) {
			spinY++;
			midX = winX;
		}
		else if (winX < midX) {
			spinY--;
			midX = winX;
		}
		//orient camera around x-axis when mouse goes in y-axis
		if (winY > midY) {
			spinX++;
			midY = winY;
		}
		else if (winY < midY) {
			spinX--;
			midY = winY;
		}
	}

}

//handler to update the mesh
void updateMesh(void) {
	//mouse coords
	GetCursorPos(&mousePos);
	hWnd = GetForegroundWindow();
	ScreenToClient(hWnd, &mousePos);
	winX = (float)mousePos.x;
	winY = (float)mousePos.y;
	VECTOR3D temp = GetOGLPos(winX, winY);
	printf("The x value is: %f\n", temp.x);
	printf("The y value is: %f\n", temp.y);
	printf("The z value is: %f\n", temp.z);
	if (cameraMode == false) {
		groundMesh->UpdateMesh(meshSize, temp, bHeight, bWidth);
	}
}


 /* Handles input from the keyboard, non-arrow keys */
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{

	//Bring 
	case 't':
		printf("CPS511 Assignment2\n");
		printf("Press F1 to toggle between camera mode and blob adjustment mode\n\n ");
		printf("Camera mode: Off\n");
		printf("--------------\n");
		printf("drag mouse left,right,up and down to change orientation.\n");
		printf("\nAlternate Camera mode: Off\n");
		printf("--------------\n");
		printf("Up arrow key/down arrow key -> change y direction \n");
		printf("Left arrow key/right arrow key -> change x direction\n");
		printf("PgUp / PgDn keys -> Zoom in an out\n");
		printf("F2 key used to switch between vertex normal and quad normal\n");
		printf("\nBlob Adjustment mode:On by default\n");
		printf("--------------\n");
		printf("Left click to create a blob\n");
		printf("Clicking near or around the blob will create another formed blob\n");
		printf("Up/Down arrow key -> change height of blob\n");
		printf("Left/Right arrow key -> change width of blob\n");

		break;
	}

	
	glutPostRedisplay();
}

void functionKeys(int key, int x, int y)
{
	VECTOR3D min, max;

	//Toggle camera mode
	if (key == GLUT_KEY_F1)
	{
		if (counter == 0) {
			cameraMode = true;
			counter++;
			printf("\nCamera mode is activated!!\n");
		}
		else {
			cameraMode = false;
			counter = 0;
			printf("\nCamera mode is off!! You may now create blobs\n");
		}
	}

	//Change shading
	if (key == GLUT_KEY_F2)
	{
		if (shadeCounter == 0) {
			glShadeModel(GL_SMOOTH);
			shadeCounter++;
			
		}
		else {
			glShadeModel(GL_FLAT);
			shadeCounter = 0;
			
		}
		
	}

	else if (key == GLUT_KEY_UP) {
		
		//case move camera
		if (cameraMode == true) {
			camY += cameraSpin;
				if (camY >= 27.0) {
				camY = 27.0;
			}
		}

		//control height of blob
		else {
			bHeight += cameraSpin;
			if (bHeight >= 11.5)
				bHeight = 11.5;
			else if (bHeight < 1.0 && bHeight > -1.0)
				bHeight = 1.0;		
			
		}
			
	}

	else if (key == GLUT_KEY_DOWN)
	{
		
		//case move camera
		if (cameraMode == true) {
			camY -= cameraSpin;
			//spinY += meshSpin;
			if (camY <= 3.0) {
				camY = 3.0;
			}
		}

		//control height of blob
		else {
			bHeight -= cameraSpin;
			if (bHeight < 1.0 && bHeight >-1.0) {
				bHeight = -1.0;
			}
			
		}
	}
	else if (key == GLUT_KEY_LEFT) {
		
		//case move camera
		if (cameraMode == true) {
			camX -= cameraSpin;
			spinY += meshSpin;
			if (camX <= -17.4) {
				camX = -17.4;
			}
		}

		//control width of blob
		else {
			bWidth += cameraSpin;
			if (bWidth >= 6.0)
				bWidth = 6.0;
		}

	}
	else if (key == GLUT_KEY_RIGHT) {
		
		//case move camera
		if (cameraMode == true) {
			camX += cameraSpin;
			spinY -= meshSpin;
			if (camX >= 17.4) {
				camX = 17.4;
			}
		}

		//control width of blob
		else {
			bWidth -= cameraSpin;
			if (bWidth <= 1.0)
				bWidth = 1.0;
		}
			
		
	}

	//Zoom out
	else if (key == GLUT_KEY_PAGE_DOWN) {
		camZ += cameraSpin;
		if (camZ >= 25.0) {
			camZ = 25.0;
		}
	}

	//Zoom in
	else if (key == GLUT_KEY_PAGE_UP) {
		camZ -= cameraSpin;
		if (camZ <= 12.5) {
			camZ = 12.5;
		}
	}

	glutPostRedisplay();
}


//Window to openGL coordinates
VECTOR3D GetOGLPos(int x, int y)
{
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	GLfloat winX, winY, winZ;
	GLdouble posX, posY, posZ;

	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);

	winX = (float)x;
	winY = (float)viewport[3] - (float)y;
	glReadPixels(x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);

	gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

	return VECTOR3D(posX, posY, posZ);
}

