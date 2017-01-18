#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glut.h>
#include <string.h>
#include <math.h>
#include <utility>
#include <vector>
#include "VECTOR3D.h"

#include "QuadMesh.h"


QuadMesh::QuadMesh(int maxMeshSize, float meshDim)
{
	minMeshSize = 1;
	numVertices = 0;
	vertices = NULL;
	numQuads = 0;
	quads = NULL;
	numFacesDrawn = 0;

	this->maxMeshSize = maxMeshSize < minMeshSize ? minMeshSize : maxMeshSize;
	this->meshDim = meshDim;
	CreateMemory();

	// Setup the material and lights used for the mesh
	mat_ambient[0] = 0.0;
	mat_ambient[1] = 0.0;
	mat_ambient[2] = 0.0;
	mat_ambient[3] = 1.0;
	mat_specular[0] = 0.0;
	mat_specular[1] = 0.0;
	mat_specular[2] = 0.0;
	mat_specular[3] = 1.0;
	mat_diffuse[0] = 0.9;
	mat_diffuse[1] = 0.5;
	mat_diffuse[2] = 0.0;
	mat_diffuse[3] = 1.0;
	mat_shininess[0] = 0.0;

}

void QuadMesh::SetMaterial(VECTOR3D ambient, VECTOR3D diffuse, VECTOR3D specular, double shininess)
{
	mat_ambient[0] = ambient.x;
	mat_ambient[1] = ambient.y;
	mat_ambient[2] = ambient.z;
	mat_ambient[3] = 1.0;
	mat_specular[0] = specular.x;
	mat_specular[1] = specular.y;
	mat_specular[2] = specular.z;
	mat_specular[3] = 1.0;
	mat_diffuse[0] = diffuse.x;
	mat_diffuse[1] = diffuse.y;
	mat_diffuse[2] = diffuse.z;
	mat_diffuse[3] = 1.0;
	mat_shininess[0] = shininess;
}

bool QuadMesh::CreateMemory()
{
	vertices = new MeshVertex[(maxMeshSize + 1)*(maxMeshSize + 1)];
	if (!vertices)
	{
		return false;
	}

	quads = new MeshQuad[maxMeshSize*maxMeshSize];
	if (!quads)
	{
		return false;
	}

	return true;
}



bool QuadMesh::InitMesh(int meshSize, VECTOR3D origin, double meshLength, double meshWidth, VECTOR3D dir1, VECTOR3D dir2)
{
	VECTOR3D o;
	int currentVertex = 0;
	double sf1, sf2;

	VECTOR3D v1, v2;

	v1.x = dir1.x;
	v1.y = dir1.y;
	v1.z = dir1.z;

	sf1 = meshLength / meshSize;
	v1 *= sf1;

	v2.x = dir2.x;
	v2.y = dir2.y;
	v2.z = dir2.z;
	sf2 = meshWidth / meshSize;
	v2 *= sf2;

	VECTOR3D meshpt;

	// VERTICES
	numVertices = (meshSize + 1)*(meshSize + 1);

	// Starts at front left corner of mesh 
	o.Set(origin.x, origin.y, origin.z);

	for (int i = 0; i< meshSize + 1; i++)
	{
		for (int j = 0; j< meshSize + 1; j++)
		{
			// compute vertex position along mesh row (along x direction)
			meshpt.x = o.x + j * v1.x;
			meshpt.y = o.y + j * v1.y;
			meshpt.z = o.z + j * v1.z;

			vertices[currentVertex].position.Set(meshpt.x, meshpt.y, meshpt.z);
			currentVertex++;
		}
		// go to next row in mesh (negative z direction)
		o += v2;
	}

	// Build Quad Polygons
	numQuads = (meshSize)*(meshSize);
	int currentQuad = 0;

	for (int j = 0; j < meshSize; j++)
	{
		for (int k = 0; k < meshSize; k++)
		{
			// Counterclockwise order
			quads[currentQuad].vertices[0] = &vertices[j*    (meshSize + 1) + k];
			quads[currentQuad].vertices[1] = &vertices[j*    (meshSize + 1) + k + 1];
			quads[currentQuad].vertices[2] = &vertices[(j + 1)*(meshSize + 1) + k + 1];
			quads[currentQuad].vertices[3] = &vertices[(j + 1)*(meshSize + 1) + k];
			currentQuad++;
		}
	}

	this->ComputeNormals();

	return true;
}

void QuadMesh::DrawMesh(int meshSize)
{
	int currentQuad = 0;

	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

	for (int j = 0; j< meshSize; j++)
	{
		for (int k = 0; k< meshSize; k++)
		{
			glBegin(GL_QUADS);

			glNormal3f(quads[currentQuad].vertices[0]->normal.x,
				quads[currentQuad].vertices[0]->normal.y,
				quads[currentQuad].vertices[0]->normal.z);
			glVertex3f(quads[currentQuad].vertices[0]->position.x,
				quads[currentQuad].vertices[0]->position.y,
				quads[currentQuad].vertices[0]->position.z);

			glNormal3f(quads[currentQuad].vertices[1]->normal.x,
				quads[currentQuad].vertices[1]->normal.y,
				quads[currentQuad].vertices[1]->normal.z);

			glVertex3f(quads[currentQuad].vertices[1]->position.x,
				quads[currentQuad].vertices[1]->position.y,
				quads[currentQuad].vertices[1]->position.z);

			glNormal3f(quads[currentQuad].vertices[2]->normal.x,
				quads[currentQuad].vertices[2]->normal.y,
				quads[currentQuad].vertices[2]->normal.z);

			glVertex3f(quads[currentQuad].vertices[2]->position.x,
				quads[currentQuad].vertices[2]->position.y,
				quads[currentQuad].vertices[2]->position.z);

			glNormal3f(quads[currentQuad].vertices[3]->normal.x,
				quads[currentQuad].vertices[3]->normal.y,
				quads[currentQuad].vertices[3]->normal.z);

			glVertex3f(quads[currentQuad].vertices[3]->position.x,
				quads[currentQuad].vertices[3]->position.y,
				quads[currentQuad].vertices[3]->position.z);
			glEnd();
			currentQuad++;
		}
	}
}







void QuadMesh::FreeMemory()
{
	if (vertices)
		delete[] vertices;
	vertices = NULL;
	numVertices = 0;

	if (quads)
		delete[] quads;
	quads = NULL;
	numQuads = 0;
}

void QuadMesh::ComputeNormals()
{
	int currentQuad = 0;

	for (int j = 0; j< this->maxMeshSize; j++)
	{
		for (int k = 0; k< this->maxMeshSize; k++)
		{
			VECTOR3D n0, n1, n2, n3, e0, e1, e2, e3, ne0, ne1, ne2, ne3;

			quads[currentQuad].vertices[0]->normal.LoadZero();
			quads[currentQuad].vertices[1]->normal.LoadZero();
			quads[currentQuad].vertices[2]->normal.LoadZero();
			quads[currentQuad].vertices[3]->normal.LoadZero();
			e0 = quads[currentQuad].vertices[1]->position - quads[currentQuad].vertices[0]->position;
			e1 = quads[currentQuad].vertices[2]->position - quads[currentQuad].vertices[1]->position;
			e2 = quads[currentQuad].vertices[3]->position - quads[currentQuad].vertices[2]->position;
			e3 = quads[currentQuad].vertices[0]->position - quads[currentQuad].vertices[3]->position;
			e0.Normalize();
			e1.Normalize();
			e2.Normalize();
			e3.Normalize();

			n0 = e0.CrossProduct(-e3);
			n0.Normalize();
			quads[currentQuad].vertices[0]->normal += n0;

			n1 = e1.CrossProduct(-e0);
			n1.Normalize();
			quads[currentQuad].vertices[1]->normal += n1;

			n2 = e2.CrossProduct(-e1);
			n2.Normalize();
			quads[currentQuad].vertices[2]->normal += n2;

			n3 = e3.CrossProduct(-e2);
			n3.Normalize();
			quads[currentQuad].vertices[3]->normal += n3;

			quads[currentQuad].vertices[0]->normal.Normalize();
			quads[currentQuad].vertices[1]->normal.Normalize();
			quads[currentQuad].vertices[2]->normal.Normalize();
			quads[currentQuad].vertices[3]->normal.Normalize();

			currentQuad++;
		}
	}
}
