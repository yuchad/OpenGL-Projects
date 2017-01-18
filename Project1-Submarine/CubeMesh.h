#pragma once
// Vertex positions of a standard size cube (width 2), centered at the origin
// of its own Model Coordinate System
GLfloat vertices[][3] = { { -1.0, -1.0,-1.0 },
{ 1.0, -1.0,-1.0 },
{ 1.0,  1.0,-1.0 },
{ -1.0,  1.0,-1.0 },
{ -1.0, -1.0, 1.0 },
{ 1.0, -1.0, 1.0 },
{ 1.0,  1.0, 1.0 },
{ -1.0,  1.0, 1.0 } };

GLubyte quads[] = { 0, 3, 2, 1,	// back face
2, 3, 7, 6,	// top face
0, 4, 7, 3,  // left face
1, 2, 6, 5,  // right face
4, 5, 6, 7,  // front face
0, 1, 5, 4 };	// bottom face

GLfloat quadNormals[][3] = { { 0.0, 0.0, -1.0 },	// Back Face 
{ 0.0, 1.0,  0.0 },	// Top Face
{ -1.0, 0.0,  0.0 },	// Left Face
{ 1.0, 0.0,  0.0 },	// Right Face
{ 0.0, 0.0,  1.0 },	// Front Face
{ 0.0,-1.0,  0.0 } };	// Bottom Face

typedef struct CubeMesh
{
	VECTOR3D center;
	VECTOR3D dim;
	float tx, ty, tz;			// Translatation Deltas
	float sfx, sfy, sfz;		// Scale Factors
	float angle;				// Angle around y-axis of cube coordinate system

	bool selected;

	// Material properties for drawing
	float mat_ambient[4];
	float mat_specular[4];
	float mat_diffuse[4];
	float mat_shininess[1];

	// Material properties if selected
	float highlightMat_ambient[4];
	float highlightMat_specular[4];
	float highlightMat_diffuse[4];
	float highlightMat_shininess[1];

} CubeMesh;

CubeMesh *createCube()
{
	CubeMesh *newCube = (CubeMesh*)calloc(1, sizeof(CubeMesh));

	newCube->angle = 0.0;
	newCube->sfx = newCube->sfy = newCube->sfz = 1.0;
	newCube->tx = 0.0;
	newCube->ty = 0.0;
	newCube->tz = 0.0;
	newCube->center.Set(0, 0, 0);
	newCube->dim.Set(2.0f, 2.0f, 2.0f);

	newCube->mat_ambient[0] = 0.0;
	newCube->mat_ambient[1] = 0.05;
	newCube->mat_ambient[2] = 0.0;
	newCube->mat_ambient[3] = 1.0;
	newCube->mat_specular[0] = 0.0;
	newCube->mat_specular[1] = 0.0;
	newCube->mat_specular[2] = 0.004;
	newCube->mat_specular[3] = 1.0;
	newCube->mat_diffuse[0] = 0.5;
	newCube->mat_diffuse[1] = 0.5;
	newCube->mat_diffuse[2] = 0.5;
	newCube->mat_diffuse[3] = 1.0;
	newCube->mat_shininess[0] = 0;

	newCube->highlightMat_ambient[0] = 0.0;
	newCube->highlightMat_ambient[1] = 0.00;
	newCube->highlightMat_ambient[2] = 0.0;
	newCube->highlightMat_ambient[3] = 1.0;
	newCube->highlightMat_specular[0] = 0.0;
	newCube->highlightMat_specular[1] = 0.0;
	newCube->highlightMat_specular[2] = 0.0;
	newCube->highlightMat_specular[3] = 1.0;
	newCube->highlightMat_diffuse[0] = 1.0;
	newCube->highlightMat_diffuse[1] = 0.0;
	newCube->highlightMat_diffuse[2] = 0.0;
	newCube->highlightMat_diffuse[3] = 1.0;
	newCube->highlightMat_shininess[0] = 0.0;

	return newCube;
}

// Given a cube mesh, compute it's current bounding box and return in vectors min and max
// i.e. compute min.x,min.y,mi.z,max.x,max.y,max.z
// Use this function for collision detection of cube and walls/floor
void getBBox(CubeMesh *cube, VECTOR3D *min, VECTOR3D *max)
{

}

void drawCube(CubeMesh *cube)
{
	if (cube->selected)
	{
		// Setup the material and lights used for the cube
		glMaterialfv(GL_FRONT, GL_AMBIENT, cube->highlightMat_ambient);
		glMaterialfv(GL_FRONT, GL_SPECULAR, cube->highlightMat_specular);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, cube->highlightMat_diffuse);
		glMaterialfv(GL_FRONT, GL_SHININESS, cube->highlightMat_shininess);
	}
	else
	{
		// Setup the material and lights used for the cube
		glMaterialfv(GL_FRONT, GL_AMBIENT, cube->mat_ambient);
		glMaterialfv(GL_FRONT, GL_SPECULAR, cube->mat_specular);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, cube->mat_diffuse);
		glMaterialfv(GL_FRONT, GL_SHININESS, cube->mat_shininess);
	}


	// Transform and Draw cube   
	glPushMatrix();
	// put your transform code here
	// ......

	glBegin(GL_QUADS);
	// Back Face
	glNormal3f(quadNormals[0][0], quadNormals[0][1], quadNormals[0][2]);
	glVertex3f(vertices[quads[0]][0], vertices[quads[0]][1], vertices[quads[0]][2]);
	glVertex3f(vertices[quads[1]][0], vertices[quads[1]][1], vertices[quads[1]][2]);
	glVertex3f(vertices[quads[2]][0], vertices[quads[2]][1], vertices[quads[2]][2]);
	glVertex3f(vertices[quads[3]][0], vertices[quads[3]][1], vertices[quads[3]][2]);
	// Top Face
	glNormal3f(quadNormals[1][0], quadNormals[1][1], quadNormals[1][2]);
	glVertex3f(vertices[quads[4]][0], vertices[quads[4]][1], vertices[quads[4]][2]);
	glVertex3f(vertices[quads[5]][0], vertices[quads[5]][1], vertices[quads[5]][2]);
	glVertex3f(vertices[quads[6]][0], vertices[quads[6]][1], vertices[quads[6]][2]);
	glVertex3f(vertices[quads[7]][0], vertices[quads[7]][1], vertices[quads[7]][2]);
	// Left Face
	glNormal3f(quadNormals[2][0], quadNormals[2][1], quadNormals[2][2]);
	glVertex3f(vertices[quads[8]][0], vertices[quads[8]][1], vertices[quads[8]][2]);
	glVertex3f(vertices[quads[9]][0], vertices[quads[9]][1], vertices[quads[9]][2]);
	glVertex3f(vertices[quads[10]][0], vertices[quads[10]][1], vertices[quads[10]][2]);
	glVertex3f(vertices[quads[11]][0], vertices[quads[11]][1], vertices[quads[11]][2]);
	// Right Face
	glNormal3f(quadNormals[3][0], quadNormals[3][1], quadNormals[3][2]);
	glVertex3f(vertices[quads[12]][0], vertices[quads[12]][1], vertices[quads[12]][2]);
	glVertex3f(vertices[quads[13]][0], vertices[quads[13]][1], vertices[quads[13]][2]);
	glVertex3f(vertices[quads[14]][0], vertices[quads[14]][1], vertices[quads[14]][2]);
	glVertex3f(vertices[quads[15]][0], vertices[quads[15]][1], vertices[quads[15]][2]);
	// Front Face
	glNormal3f(quadNormals[4][0], quadNormals[4][1], quadNormals[4][2]);
	glVertex3f(vertices[quads[16]][0], vertices[quads[16]][1], vertices[quads[16]][2]);
	glVertex3f(vertices[quads[17]][0], vertices[quads[17]][1], vertices[quads[17]][2]);
	glVertex3f(vertices[quads[18]][0], vertices[quads[18]][1], vertices[quads[18]][2]);
	glVertex3f(vertices[quads[19]][0], vertices[quads[19]][1], vertices[quads[19]][2]);
	// Bottom Face
	glNormal3f(quadNormals[5][0], quadNormals[5][1], quadNormals[5][2]);
	glVertex3f(vertices[quads[20]][0], vertices[quads[20]][1], vertices[quads[20]][2]);
	glVertex3f(vertices[quads[21]][0], vertices[quads[21]][1], vertices[quads[21]][2]);
	glVertex3f(vertices[quads[22]][0], vertices[quads[22]][1], vertices[quads[22]][2]);
	glVertex3f(vertices[quads[23]][0], vertices[quads[23]][1], vertices[quads[23]][2]);
	glEnd();
	glPopMatrix();
}
#pragma once
