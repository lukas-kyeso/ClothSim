//////////////////////////////////////////////////////////////////////////////////////////
//	Scene.cpp
#include <windows.h>
#include <stdio.h>
#include <GL/glut.h>
#include "Maths/Maths.h"
#include "scene.h"

void DrawSceneBro()
{
	//Display lists for objects
	static GLuint spheresList=0, torusList=0, baseList=0;

	//Create spheres list if necessary
	if(!spheresList)
	{
		spheresList=glGenLists(1);
		glNewList(spheresList, GL_COMPILE);
		{
			glColor3f(0.0f, 1.0f, 0.0f);
			glPushMatrix();

			glTranslatef(0.45f, 1.0f, 0.45f);
			glutSolidSphere(0.2, 24, 24);

			glTranslatef(-0.9f, 0.0f, 0.0f);
			glutSolidSphere(0.2, 24, 24);

			glTranslatef(0.0f, 0.0f,-0.9f);
			glutSolidSphere(0.2, 24, 24);

			glTranslatef(0.9f, 0.0f, 0.0f);
			glutSolidSphere(0.2, 24, 24);

			glPopMatrix();
		}
		glEndList();
	}

	//Create torus if necessary
	if(!torusList)
	{
		torusList=glGenLists(1);
		glNewList(torusList, GL_COMPILE);
		{
			glColor3f(1.0f, 0.0f, 0.0f);
			glPushMatrix();

			glTranslatef(0.0f, 0.5f, 0.0f);
			glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
			glutSolidTorus(0.2, 0.5, 24, 48);

			glPopMatrix();
		}
		glEndList();
	}

	//Create base if necessary
	if(!baseList)
	{
		baseList=glGenLists(1);
		glNewList(baseList, GL_COMPILE);
		{
			glColor3f(0.0f, 0.0f, 1.0f);
			glPushMatrix();

			glScalef(1.0f, 0.05f, 1.0f);
			glutSolidCube(3.0f);

			glPopMatrix();
		}
		glEndList();
	}


	//Draw objects
	glCallList(baseList);
	glCallList(torusList);

	glPushMatrix();
	glRotatef(angle, 0.0f, 1.0f, 0.0f);
	glCallList(spheresList);
	glPopMatrix();
}



