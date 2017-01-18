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

void initOpenGL(int w, int h);
void display(void);
void reshape(int w, int h);
void mouse(int button, int state, int x, int y);
void mouseMotionHandler(int xMouse, int yMouse);
void keyboard(unsigned char key, int x, int y);
void functionKeys(int key, int x, int y);
void SpinDisplay(void);
VECTOR3D ScreenToWorld(int x, int y);

static int currentButton;
static unsigned char currentKey;

static GLfloat spin = 0.0;
static GLfloat turn = 0.0;
static GLfloat moveV = 0.0;
static GLfloat speed = 0.0;
static GLfloat Xtri, Ztri = 0.0;
static int maxCounter = 0;

GLfloat light_position0[] = {-6.0,  12.0, 0.0,1.0};
GLfloat light_position1[] = { 6.0,  12.0, 0.0,1.0};
GLfloat light_diffuse[]   = {1.0, 1.0, 1.0, 1.0};
GLfloat light_specular[]  = {1.0, 1.0, 1.0, 1.0};
GLfloat light_ambient[]   = {0.2, 0.2, 0.2, 1.0};


// Set up lighting/shading and material properties for submarine - upcoming lecture - just copy for now
GLfloat submarine_mat_ambient[] = { 0.4, 0.2, 0.0, 1.0 };
GLfloat submarine_mat_specular[] = { 0.1, 0.1, 0.0, 1.0 };
GLfloat submarine_mat_diffuse[] = { 0.9, 0.5, 0.0, 1.0 };
GLfloat submarine_mat_shininess[] = { 0.0 };

//Trying out lights of propellor
GLfloat prop_mat_ambient[] = {1.0,1.0,1.0,1.0};
GLfloat prop_mat_specular[] = { 0.1, 0.1, 0.0, 1.0 };
GLfloat prop_mat_diffuse[] = { 0.9, 0.5, 0.0, 1.0 };
GLfloat prop_mat_shininess[] = { 0.0};

QuadMesh *groundMesh = NULL;


struct BoundingBox
{
  VECTOR3D min;
  VECTOR3D max;
} BBox;

// Default Mesh Size
int meshSize = 16;

int main(int argc, char **argv)
{
  glutInit(&argc,argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(1000, 500);
  glutInitWindowPosition(100, 100);
  glutCreateWindow("CPS511 Assignment 1");
  initOpenGL(1000,500);
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse);
  glutMotionFunc(mouseMotionHandler);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(functionKeys);  
  glutMainLoop();

  

  
  return 0;

}



// Setup openGL */
void initOpenGL(int w, int h)
{
	
  // Set up viewport, projection, then change to modelview matrix mode - 
  // display function will then set up camera and modeling transforms
  glViewport(0, 0, (GLsizei) w, (GLsizei) h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0,1.0,0.2,500.0);
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
  glShadeModel(GL_SMOOTH);
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
  glHint(GL_PERSPECTIVE_CORRECTION_HINT , GL_NICEST);
  
  // Set up ground quad mesh
  VECTOR3D origin  = VECTOR3D(-8.0f,0.0f,8.0f);
  VECTOR3D dir1v   = VECTOR3D(1.0f, 0.0f, 0.0f);
  VECTOR3D dir2v   = VECTOR3D(0.0f, 0.0f,-1.0f);
  groundMesh = new QuadMesh(meshSize, 16.0);
  groundMesh->InitMesh(meshSize, origin, 16.0, 16.0, dir1v, dir2v);
  
  VECTOR3D ambient = VECTOR3D(0.0f, 0.05f, 0.0f);
  VECTOR3D diffuse = VECTOR3D(0.4f, 0.8f, 0.4f);
  VECTOR3D specular = VECTOR3D(0.04f, 0.04f, 0.04f);
  float shininess = 0.2;
  groundMesh->SetMaterial(ambient,diffuse,specular,shininess);

  // Set up the bounding box of the scene
  // Currently unused. You could set up bounding boxes for your objects eventually.
  BBox.min.Set(-8.0f, 0.0, -8.0);
  BBox.max.Set( 8.0f, 6.0,  8.0);
}



void display(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glLoadIdentity();
  
  // Set up the camera
  gluLookAt(0.0,6.0,22.0,0.0,0.0,0.0,0.0,1.0,0.0);

  //Draw ground
  glPushMatrix();
  glTranslatef(0.0, -6.0, 0.0);
  groundMesh->DrawMesh(meshSize);
  glPopMatrix();

  

  // Set submarine material properties
  glMaterialfv(GL_FRONT, GL_AMBIENT, submarine_mat_ambient);
  glMaterialfv(GL_FRONT, GL_SPECULAR, submarine_mat_specular);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, submarine_mat_diffuse);
  glMaterialfv(GL_FRONT, GL_SHININESS, submarine_mat_shininess);

  // Apply transformations to move submarine
  glTranslatef(0.0, moveV, 0.0);
  glTranslatef(Xtri, 0.0, Ztri);
  glRotatef(turn, 0.0, -1.0, 0.0);
  glScalef(0.2, 0.7, 0.5);

  // Apply transformations to construct submarine
  glPushMatrix();
  glTranslatef(0.0, 4.0, 0.0);
  glScalef(6.0, 1.0, 1.0);
  glutSolidSphere(1.0,50,50);
  glPopMatrix();

  //Applying top as cube
  glPushMatrix();
  glTranslatef(0.0, 4.5, 0.0);
  glRotatef(90.0, 0.0, 0.0, 1.0);
  glScalef(2.0, 1.0, 1.0);
  glutSolidCube(1.0);
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
		  glPopMatrix();
	  }
  glScalef(0.2,0.2,0.2);
  glutSolidTorus(1.0, 2.0, 20.0, 20.0);
  glPopMatrix();

  
  glutSwapBuffers();
}


// Called at initialization and whenever user resizes the window */
void reshape(int w, int h)
{
  glViewport(0, 0, (GLsizei) w, (GLsizei) h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0,1.0,0.2,40.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

}


// Mouse button callback - use only if you want to 
void mouse(int button, int state, int x, int y)
{
  currentButton = button;

  switch(button)
  {
  case GLUT_LEFT_BUTTON:
    if (state == GLUT_DOWN)
	{
		maxCounter++;//keep track of times clicked
		glutIdleFunc(SpinDisplay);
		
	}
	break;
  case GLUT_RIGHT_BUTTON:
    if (state == GLUT_DOWN)
	{
		maxCounter--;//No spinning of counter is at 0
		if (maxCounter <= 0) {
			maxCounter = 0;
			glutIdleFunc(NULL);
		}
	}
	break;
  default:
	break;
  }
  glutPostRedisplay(); 

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

// Mouse motion callback - use only if you want to 
void mouseMotionHandler(int xMouse, int yMouse)
{
  if (currentButton == GLUT_LEFT_BUTTON)
  {
	  ;
  }
  glutPostRedisplay();
}


VECTOR3D ScreenToWorld(int x, int y)
{
	// you will need this if you use the mouse
	return VECTOR3D(0);
}// ScreenToWorld()

/* Handles input from the keyboard, non-arrow keys */
void keyboard(unsigned char key, int x, int y)
{
  switch (key) 
  {
  case 't':
	  printf("%s","CPS511 Assignment1 Instructions\n");
	  printf("%s", "1) To move the submarine up and down, press the up/down arrow keys respectively\n");
	  printf("%s", "\n 2) To turn the submarine, use the left and right arrow keys respectively\n");
	  printf("%s", "\n 3) Use left click to begin movement of the submarine, right click to slow down\n");
	  printf("%s", "\n Submarine has three speeds. User can left click up to three times. User may also slow down via right click until completely stopped\n");
	  break;
  }
  glutPostRedisplay();
}

void functionKeys(int key, int x, int y)
{
  VECTOR3D min, max;

  if (key == GLUT_KEY_F1)
  {
    
    
  }


  else if (key == GLUT_KEY_LEFT) {
	  //turn sub
	  turn -= 1.2;
	  if (maxCounter == 3) {
		  turn -= 3.0;
	  }
	  glutPostRedisplay();
  }

  else if (key == GLUT_KEY_RIGHT) {
	 //turn sub
	  turn += 1.2;
	  if (maxCounter == 3) {
		  turn += 3.0;
	  }
	  glutPostRedisplay();
  }

  else if (key == GLUT_KEY_UP) {
	  //Move vertical pos of sub 
	  moveV+= 0.2;
	  if (moveV > 6.0)
		  moveV = 6.0;
	  glutPostRedisplay();
  }

  else if (key == GLUT_KEY_DOWN) {
	  //Move vertical pos of sub
	  moveV -= 0.2;
	 if (moveV < -7.5)
		 moveV = -7.5;
	  glutPostRedisplay();
  }



}





