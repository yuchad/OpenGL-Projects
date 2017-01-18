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

#define PI 3.141592654
#define bound 8
#define boundTor 16

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
void createProjectile(void);
void makeTextures(void);
void makeTextureMap(void);
void displayTorpedo(int);
void createAI(void);
void spinforAI(int);

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

//Submarine variables
static GLfloat spin = 0.0;
static GLfloat turn = 0.0;
static GLfloat moveV = 0.0;
static GLfloat speed = 0.0;
static GLfloat Xtri, Ztri = 0.0;
static int maxCounter = 0;
static boolean isFire = false;
static GLfloat fireSpeed = 1.0;

//Torpedo Variables
static GLfloat torRadius = 0;
static GLfloat xTor = 0.0;
static GLfloat yTor = 0.0;
static GLfloat zTor = 0.0;
static GLfloat torSpeed = 0.1;
static GLfloat turnTor = 0.0;

//AI variables
static GLfloat turnAI = 0;
static GLfloat speedAI = 0.03;
static GLfloat xAI = 0;
static GLfloat zAI = 0;
static int countAI = 0;


//Trying out lights of propellor
GLfloat prop_mat_ambient[] = { 1.0,1.0,1.0,1.0 };
GLfloat prop_mat_specular[] = { 0.1, 0.1, 0.0, 1.0 };
GLfloat prop_mat_diffuse[] = { 0.9, 0.5, 0.0, 1.0 };
GLfloat prop_mat_shininess[] = { 0.0 };


//camera positions
GLfloat camX = 0.0;
GLfloat camY = 6.0;
GLfloat camZ = 22.0;
static int camCounter = 0;

GLfloat subCamEyeX = 0.10;
GLfloat subCamEyeY = 4.0;
GLfloat subCamEyeZ = 0.0;
GLfloat subCamCentreX = 2.0;
GLfloat subCamCentreY = 3.0;
GLfloat subCamCentreZ = 0.0;


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

/*From example*/
GLfloat textureMap[64][64][3];
GLuint tex[2];
void assignColor(GLfloat col[3], GLfloat r, GLfloat g, GLfloat b) {
	col[0] = r;
	col[1] = g;
	col[2] = b;
}

/*From example*/
int isEven(int x) {
	if (x % 2 == 0)
		return 1;
	else
		return 0;
}





QuadMesh *groundMesh = NULL;

Metaballs *metaBlobList;

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
	glutCreateWindow("CPS511 Ass3");

	initOpenGL(500, 500);

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMotionHandler);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(functionKeys);
	glutPassiveMotionFunc(mouseMove);
	glutTimerFunc(1, displayTorpedo, 0);
	glutTimerFunc(1, spinforAI, 0);


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

	glEnable(GL_TEXTURE_2D);
	makeTextureMap();
	makeTextures();
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

	metaBlobList = new Metaballs[20];

	metaBlobList[0].pos = VECTOR3D(-5.5, -0.02, -4.5);
	metaBlobList[0].height = 2;
	metaBlobList[0].width = 1;

	metaBlobList[1].pos = VECTOR3D(-3.2, -0.02, -4.5);
	metaBlobList[1].height = 3;
	metaBlobList[1].width = 2;

	metaBlobList[2].pos = VECTOR3D(6.15, -0.01, 5.8);
	metaBlobList[2].height = 2;
	metaBlobList[2].width = 1;

	metaBlobList[5].pos = VECTOR3D(6.15, -0.01, 5.8);
	metaBlobList[5].height = 2;
	metaBlobList[5].width = 1;

	metaBlobList[3].pos = VECTOR3D(0.0, -0.02, 0.3);
	metaBlobList[3].height = -3;
	metaBlobList[3].width = 0.3;

	metaBlobList[4].pos = VECTOR3D(-5.3, -0.02, 5.5);
	metaBlobList[4].height = 2;
	metaBlobList[4].width = .3;

	metaBlobList[6].pos = VECTOR3D(-2.56, -0.02, 5.9);
	metaBlobList[6].height = 2;
	metaBlobList[6].width = 1;

	metaBlobList[7].pos = VECTOR3D(0.6, -0.02, 4.3);
	metaBlobList[7].height = 3;
	metaBlobList[7].width = 3;

	metaBlobList[8].pos = VECTOR3D(3.55, -0.02, 5.1);
	metaBlobList[8].height = 2;
	metaBlobList[8].width = 3;

	metaBlobList[9].pos = VECTOR3D(5.17, -0.02, -4.1);
	metaBlobList[9].height = 4;
	metaBlobList[9].width = 0.4;

	metaBlobList[10].pos = VECTOR3D(3.85, -0.02, -2.8);
	metaBlobList[10].height = 2;
	metaBlobList[10].width = 1;

	metaBlobList[11].pos = VECTOR3D(6.0, -0.02, -1.9);
	metaBlobList[11].height = 2;
	metaBlobList[11].width = 1;

	metaBlobList[12].pos = VECTOR3D(-6.08, -0.02, 0.87);
	metaBlobList[12].height = 2;
	metaBlobList[12].width = 1;

	metaBlobList[13].pos = VECTOR3D(-5.08, -0.02, 2.87);
	metaBlobList[13].height = 2;
	metaBlobList[13].width = 0.5;

	metaBlobList[14].pos = VECTOR3D(6.0, -0.02, 0.87);
	metaBlobList[14].height = 2;
	metaBlobList[14].width = .3;

	

	groundMesh->UpdateMesh(metaBlobList, 14, meshSize);
	
	
}



void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glLoadIdentity();
	
	if (camCounter == 1) {
		gluLookAt(subCamEyeX, subCamEyeY,subCamEyeZ, subCamCentreX, subCamCentreY, subCamCentreZ, 0.0, 1.0, 0.0);
	}
	else
	// Set up the camera
	gluLookAt(camX, camY, camZ, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	//spin world and object
	glRotatef(spinX, 1.0, 0.0, 0.0);
	glRotatef(spinY, 0.0, 1.0, 0.0);


	
	// Draw ground
	groundMesh->DrawMesh(meshSize);
	
	// Set submarine material properties
	glMaterialfv(GL_FRONT, GL_AMBIENT, submarine_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, submarine_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, submarine_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, submarine_mat_shininess);

	//control Bullet
	glPushMatrix();
	glTranslatef(xTor, yTor, zTor);
	glRotatef(180, 0.0, 1.0, 0.0);
	createProjectile();
	glPopMatrix();

	
	glPushMatrix();
	//glTranslatef(xAI, 0.0, zAI);
	glRotatef(turnAI, 0.0, 1.0, 0.0);
	createAI();
	glPopMatrix();
	

	// Apply transformations to move submarine
	glTranslatef(0.0, moveV, 0.0);
	glTranslatef(Xtri, 0.0, Ztri);
	glRotatef(turn, 0.0, -1.0, 0.0);
	glScalef(0.2, 0.7, 0.5);



	// Apply transformations to construct submarine
	glPushMatrix();
	glTranslatef(0.0, 4.0, 0.0);
	glScalef(6.0, 1.0, 1.0);
	GLUquadricObj *cylinder = gluNewQuadric();
	glBindTexture(GL_TEXTURE_2D, tex[0]);
	gluQuadricTexture(cylinder, GL_TRUE);
	gluQuadricDrawStyle(cylinder, GLU_FILL);
	glPolygonMode(GL_FRONT, GL_FILL);
	gluQuadricNormals(cylinder, GLU_SMOOTH);
	gluSphere(cylinder, 1.0,50,50);
	glPopMatrix();

	
	
	//Creating spheres as windows on left side
	for (int i = 0; i < 2; i++) {
		glPushMatrix();
		glTranslatef(i * 2, 4.0, -0.8);
		glScalef(0.5, 0.5, 0.5);
		glutSolidSphere(1.0, 50.0, 50.0);
		glPopMatrix();
	}

	//Creating spheres as windows on right side
	for (int i = 0; i < 2; i++) {
		glPushMatrix();
		glTranslatef(i * 2, 4.0, 0.8);
		glScalef(0.5, 0.5, 0.5);
		glutSolidSphere(1.0, 50.0, 50.0);
		glPopMatrix();
	}


	//Changing colours for propellors
	glMaterialfv(GL_FRONT, GL_AMBIENT, prop_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, prop_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, prop_mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, prop_mat_shininess);

	//Propellor
	glPushMatrix();
	glTranslatef(-5.8, 4.0, 0.0);
	glRotatef(90.0, 0.0, 1.0, 0.0);
	//blades for propellor, relative to propellor position
	for (int j = 0; j<6; j++)
	{
		glPushMatrix();
		glRotatef(spin, 0.0, 0.0, 1.0);
		glRotatef(60.0*j, 0.0, 0.0, 1.0);
		glTranslatef(1.0, 0.0, 0.0);
		glScalef(1.0, 0.1, 0.3);
		glutSolidCube(1.0);
		//(cylinder, 1.0);
		glPopMatrix();
	}
	glScalef(0.2, 0.2, 0.2);
	glutSolidTorus(1.0, 2.0, 20.0, 20.0);
	glPopMatrix();

	//Creating periscope
	glPushMatrix();
	glTranslatef(1.0, 6.0, 0.0);
	glRotatef(-90, 0.0, 1.0, 0.0);
	glScalef(0.5, 0.5, 0.5);
	glEnable(GL_TEXTURE_GEN_S); 
	glEnable(GL_TEXTURE_GEN_T);
	glBindTexture(GL_TEXTURE_2D, tex[1]);
	glutSolidCone(1, 2, 16, 16);
	glDisable(GL_TEXTURE_GEN_S); 
	glDisable(GL_TEXTURE_GEN_T);
	glPopMatrix();

	//creating stem
	glPushMatrix();
	glTranslatef(0.0, 5.5, 0.0);
	glScalef(0.5, 1.5, 0.5);
	glutSolidCube(1.0);
	glPopMatrix();

	//Applying top as cube
	glPushMatrix();
	glTranslatef(0.0, 4.5, 0.0);
	glRotatef(90.0, 0.0, 0.0, 1.0);
	glScalef(2.0, 1.0, 1.0);
	glutSolidCube(1.0);
	glPopMatrix();

	 glutSwapBuffers();

}

void createProjectile(void) {
	glPushMatrix();
	glTranslatef(Xtri, moveV + 5.0, Ztri);
	GLUquadricObj *cylinder = gluNewQuadric();
	glBindTexture(GL_TEXTURE_2D, tex[1]);
	gluQuadricTexture(cylinder, GL_TRUE);
	gluQuadricDrawStyle(cylinder, GLU_FILL);
	glPolygonMode(GL_FRONT, GL_FILL);
	gluQuadricNormals(cylinder, GLU_SMOOTH);
	gluSphere(cylinder, torRadius , 20.0, 20.0);
	glPopMatrix();
}

void createAI(void) {
	
	glPushMatrix();
	glTranslatef(5.0, 5.0, 0.0);
	glRotatef(45.0, 0.0, 1.0, 0.0);
	glScalef(0.2, 0.7, 0.5);
	

		// Apply transformations to construct submarine
		glPushMatrix();
		glScalef(6.0, 1.0, 1.0);
		GLUquadricObj *cylinder = gluNewQuadric();
		glBindTexture(GL_TEXTURE_2D, tex[0]);
		gluQuadricTexture(cylinder, GL_TRUE);
		gluQuadricDrawStyle(cylinder, GLU_FILL);
		glPolygonMode(GL_FRONT, GL_FILL);
		gluQuadricNormals(cylinder, GLU_SMOOTH);
		gluSphere(cylinder, 1.0, 50, 50);
		glPopMatrix();


		//Changing colours for propellors
		glMaterialfv(GL_FRONT, GL_AMBIENT, prop_mat_ambient);
		glMaterialfv(GL_FRONT, GL_SPECULAR, prop_mat_specular);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, prop_mat_diffuse);
		glMaterialfv(GL_FRONT, GL_SHININESS, prop_mat_shininess);

		//Propellor
		glPushMatrix();
		glTranslatef(-5.8, 0.0, 0.0);
		glRotatef(90.0, 0.0, 1.0, 0.0);
		//blades for propellor, relative to propellor position
		for (int j = 0; j<6; j++)
		{
			glPushMatrix();
			glRotatef(60.0*j, 0.0, 0.0, 1.0);
			glTranslatef(1.0, 0.0, 0.0);
			glScalef(1.0, 0.1, 0.3);
			glutSolidCube(1.0);
			//(cylinder, 1.0);
			glPopMatrix();
		}
		glScalef(0.2, 0.2, 0.2);
		glutSolidTorus(1.0, 2.0, 20.0, 20.0);
		glPopMatrix();
	glPopMatrix();
}
	

	
void displayTorpedo(int) {
	if (isFire) {
		if (xTor < -boundTor) {
			torRadius = 0.0;
			isFire = false;
		}
		else if (xTor > boundTor) {
			torRadius = 0.0;
			isFire = false;
		}
		else if (zTor < -boundTor) {
			torRadius = 0.0;
			isFire = false;
		}
		else if (zTor > boundTor) {
			torRadius = 0.0;
			isFire = false;
		}
		else {
			torRadius = 0.25;
			zTor += torSpeed * cos((turn * PI) / 180);
			xTor -= torSpeed * sin((turn * PI) / 180);
		}
	}
	else torRadius = 0.0;

	glutPostRedisplay();
	glutTimerFunc(1, displayTorpedo, 0);
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
			//updateMesh();
			//printf("%f",groundMesh->)
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
	
	if (cameraMode == false) {
		//groundMesh->UpdateMesh(meshSize, temp, bHeight, bWidth);
	}
}

//Spin and speed according to counter
void SpinDisplay(void) {
	if (maxCounter == 1) {
		spin += 0.5;
		speed = 0.003;
	}

	if (maxCounter == 2) {
		spin += 1.0;
		speed = 0.009;
	}

	if (maxCounter >= 3) {
		spin += 2.0;
		speed = 0.015;
		maxCounter = 3;
	}

	//Find direction sub is running
	Xtri += speed * cos((turn * PI) / 180);
	Ztri += speed * sin((turn * PI) / 180);

	//Update the camera in periscope
	subCamEyeX = Xtri + .10;
	subCamCentreX = Xtri + 2.0;
	subCamEyeZ = Ztri;

	//Set up bounds for sub
	if (Xtri > bound)
		Xtri = bound;

	else if (Xtri < -bound)
		Xtri = -bound;

	if (Ztri > bound)
		Ztri = bound;

	else if (Ztri < -bound)
		Ztri = -bound;

	//Reset spin
	if (spin > 360)
		spin -= 360;

	glutPostRedisplay();
}

//Spin of AI
void spinforAI(int) {
	
	turnAI += 0.3;
	if (turnAI > 360)
		turnAI -= 360;
	glutPostRedisplay();
	glutTimerFunc(1, spinforAI, 0);
}


 /* Handles input from the keyboard, non-arrow keys */
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{

	//Bring 
	case 't':
		printf("CPS511 Assignment3\n");
		printf("Press F1 to toggle between camera mode and submarine mode\n\n ");
		printf("Press F2 to toggle between Periscope Mode and submarine mode\n\n");
		printf("Camera mode: Off(Default)\n");
		printf("--------------\n");
		printf("%s", "1) To move the submarine up and down, press the  'w' and 's' respectively\n");
		printf("%s", "\n 2) To turn the submarine, use the 'a' and 'd' respectively\n");
		printf("%s", "\n 3) Use 'j' to begin movement of the submarine, 'k' to slow down\n");
		printf("%s", "\n 4) Submarine has three speeds. User can press 'j' up to three times. User may also slow down via 'k' until completely stopped\n");
		printf("%s", "\n 5) Press spacebar to fire a torpedo shot");

		break;

	

	case 'w':
		//Move vertical pos of sub 
		moveV += 0.2;
		if (moveV > 6.0)
			moveV = 6.0;
		break;

	case 'a':
		//turn sub
		turn -= 1.2;
		if (maxCounter == 3) {
			turn -= 3.0;
		}
		break;

	case 's':
		//Move vertical pos of sub
		moveV -= 0.2;
		if (moveV < -7.5)
			moveV = -7.5;
		break;

	case 'd':
		//turn sub
		turn += 1.2;
		if (maxCounter == 3) {
			turn += 3.0;
		}
		break;

	case 'j':
		maxCounter++;//keep track of times clicked
		glutIdleFunc(SpinDisplay);
		break;

	case 'k':
		maxCounter--;//No spinning of counter is at 0
		if (maxCounter <= 0) {
			maxCounter = 0;
			glutIdleFunc(NULL);
		}
		break;

	case ' ':
		 xTor = Xtri;
		 yTor = moveV;
		 zTor = Ztri;
		 turnTor = turn;
		isFire = true;
		xTor = torSpeed * cos((turn * PI) / 180);
		zTor = torSpeed * sin((turn * PI) / 180);
		
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

	//Change to periscope
	if (key == GLUT_KEY_F2)
	{
		if (camCounter == 0) {
			camCounter++;

		}
		else {
			camCounter = 0;

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

/*From Example*/
void makeTextures(void)
{
	GLfloat planes[] = { 0.0, 0.0, 0.3, 0.0 };
	GLfloat planet[] = { 0.0, 0.3, 0.0, 0.0 };

	glGenTextures(2, tex);

	glBindTexture(GL_TEXTURE_2D, tex[0]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 64, 64, 0, GL_RGB, GL_FLOAT, textureMap);

	glBindTexture(GL_TEXTURE_2D, tex[1]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGenfv(GL_S, GL_OBJECT_PLANE, planes);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGenfv(GL_T, GL_OBJECT_PLANE, planet);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 64, 64, 0, GL_RGB, GL_FLOAT, textureMap);
}

/*From Example*/
void makeTextureMap(void)
{
	for (int i = 0; i < 64; i++)
		for (int j = 0; j< 64; j++) {
			if (isEven(i / 8 + j / 8))
				assignColor(textureMap[i][j], 1.0, 1.0, 1.0);
			else
				assignColor(textureMap[i][j], 1.0, 0.0, 0.0);
		}
}



