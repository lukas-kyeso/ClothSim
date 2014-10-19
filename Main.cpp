#include <stdio.h>
#include "Maths/Maths.h"
//#include "GLee/GLee.h"
#include <GL/glut.h>
#include <GL/gl.h>
#include <stdlib.h>
#include "string.h"
#include <string>
#include <math.h>
#include "VECTOR3D.h"
#include "VECTOR3D.cpp"
#include "SPRING.h"
#include "TIMER.h"
#include "TIMER.cpp"
#include "BALL.h"
#include "COLOR.h"
#include "G308_ImageLoader.h"
#include <png.h>
#include <cstdlib>

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
using namespace glm;

bool drawBalls=false, drawSprings=false;
bool drawTriangles=true, drawPatches=false;

TIMER timer;
float timeInterval = 5000;

BALL * currentBallsCloth1 = NULL;
BALL * nextBallsCloth1 = NULL;
BALL * currentBallsCloth2 = NULL;
BALL * nextBallsCloth2 = NULL;
BALL * currentBallsCloth3 = NULL;
BALL * nextBallsCloth3 = NULL;

// ball arrays
BALL * balls1Array1 = NULL;
BALL * balls1Array2 = NULL;
BALL * balls2Array1 = NULL;
BALL * balls2Array2 = NULL;
BALL * balls3Array1 = NULL;
BALL * balls3Array2 = NULL;

//cloth texture no joke
TextureInfo clothTexInfo;
GLuint clothTex = 99;
TextureInfo cloth2TexInfo;
GLuint cloth2Tex = 99;
TextureInfo cloth3TexInfo;
GLuint cloth3Tex = 99;
TextureInfo woodTexInfo;
GLuint woodTex = 99;


float windStrength = 0.0f;

//shadow map stuff/////////////////////////////////////
VECTOR3D cameraPosition(-2.5f, 3.5f,-2.5f);
VECTOR3D lightPosition(10.4f,10,11.6f);
const int shadowMapSize=512;
GLuint shadowMapTexture;
int windowWidth = 1000, windowHeight = 800;
mat4 lightProjectionMatrix, lightViewMatrix;
mat4 cameraProjectionMatrix, cameraViewMatrix;
static GLuint spheresList=0, baseList=0;
///////////////////////////////////////////////////////

// number of balls that make up the cloth
int numBalls;
int numBalls2;
int numBalls3;

// number of springs
int numSprings;
int numSprings2;
int numSprings3;

// spring array
SPRING * springArray = NULL;
SPRING * springArray2 = NULL;
SPRING * springArray3 = NULL;

// number of balls along the length of the cloth. Cloth is always square so we only need 1 value
int clothSize = 15;
int cloth2Size = 10;
int cloth3Size = 18;

float cloth1StartX = 0;
float cloth2StartX = 15;
float cloth3StartX = 30;

VECTOR3D gravity(0.0,-0.9,0.0);

float dampen=.9;

// mass of ball
float mass = 0.01;
float mass2 = 0.008;
float mass3 = 0.01;

//spring constants - higher = more taut
float springConstant = 30.0;
float springConstant2 = 40.0;
float springConstant3 = 10.0;

float naturalLength = 1.0;
float naturalLength2 = 1.0;
float naturalLength3 = 1.0;

float sphereRadius = 4;
float cubeSize = 5;
VECTOR3D cubePosition(0,0,0);
VECTOR3D spherePosition(7,0,0);



void DirectionalLight(){
	glPushMatrix();

	glEnable(GL_DEPTH_TEST);
	GLfloat    light_position[] = { 10.4,10,11.6, 1};
	GLfloat    light_white[] = {.3,.3,.3, 1 };
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_SMOOTH);
	glLightfv(GL_LIGHT0, GL_POSITION,light_position);
	glLightfv(GL_LIGHT0, GL_DIFFUSE,light_white    );
	glLightfv(GL_LIGHT0, GL_SPECULAR,light_white);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	//Use 2-sided lighting
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, true);

	glPopMatrix();
}
void SetCamera(){
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (double) windowWidth / (double) windowHeight, 1, 1000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(
			-10, 0, 20,
			0, 0, 0,
			0.0, 1.0, 0.0
	);
}
void ResetCloth1(){
	// start balls of at evenly spaced intervals
	for(int i = 0; i < clothSize; ++i){
		for(int j = 0; j<clothSize; ++j){
			balls1Array1[i*clothSize+j].position.Set(cloth1StartX + float(j)-float(clothSize-1)/2, 8.5, float(i)-float(clothSize-1)/2);
			balls1Array1[i*clothSize+j].velocity.LoadZero();
			balls1Array1[i*clothSize+j].mass = mass;
			balls1Array1[i*clothSize+j].fixed = false;
		}
	}

	// set corner balls in place
	for(int i = 0; i < clothSize-1; i++) balls1Array1[i].fixed = true;
	balls1Array1[clothSize-1].fixed = true;
	for(int i = 0; i < clothSize; i++) balls1Array1[i*clothSize + clothSize-1].fixed = true;
	balls1Array1[(clothSize-1)*(clothSize-1)].fixed = true;

	// copy balls into swap array
	for(int i = 0; i<numBalls; i++){
		balls1Array2[i] = balls1Array1[i];
	}

	// set pointers
	currentBallsCloth1 = balls1Array1;
	nextBallsCloth1 = balls1Array2;

	// initalize springs
	SPRING * currentSpring = &springArray[0];

	//The first (gridSize-1)*gridSize springs go from one ball to the next,
	//excluding those on the right hand edge
	for(int i=0; i<clothSize; ++i)
	{
		for(int j=0; j<clothSize-1; ++j)
		{
			currentSpring->ball1=i*clothSize+j;
			currentSpring->ball2=i*clothSize+j+1;

			currentSpring->springConstant=springConstant;
			currentSpring->naturalLength=naturalLength;

			++currentSpring;
		}
	}

	//The next (gridSize-1)*gridSize springs go from one ball to the one below,
	//excluding those on the bottom edge
	for(int i=0; i<clothSize-1; ++i)
	{
		for(int j=0; j<clothSize; ++j)
		{
			currentSpring->ball1=i*clothSize+j;
			currentSpring->ball2=(i+1)*clothSize+j;

			currentSpring->springConstant=springConstant;
			currentSpring->naturalLength=naturalLength;

			++currentSpring;
		}
	}

	//The next (gridSize-1)*(gridSize-1) go from a ball to the one below and right
	//excluding those on the bottom or right
	for(int i=0; i<clothSize-1; ++i)
	{
		for(int j=0; j<clothSize-1; ++j)
		{
			currentSpring->ball1=i*clothSize+j;
			currentSpring->ball2=(i+1)*clothSize+j+1;

			currentSpring->springConstant=springConstant;
			currentSpring->naturalLength=naturalLength*sqrt(2.0f);

			++currentSpring;
		}
	}

	//The next (gridSize-1)*(gridSize-1) go from a ball to the one below and left
	//excluding those on the bottom or right
	for(int i=0; i<clothSize-1; ++i)
	{
		for(int j=1; j<clothSize; ++j)
		{
			currentSpring->ball1=i*clothSize+j;
			currentSpring->ball2=(i+1)*clothSize+j-1;

			currentSpring->springConstant=springConstant;
			currentSpring->naturalLength=naturalLength*sqrt(2.0f);

			++currentSpring;
		}
	}

	//The first (gridSize-2)*gridSize springs go from one ball to the next but one,
	//excluding those on or next to the right hand edge
	for(int i=0; i<clothSize; ++i)
	{
		for(int j=0; j<clothSize-2; ++j)
		{
			currentSpring->ball1=i*clothSize+j;
			currentSpring->ball2=i*clothSize+j+2;

			currentSpring->springConstant=springConstant;
			currentSpring->naturalLength=naturalLength*2;

			++currentSpring;
		}
	}

	//The next (gridSize-2)*gridSize springs go from one ball to the next but one below,
	//excluding those on or next to the bottom edge
	for(int i=0; i<clothSize-2; ++i)
	{
		for(int j=0; j<clothSize; ++j)
		{
			currentSpring->ball1=i*clothSize+j;
			currentSpring->ball2=(i+2)*clothSize+j;

			currentSpring->springConstant=springConstant;
			currentSpring->naturalLength=naturalLength*2;

			++currentSpring;
		}
	}

}
void ResetCloth2(){

	// start balls of at evenly spaced intervals
	for(int i = 0; i < cloth2Size; ++i){
		for(int j = 0; j<cloth2Size; ++j){
			balls2Array1[i*cloth2Size+j].position.Set(cloth2StartX + float(j)-float(cloth2Size-1)/2, 8.5, float(i)-float(cloth2Size-1)/2);
			balls2Array1[i*cloth2Size+j].velocity.LoadZero();
			balls2Array1[i*cloth2Size+j].mass = mass2;
			balls2Array1[i*cloth2Size+j].fixed = false;
		}
	}

	// set corner balls in place
	for(int i = 0; i < cloth2Size-1; i++) balls2Array1[i].fixed = true;
	balls2Array1[cloth2Size-1].fixed = true;
	for(int i = 0; i < cloth2Size; i++) balls2Array1[i*cloth2Size + cloth2Size-1].fixed = true;
	balls2Array1[cloth2Size*cloth2Size-1].fixed = true;

	// copy balls into swap array
	for(int i = 0; i<numBalls; i++){
		balls2Array2[i] = balls2Array1[i];
	}

	// set pointers
	currentBallsCloth2 = balls2Array1;
	nextBallsCloth2 = balls2Array2;

	// initalize springs
	SPRING * currentSpring = &springArray2[0];

	//The first (gridSize-1)*gridSize springs go from one ball to the next,
	//excluding those on the right hand edge
	for(int i=0; i<cloth2Size; ++i)
	{
		for(int j=0; j<cloth2Size-1; ++j)
		{
			currentSpring->ball1=i*cloth2Size+j;
			currentSpring->ball2=i*cloth2Size+j+1;

			currentSpring->springConstant=springConstant2;
			currentSpring->naturalLength=naturalLength2;

			++currentSpring;
		}
	}

	//The next (gridSize-1)*gridSize springs go from one ball to the one below,
	//excluding those on the bottom edge
	for(int i=0; i<cloth2Size-1; ++i)
	{
		for(int j=0; j<cloth2Size; ++j)
		{
			currentSpring->ball1=i*cloth2Size+j;
			currentSpring->ball2=(i+1)*cloth2Size+j;

			currentSpring->springConstant=springConstant2;
			currentSpring->naturalLength=naturalLength2;

			++currentSpring;
		}
	}

	//The next (gridSize-1)*(gridSize-1) go from a ball to the one below and right
	//excluding those on the bottom or right
	for(int i=0; i<cloth2Size-1; ++i)
	{
		for(int j=0; j<cloth2Size-1; ++j)
		{
			currentSpring->ball1=i*cloth2Size+j;
			currentSpring->ball2=(i+1)*cloth2Size+j+1;

			currentSpring->springConstant=springConstant2;
			currentSpring->naturalLength=naturalLength2*sqrt(2.0f);

			++currentSpring;
		}
	}

	//The next (gridSize-1)*(gridSize-1) go from a ball to the one below and left
	//excluding those on the bottom or right
	for(int i=0; i<cloth2Size-1; ++i)
	{
		for(int j=1; j<cloth2Size; ++j)
		{
			currentSpring->ball1=i*cloth2Size+j;
			currentSpring->ball2=(i+1)*cloth2Size+j-1;

			currentSpring->springConstant=springConstant2;
			currentSpring->naturalLength=naturalLength2*sqrt(2.0f);

			++currentSpring;
		}
	}

	//The first (gridSize-2)*gridSize springs go from one ball to the next but one,
	//excluding those on or next to the right hand edge
	for(int i=0; i<cloth2Size; ++i)
	{
		for(int j=0; j<cloth2Size-2; ++j)
		{
			currentSpring->ball1=i*cloth2Size+j;
			currentSpring->ball2=i*cloth2Size+j+2;

			currentSpring->springConstant=springConstant2;
			currentSpring->naturalLength=naturalLength2*2;

			++currentSpring;
		}
	}

	//The next (gridSize-2)*gridSize springs go from one ball to the next but one below,
	//excluding those on or next to the bottom edge
	for(int i=0; i<cloth2Size-2; ++i)
	{
		for(int j=0; j<cloth2Size; ++j)
		{
			currentSpring->ball1=i*cloth2Size+j;
			currentSpring->ball2=(i+2)*cloth2Size+j;

			currentSpring->springConstant=springConstant2;
			currentSpring->naturalLength=naturalLength2*2;

			++currentSpring;
		}
	}

}
void ResetCloth3(){

	// start balls of at evenly spaced intervals
	for(int i = 0; i < cloth3Size; ++i){
		for(int j = 0; j<cloth3Size; ++j){
			balls3Array1[i*cloth3Size+j].position.Set(cloth3StartX + float(j)-float(cloth3Size-1)/2, 8.5, float(i)-float(cloth3Size-1)/2);
			balls3Array1[i*cloth3Size+j].velocity.LoadZero();
			balls3Array1[i*cloth3Size+j].mass = mass3;
			balls3Array1[i*cloth3Size+j].fixed = false;
		}
	}

	// set corner balls in placeq
	for(int i = 0; i < cloth3Size-1; i++) balls3Array1[i].fixed = true;
	balls3Array1[cloth3Size-1].fixed = true;
	for(int i = 0; i < cloth3Size; i++) balls3Array1[i*cloth3Size + cloth3Size-1].fixed = true;
	balls3Array1[cloth3Size*cloth3Size-1].fixed = true;

	// copy balls into swap array
	for(int i = 0; i<numBalls3; i++){
		balls1Array2[i] = balls3Array1[i];
	}

	// set pointers
	currentBallsCloth3 = balls3Array1;
	nextBallsCloth3 = balls3Array2;

	// initalize springs
	SPRING * currentSpring = &springArray3[0];

	//The first (gridSize-1)*gridSize springs go from one ball to the next,
	//excluding those on the right hand edge
	for(int i=0; i<cloth3Size; ++i)
	{
		for(int j=0; j<cloth3Size-1; ++j)
		{
			currentSpring->ball1=i*cloth3Size+j;
			currentSpring->ball2=i*cloth3Size+j+1;

			currentSpring->springConstant=springConstant3;
			currentSpring->naturalLength=naturalLength3;

			++currentSpring;
		}
	}

	//The next (gridSize-1)*gridSize springs go from one ball to the one below,
	//excluding those on the bottom edge
	for(int i=0; i<cloth3Size-1; ++i)
	{
		for(int j=0; j<cloth3Size; ++j)
		{
			currentSpring->ball1=i*cloth3Size+j;
			currentSpring->ball2=(i+1)*cloth3Size+j;

			currentSpring->springConstant=springConstant3;
			currentSpring->naturalLength=naturalLength3;

			++currentSpring;
		}
	}

	//The next (gridSize-1)*(gridSize-1) go from a ball to the one below and right
	//excluding those on the bottom or right
	for(int i=0; i<cloth3Size-1; ++i)
	{
		for(int j=0; j<cloth3Size-1; ++j)
		{
			currentSpring->ball1=i*cloth3Size+j;
			currentSpring->ball2=(i+1)*cloth3Size+j+1;

			currentSpring->springConstant=springConstant3;
			currentSpring->naturalLength=naturalLength3*sqrt(2.0f);

			++currentSpring;
		}
	}

	//The next (gridSize-1)*(gridSize-1) go from a ball to the one below and left
	//excluding those on the bottom or right
	for(int i=0; i<cloth3Size-1; ++i)
	{
		for(int j=1; j<cloth3Size; ++j)
		{
			currentSpring->ball1=i*cloth3Size+j;
			currentSpring->ball2=(i+1)*cloth3Size+j-1;

			currentSpring->springConstant=springConstant3;
			currentSpring->naturalLength=naturalLength3*sqrt(2.0f);

			++currentSpring;
		}
	}

	//The first (gridSize-2)*gridSize springs go from one ball to the next but one,
	//excluding those on or next to the right hand edge
	for(int i=0; i<cloth3Size; ++i)
	{
		for(int j=0; j<cloth3Size-2; ++j)
		{
			currentSpring->ball1=i*cloth3Size+j;
			currentSpring->ball2=i*cloth3Size+j+2;

			currentSpring->springConstant=springConstant3;
			currentSpring->naturalLength=naturalLength3*2;

			++currentSpring;
		}
	}

	//The next (gridSize-2)*gridSize springs go from one ball to the next but one below,
	//excluding those on or next to the bottom edge
	for(int i=0; i<cloth3Size-2; ++i)
	{
		for(int j=0; j<cloth3Size; ++j)
		{
			currentSpring->ball1=i*cloth3Size+j;
			currentSpring->ball2=(i+2)*cloth3Size+j;

			currentSpring->springConstant=springConstant3;
			currentSpring->naturalLength=naturalLength3*2;

			++currentSpring;
		}
	}

}

bool VariablesInit(){
	// total number of balls
	numBalls = clothSize * clothSize;
	numBalls2= cloth2Size * cloth2Size;
	numBalls3 = cloth3Size * cloth3Size;

	numSprings = (clothSize - 1) * clothSize * 2;
	numSprings += (clothSize - 1) * (clothSize - 1)*2;
	numSprings += (clothSize - 2)*clothSize*2;
	numSprings2 = (cloth2Size - 1) * cloth2Size * 2;
	numSprings2 += (cloth2Size - 1) * (cloth2Size - 1)*2;
	numSprings2 += (cloth2Size - 2)*cloth2Size*2;
	numSprings3 = (cloth3Size - 1) * cloth3Size * 2;
	numSprings3 += (cloth3Size - 1) * (cloth3Size - 1)*2;
	numSprings3 += (cloth3Size - 2)*cloth3Size*2;

	balls1Array1 = new BALL[numBalls];
	balls1Array2 = new BALL[numBalls];
	balls2Array1 = new BALL[numBalls2];
	balls2Array2 = new BALL[numBalls2];
	balls3Array1 = new BALL[numBalls3];
	balls3Array2 = new BALL[numBalls3];
	springArray = new SPRING[numSprings];
	springArray2 = new SPRING[numSprings2];
	springArray3 = new SPRING[numSprings3];

	ResetCloth1();
	ResetCloth2();
	ResetCloth3();

	timer.Reset();
}

void drawCloth1(){
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glBindTexture(GL_TEXTURE_2D, clothTex);

	glEnable(GL_LIGHTING);
	glMaterialfv(GL_FRONT, GL_AMBIENT, COLOR(0.8f, 0.0f, 1.0f));    //set material
	glMaterialfv(GL_FRONT, GL_DIFFUSE, COLOR(0.8f, 0.0f, 1.0f));
	glMaterialfv(GL_BACK, GL_AMBIENT, COLOR(1.0f, 1.0f, 0.0f));
	glMaterialfv(GL_BACK, GL_DIFFUSE, COLOR(1.0f, 1.0f, 0.0f));
	glBegin(GL_TRIANGLES);
	{
		for(int i=1; i<clothSize-1; ++i)
		{
			for(int j=1; j<clothSize-1; ++j)
			{
				glNormal3fv(currentBallsCloth1[i*clothSize+j].normal);
				glTexCoord2f(((float)i/clothSize), (float)j/clothSize);
				glVertex3fv(currentBallsCloth1[i*clothSize+j].position);
				glNormal3fv(currentBallsCloth1[i*clothSize+j+1].normal);
				glTexCoord2f(((float)i/clothSize), ((float)j+1)/clothSize);
				glVertex3fv(currentBallsCloth1[i*clothSize+j+1].position);
				glNormal3fv(currentBallsCloth1[(i+1)*clothSize+j].normal);
				glTexCoord2f((((float)i+1)/clothSize), (float)j/clothSize);
				glVertex3fv(currentBallsCloth1[(i+1)*clothSize+j].position);

				glNormal3fv(currentBallsCloth1[(i+1)*clothSize+j].normal);
				glTexCoord2f((((float)i+1)/clothSize), (float)j/clothSize);
				glVertex3fv(currentBallsCloth1[(i+1)*clothSize+j].position);
				glNormal3fv(currentBallsCloth1[i*clothSize+j+1].normal);
				glTexCoord2f(((float)i/clothSize), ((float)j+1)/clothSize);
				glVertex3fv(currentBallsCloth1[i*clothSize+j+1].position);
				glNormal3fv(currentBallsCloth1[(i+1)*clothSize+j+1].normal);
				glTexCoord2f((((float)i+1)/clothSize), ((float)j+1)/clothSize);
				glVertex3fv(currentBallsCloth1[(i+1)*clothSize+j+1].position);
			}
		}
	}
	glEnd();
	glDisable(GL_LIGHTING);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void drawCloth2(){
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glBindTexture(GL_TEXTURE_2D, cloth2Tex);

	glEnable(GL_LIGHTING);
	glMaterialfv(GL_FRONT, GL_AMBIENT, COLOR(0.8f, 0.0f, 1.0f));    //set material
	glMaterialfv(GL_FRONT, GL_DIFFUSE, COLOR(0.8f, 0.0f, 1.0f));
	glMaterialfv(GL_BACK, GL_AMBIENT, COLOR(1.0f, 1.0f, 0.0f));
	glMaterialfv(GL_BACK, GL_DIFFUSE, COLOR(1.0f, 1.0f, 0.0f));
	glBegin(GL_TRIANGLES);
	{
		for(int i=1; i<cloth2Size-1; ++i)
		{
			for(int j=1; j<cloth2Size-1; ++j)
			{
				glNormal3fv(currentBallsCloth2[i*cloth2Size+j].normal);
				glTexCoord2f(((float)i/cloth2Size), (float)j/cloth2Size);
				glVertex3fv(currentBallsCloth2[i*cloth2Size+j].position);
				glNormal3fv(currentBallsCloth2[i*cloth2Size+j+1].normal);
				glTexCoord2f(((float)i/cloth2Size), ((float)j+1)/cloth2Size);
				glVertex3fv(currentBallsCloth2[i*cloth2Size+j+1].position);
				glNormal3fv(currentBallsCloth2[(i+1)*cloth2Size+j].normal);
				glTexCoord2f((((float)i+1)/cloth2Size), (float)j/cloth2Size);
				glVertex3fv(currentBallsCloth2[(i+1)*cloth2Size+j].position);

				glNormal3fv(currentBallsCloth2[(i+1)*cloth2Size+j].normal);
				glTexCoord2f((((float)i+1)/cloth2Size), (float)j/cloth2Size);
				glVertex3fv(currentBallsCloth2[(i+1)*cloth2Size+j].position);
				glNormal3fv(currentBallsCloth2[i*cloth2Size+j+1].normal);
				glTexCoord2f(((float)i/cloth2Size), ((float)j+1)/cloth2Size);
				glVertex3fv(currentBallsCloth2[i*cloth2Size+j+1].position);
				glNormal3fv(currentBallsCloth2[(i+1)*cloth2Size+j+1].normal);
				glTexCoord2f((((float)i+1)/cloth2Size), ((float)j+1)/cloth2Size);
				glVertex3fv(currentBallsCloth2[(i+1)*cloth2Size+j+1].position);
			}
		}
	}
	glEnd();
	glDisable(GL_LIGHTING);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void drawCloth3(){
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glBindTexture(GL_TEXTURE_2D, cloth3Tex);

	glEnable(GL_LIGHTING);
	glMaterialfv(GL_FRONT, GL_AMBIENT, COLOR(0.8f, 0.0f, 1.0f));    //set material
	glMaterialfv(GL_FRONT, GL_DIFFUSE, COLOR(0.8f, 0.0f, 1.0f));
	glMaterialfv(GL_BACK, GL_AMBIENT, COLOR(1.0f, 1.0f, 0.0f));
	glMaterialfv(GL_BACK, GL_DIFFUSE, COLOR(1.0f, 1.0f, 0.0f));
	glBegin(GL_TRIANGLES);
	{
		for(int i=1; i<cloth3Size-1; ++i)
		{
			for(int j=1; j<cloth3Size-1; ++j)
			{
				glNormal3fv(currentBallsCloth3[i*cloth3Size+j].normal);
				glTexCoord2f(((float)i/cloth3Size), (float)j/cloth3Size);
				glVertex3fv(currentBallsCloth3[i*cloth3Size+j].position);
				glNormal3fv(currentBallsCloth3[i*cloth3Size+j+1].normal);
				glTexCoord2f(((float)i/cloth3Size), ((float)j+1)/cloth3Size);
				glVertex3fv(currentBallsCloth3[i*cloth3Size+j+1].position);
				glNormal3fv(currentBallsCloth3[(i+1)*cloth3Size+j].normal);
				glTexCoord2f((((float)i+1)/cloth3Size), (float)j/cloth3Size);
				glVertex3fv(currentBallsCloth3[(i+1)*cloth3Size+j].position);

				glNormal3fv(currentBallsCloth3[(i+1)*cloth3Size+j].normal);
				glTexCoord2f((((float)i+1)/cloth3Size), (float)j/cloth3Size);
				glVertex3fv(currentBallsCloth3[(i+1)*cloth3Size+j].position);
				glNormal3fv(currentBallsCloth3[i*cloth3Size+j+1].normal);
				glTexCoord2f(((float)i/cloth3Size), ((float)j+1)/cloth3Size);
				glVertex3fv(currentBallsCloth3[i*cloth3Size+j+1].position);
				glNormal3fv(currentBallsCloth3[(i+1)*cloth3Size+j+1].normal);
				glTexCoord2f((((float)i+1)/cloth3Size), ((float)j+1)/cloth3Size);
				glVertex3fv(currentBallsCloth3[(i+1)*cloth3Size+j+1].position);
			}
		}
	}
	glEnd();
	glDisable(GL_LIGHTING);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void Render(double currentTime, double timePassed){
	// MIGHT NEED TO POSTREDISPLAY
	glutPostRedisplay();

	SetCamera();
	DirectionalLight();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glEnable( GL_MULTISAMPLE );
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	glTranslatef(0.0f, 0.0f, -28.0f);

	//Draw sphere (at origin)
	static GLUquadricObj * sphere=gluNewQuadric();
	glEnable(GL_LIGHTING);
	glMaterialfv(GL_FRONT, GL_AMBIENT, COLOR(1.0f, 0.0f, 0.0f, 0.0f));
	glMaterialfv(GL_FRONT, GL_DIFFUSE, COLOR(1.0, 0.0, 0.0, 0.0));
	glMaterialfv(GL_FRONT, GL_SPECULAR, white);
	glMaterialf(GL_FRONT, GL_SHININESS, 32.0f);
	//glEnable(GL_CULL_FACE);

	glPushMatrix();
	glColor3f(1.0f, 1.0f, 1.0f);
	glTranslatef(spherePosition.x, 0, 0);
	//    glTranslatef(2, 0, 0);
	//gluSphere(sphere, sphereRadius * .94, 48, 24);
	glutSolidSphere(sphereRadius*0.94, 48, 24);
	glPopMatrix();

	// DRAW CUBE
	glPushMatrix();
	glColor3f(2.0,1.0,1.0);
	glTranslatef(cubePosition.x, cubePosition.y, cubePosition.z);
	glutSolidCube(cubeSize * 0.75);
	glPopMatrix();

	//draw floor
	float floorX = 50;
	float floorY = -8.6;
	float floorz = 50;
	glColor4fv(white);
	glBindTexture(GL_TEXTURE_2D, woodTex);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_TRIANGLE_STRIP);
	{
		glNormal3f(0.0f,0.0f,1.0f);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-floorX, floorY, floorz);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f( floorX, floorY, floorz);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-floorX, floorY,-floorz);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f( floorX, floorY,-floorz);
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);

	//Draw cloths as triangles
	if(drawTriangles)
	{
		drawCloth1();
		drawCloth2();
		drawCloth3();
	}

	if(drawBalls)
	{
		glColor3f(1.0f, 0.0f, 0.0f);
		glPointSize(5.0f);
		glBegin(GL_POINTS);
		{
			for(int i=0; i<numBalls; ++i)
				glVertex3fv(currentBallsCloth1[i].position);
		}
		glEnd();
	}

	if(drawSprings)
	{
		glColor3f(0.0f, 0.0f, 1.0f);

		//bring the lines closer to prevent z-fighting with triangles
		glPushMatrix();
		glTranslatef(0.0f, 0.0f, 0.5f);

		glBegin(GL_LINES);
		{
			for(int i=0; i<numSprings; ++i)
			{
				//Check the spring has been initialised and the ball numbers are in bounds
				if(springArray[i].ball1!=-1 && springArray[i].ball2!=-1 && springArray[i].ball1<numBalls && springArray[i].ball2<numBalls)
				{
					glVertex3fv(currentBallsCloth1[springArray[i].ball1].position);
					glVertex3fv(currentBallsCloth1[springArray[i].ball2].position);
				}
			}
		}
		glEnd();

		glPopMatrix();
	}
	glDisable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);
	glMaterialfv(GL_FRONT, GL_SPECULAR, black);

	// stop drawing shit
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);

	glutSwapBuffers();
}

void shadowMapFirstPass(){
	//First pass - from light's point of view
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(value_ptr((lightProjectionMatrix)));

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(value_ptr(lightViewMatrix));

	//Use viewport the same size as the shadow map
	glViewport(0, 0, shadowMapSize, shadowMapSize);

	//Draw back faces into the shadow map
	glCullFace(GL_FRONT);

	//Disable color writes, and use flat shading for speed
	glShadeModel(GL_FLAT);
	glColorMask(0, 0, 0, 0);
}
void shadowMapSecondPass(){
	//leftovers from 1st pass/////////////////////////
	//Read the depth buffer into the shadow map texture
	glBindTexture(GL_TEXTURE_2D, shadowMapTexture);
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, shadowMapSize, shadowMapSize);

	//restore states
	glCullFace(GL_BACK);
	glShadeModel(GL_SMOOTH);
	glColorMask(1, 1, 1, 1);
	//////////////////////////////////////////////////


	//2nd pass - Draw from camera's point of view
	glClear(GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(value_ptr(cameraProjectionMatrix));

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(value_ptr(cameraViewMatrix));

	glViewport(0, 0, windowWidth, windowHeight);

	//Use dim light to represent shadowed areas
	glLightfv(GL_LIGHT1, GL_POSITION, VECTOR4D(lightPosition));
	glLightfv(GL_LIGHT1, GL_AMBIENT, white*0.2f);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, white*0.2f);
	glLightfv(GL_LIGHT1, GL_SPECULAR, black);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHTING);
}
void shadowMapThirdPass(){
	//3rd pass
	//Draw with bright light
	glLightfv(GL_LIGHT1, GL_DIFFUSE, white);
	glLightfv(GL_LIGHT1, GL_SPECULAR, white);

	//Calculate texture matrix for projection
	//This matrix takes us from eye space to the light's clip space
	//It is postmultiplied by the inverse of the current view matrix when specifying texgen
	static mat4 biasMatrix(0.5f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.5f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.5f, 0.0f,
			0.5f, 0.5f, 0.5f, 1.0f);	//bias from [-1, 1] to [0, 1]

	mat4 textureMatrix=biasMatrix*lightProjectionMatrix*lightViewMatrix;

	//Set up texture coordinate generation.
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGenfv(GL_S, GL_EYE_PLANE, value_ptr(textureMatrix[0]));
	glEnable(GL_TEXTURE_GEN_S);

	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGenfv(GL_T, GL_EYE_PLANE, value_ptr(textureMatrix[1]));
	glEnable(GL_TEXTURE_GEN_T);

	glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGenfv(GL_R, GL_EYE_PLANE, value_ptr(textureMatrix[2]));
	glEnable(GL_TEXTURE_GEN_R);

	glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGenfv(GL_Q, GL_EYE_PLANE, value_ptr(textureMatrix[3]));
	glEnable(GL_TEXTURE_GEN_Q);

	//Bind & enable shadow map texture
	glBindTexture(GL_TEXTURE_2D, shadowMapTexture);
	glEnable(GL_TEXTURE_2D);

	//Enable shadow comparison
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE);

	//Shadow comparison should be true (ie not in shadow) if r<=texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_ARB, GL_LEQUAL);

	//Shadow comparison should generate an INTENSITY result
	glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE_ARB, GL_INTENSITY);

	//Set alpha test to discard false comparisons
	glAlphaFunc(GL_GEQUAL, 0.99f);
	glEnable(GL_ALPHA_TEST);
}

void DrawScene(){
	if(!spheresList)
	{
		spheresList=glGenLists(1);
		glNewList(spheresList, GL_COMPILE);
		{
			glColor3f(0.0f, 1.0f, 0.0f);
			glPushMatrix();

			glTranslatef(0.9f, 0.0f, 0.0f);
			glutSolidSphere(spherePosition.x, 0, 0);

			glPopMatrix();
		}
		glEndList();
	}

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

	//draw
	glCallList(baseList);
	glCallList(spheresList);
}

void calcSwapArray(int numBallz, BALL nextBallsCloth[], BALL currentBallsCloth[], int numSpringz, SPRING springzArray[], float timePassed){
	// calculate swap array
	for(int i = 0; i < numBallz; ++i){
		nextBallsCloth[i].fixed = currentBallsCloth[i].fixed;
		nextBallsCloth[i].mass = currentBallsCloth[i].mass;

		if(currentBallsCloth[i].fixed)
		{
			nextBallsCloth[i].position = currentBallsCloth[i].position;
			nextBallsCloth[i].velocity.LoadZero();
		}
		else
		{
			VECTOR3D forceOnBall = gravity;

			// add pull of springs to forceOnBall
			for(int j = 0; j < numSpringz; ++j)
			{
				if(i == springzArray[j].ball1){
					VECTOR3D pull = currentBallsCloth[springzArray[j].ball2].position - currentBallsCloth[i].position;
					pull.Normalize();

					forceOnBall += springzArray[j].tension * pull;
				}
				if(i == springzArray[j].ball2){
					VECTOR3D pull = currentBallsCloth[springzArray[j].ball1].position - currentBallsCloth[i].position;
					pull.Normalize();

					forceOnBall += springzArray[j].tension * pull;
				}
			}

			VECTOR3D accel = forceOnBall / currentBallsCloth[i].mass;

			nextBallsCloth[i].velocity = currentBallsCloth[i].velocity + accel * timePassed;

			nextBallsCloth[i].velocity *= dampen;

			//    BALL nextPos = BALL();
			//    tempBall.position = currentBalls[i].position + (nextBalls[i].velocity + currentBalls[i].velocity) * timePassedInSeconds / 2;
			nextBallsCloth[i].position = currentBallsCloth[i].position + (nextBallsCloth[i].velocity + currentBallsCloth[i].velocity) * timePassed /2 ;




			//for each wind ball, do the following TODO improve
			int maxPercentOfWindStr = 2000;
			float randStrMod = (((float)(rand() % maxPercentOfWindStr))/100.0f)*windStrength;
			float randChance = ((float)(rand() % 100));
			float windStrMod = randChance > 95 ? windStrength + randStrMod : windStrength;
			VECTOR3D windDir; windDir.x = windStrMod; windDir.y = -windStrMod/2; windDir.z = 0;

			int clothsize = (int)sqrtf((double)numBallz);
			int x = i%clothsize;
			int y = i/clothsize;
			VECTOR3D & p0=nextBallsCloth[x*clothsize+y].position;
			VECTOR3D & p1=nextBallsCloth[x*clothsize+y+1].position;
			VECTOR3D & p2=nextBallsCloth[(x+1)*clothsize+y].position;

			VECTOR3D triangleNormal=(p1-p0).CrossProduct(p2-p0);
			VECTOR3D triNormized = triangleNormal; triNormized.Normalize();
			VECTOR3D force = triangleNormal*(triNormized.DotProduct(windDir));

			if(!nextBallsCloth[x*clothsize+y].fixed && !nextBallsCloth[x*clothsize+y+1].fixed && !nextBallsCloth[(x+1)*clothsize+y].fixed){
				nextBallsCloth[x*clothsize+y].velocity += force;
				nextBallsCloth[x*clothsize+y+1].velocity += force;
				nextBallsCloth[(x+1)*clothsize+y].velocity += force;
			}




			float distanceBetweenBallAndSphere = (spherePosition - nextBallsCloth[i].position).GetSquaredLength();
			if(distanceBetweenBallAndSphere < sphereRadius  * sphereRadius )
			{
				nextBallsCloth[i].position = spherePosition + ((nextBallsCloth[i].position - spherePosition).GetNormalized() * sphereRadius );
				nextBallsCloth[i].velocity *= dampen;
			}

			// check if in cube
			bool withinYRange = nextBallsCloth[i].position.y > cubePosition.y - cubeSize / 2 && nextBallsCloth[i].position.y < cubePosition.y + cubeSize / 2 ? true : false;
			bool withinXRange = nextBallsCloth[i].position.x > cubePosition.x - cubeSize / 2 && nextBallsCloth[i].position.x < cubePosition.x + cubeSize / 2 ? true : false;
			bool withinZRange = nextBallsCloth[i].position.z > cubePosition.z - cubeSize / 2 && nextBallsCloth[i].position.z < cubePosition.z + cubeSize / 2 ? true : false;
			if(withinYRange && withinXRange && withinZRange){
				//	printf("yes within ze cube\n");
				float closestSide = 7;
				float closestDistance = cubeSize * 100;
				float distanceToTop = abs(nextBallsCloth[i].position.y - (cubePosition.y + cubeSize / 2));
				closestSide = 1;
				closestDistance = distanceToTop;
				float distanceToBottom = abs(nextBallsCloth[i].position.y - (cubePosition.y - cubeSize / 2));
				if (distanceToBottom < closestDistance){
					closestSide = 2;
					closestDistance = distanceToBottom;
				}
				float distanceToFront = abs(nextBallsCloth[i].position.z - (cubePosition.z - cubeSize / 2));
				if (distanceToFront < closestDistance){
					closestSide = 3;
					closestDistance = distanceToFront;
				}
				float distanceToBack = abs(nextBallsCloth[i].position.z - (cubePosition.z + cubeSize / 2));
				if (distanceToBack < closestDistance){
					closestSide = 4;
					closestDistance = distanceToBack;
				}
				float distanceToLeft = abs(nextBallsCloth[i].position.x - (cubePosition.x - cubeSize / 2));
				if (distanceToLeft < closestDistance){
					closestSide = 5;
					closestDistance = distanceToLeft;
				}
				float distanceToRight = abs(nextBallsCloth[i].position.x - (cubePosition.x + cubeSize / 2));
				if (distanceToRight < closestDistance){
					closestSide = 6;
					closestDistance = distanceToRight;
				}
				if(nextBallsCloth[i].position.y > (cubePosition.y + cubeSize / 2) - 1 && closestSide == 1){
					nextBallsCloth[i].position.y = cubePosition.y + cubeSize / 2;
				}
				else{
					//		printf("top = %f bottom = %f front = %f back = %f left = %f right = %f\n", distanceToTop, distanceToBottom, distanceToFront, distanceToBack, distanceToLeft, distanceToRight);
					//	printf("closetsSide = %f\n", closestSide);
					if(closestSide == 3){
						nextBallsCloth[i].position.z = cubePosition.z - cubeSize / 2;
					}
					if(closestSide == 4){
						nextBallsCloth[i].position.z = cubePosition.z + cubeSize / 2;
					}
					if(closestSide == 5){
						nextBallsCloth[i].position.x = cubePosition.x - cubeSize / 2;
					}
					if(closestSide == 6){
						nextBallsCloth[i].position.x = cubePosition.x + cubeSize / 2;
					}
				}
				nextBallsCloth[i].velocity.LoadZero();
			}



			if(nextBallsCloth[i].position.y < -8){
				nextBallsCloth[i].position.y = -8;
			}
		}
	}
}


void Physics(void)
{
	static double lastTime = timer.GetTime();
	double currentTime = timer.GetTime();
	double timePassed = currentTime - lastTime;
	lastTime = currentTime;

	static double timeSinceLastUpdate = 0.0;
	timeSinceLastUpdate+=timePassed;

	bool updateMade = false;
	while(timeSinceLastUpdate > timeInterval)
	{
		timeSinceLastUpdate -= timeInterval;
		float timePassedInSeconds = 0.01;
		updateMade = true;

		// calculate tensions
		//cloth1
		for(int i = 0; i < numSprings; ++i){
			float springLength = (currentBallsCloth1[springArray[i].ball1].position - currentBallsCloth1[springArray[i].ball2].position).GetLength();
			float stretch = springLength - springArray[i].naturalLength;
			float tension = springArray[i].springConstant * stretch / springArray[i].naturalLength;

			springArray[i].tension = tension;
		}
		//cloth2
		for(int i = 0; i < numSprings2; ++i){
			float springLength = (currentBallsCloth2[springArray2[i].ball1].position - currentBallsCloth2[springArray2[i].ball2].position).GetLength();
			float stretch = springLength - springArray2[i].naturalLength;
			float tension = springArray2[i].springConstant * stretch / springArray2[i].naturalLength;

			springArray2[i].tension = tension;
		}
		//cloth3
		for(int i = 0; i < numSprings3; ++i){
			float springLength = (currentBallsCloth3[springArray3[i].ball1].position - currentBallsCloth3[springArray3[i].ball2].position).GetLength();
			float stretch = springLength - springArray3[i].naturalLength;
			float tension = springArray3[i].springConstant * stretch / springArray3[i].naturalLength;

			springArray3[i].tension = tension;
		}

		calcSwapArray(numBalls, nextBallsCloth1, currentBallsCloth1, numSprings, springArray, timePassedInSeconds);
		calcSwapArray(numBalls2, nextBallsCloth2, currentBallsCloth2, numSprings2, springArray2, timePassedInSeconds);
		calcSwapArray(numBalls3, nextBallsCloth3, currentBallsCloth3, numSprings3, springArray3, timePassedInSeconds);

		BALL * temporaryBalls = currentBallsCloth1;
		currentBallsCloth1 = nextBallsCloth1;
		nextBallsCloth1 = currentBallsCloth1;

		temporaryBalls = currentBallsCloth2;
		currentBallsCloth2 = nextBallsCloth2;
		nextBallsCloth2 = currentBallsCloth2;

		temporaryBalls = currentBallsCloth3;
		currentBallsCloth3 = nextBallsCloth3;
		nextBallsCloth3 = currentBallsCloth3;
	}

	if(updateMade)
	{

		for(int i = 0; i < numBalls; ++i)
		{
			currentBallsCloth1[i].normal.LoadZero();
		}

		for(int i = 0; i < clothSize - 1; ++i)
		{
			for(int j = 0; j < clothSize-1; ++j)
			{
				//TODO add force to triangles
				VECTOR3D & p0=currentBallsCloth1[i*clothSize+j].position;
				VECTOR3D & p1=currentBallsCloth1[i*clothSize+j+1].position;
				VECTOR3D & p2=currentBallsCloth1[(i+1)*clothSize+j].position;
				VECTOR3D & p3=currentBallsCloth1[(i+1)*clothSize+j+1].position;

				VECTOR3D & n0=currentBallsCloth1[i*clothSize+j].normal;
				VECTOR3D & n1=currentBallsCloth1[i*clothSize+j+1].normal;
				VECTOR3D & n2=currentBallsCloth1[(i+1)*clothSize+j].normal;
				VECTOR3D & n3=currentBallsCloth1[(i+1)*clothSize+j+1].normal;

				//Calculate the normals for the 2 triangles and add on
				VECTOR3D normal=(p1-p0).CrossProduct(p2-p0);

				n0+=normal;
				n1+=normal;
				n2+=normal;

				normal=(p1-p2).CrossProduct(p3-p2);

				n1+=normal;
				n2+=normal;
				n3+=normal;
			}
		}
		//Normalize normals
		for(int i=0; i<numBalls; ++i)
			currentBallsCloth1[i].normal.Normalize();
	}

	bool useShadowStuff = false;
	if(useShadowStuff){
		//shadow map//////////////////////////////////////////////////////////////////////////////
		shadowMapFirstPass();
		//Render(currentTime, timePassed);
		DrawScene();
		shadowMapSecondPass();
		//Render(currentTime, timePassed);
		DrawScene();
		shadowMapThirdPass();
		//Render(currentTime, timePassed);
		DrawScene();

		//Disable textures and texgen
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glDisable(GL_TEXTURE_GEN_R);
		glDisable(GL_TEXTURE_GEN_Q);

		//Restore other states
		glDisable(GL_LIGHTING);
		glDisable(GL_ALPHA_TEST);

		glFinish();
		glutSwapBuffers();
		glutPostRedisplay();
		//////////////////////////////////////////////////////////////////////////////////////////
	}
	else{
		//normal///////////////////////////
		glutPostRedisplay();
		Render(currentTime, timePassed);
		///////////////////////////////////
	}

	//printf("\nPrinting");
}
void keyboard(unsigned char key, int x, int y) {

	if (key == '1'){
		for(int i = 0; i < clothSize-1; i++) currentBallsCloth1[i].fixed=false;
		for(int i = 0; i < cloth2Size-1; i++) currentBallsCloth2[i].fixed=false;
		for(int i = 0; i < cloth3Size-1; i++) currentBallsCloth3[i].fixed=false;
	}
	if (key == '2'){
		for(int i = 0; i < clothSize-1; i++) currentBallsCloth1[i*clothSize + clothSize-1].fixed = false;
		for(int i = 0; i < cloth2Size-1; i++) currentBallsCloth2[i*cloth2Size + cloth2Size-1].fixed = false;
		for(int i = 0; i < cloth3Size-1; i++) currentBallsCloth3[i*cloth3Size + cloth3Size-1].fixed = false;
//		currentBallsCloth1[clothSize-1].fixed=false;
//		currentBallsCloth2[cloth2Size-1].fixed=false;
//		currentBallsCloth3[cloth3Size-1].fixed=false;
	}
	if (key == '3'){
		currentBallsCloth1[(clothSize-1)*(clothSize-1)].fixed=false;
		currentBallsCloth2[(cloth2Size-1)*(cloth2Size-1)].fixed=false;
		currentBallsCloth3[(cloth3Size-1)*(cloth3Size-1)].fixed=false;
	}
	if (key == '4'){
		currentBallsCloth1[clothSize*clothSize-1].fixed=false;
		currentBallsCloth2[cloth2Size*cloth2Size-1].fixed=false;
		currentBallsCloth3[cloth3Size*cloth3Size-1].fixed=false;
	}
	if (key == ' '){
		ResetCloth1();
		ResetCloth2();
		ResetCloth3();
	}
	if (key == 'j'){
		spherePosition.x -= .15;
	}
	if (key == 'k'){
		spherePosition.x += .15;
	}
	if (key == 's'){
		drawSprings = !drawSprings;
	}
	if (key == 'b'){
		drawBalls = !drawBalls;
	}
	if(key == 'm'){
		windStrength += 0.05f;
		if(windStrength > 1.4f) windStrength = 1.4f;
	}
	if(key == 'n'){
		windStrength -= 0.05f;
		if(windStrength < -1.4f) windStrength = -1.4f;
	}
	if (key == '['){
		cubePosition.x -= .15;
	}
	if (key == ']'){
		cubePosition.x += .15;
	}
	if (key == '='){
		cubePosition.y += .15;
	}
	if (key == '\''){
		cubePosition.y -= .15;
	}
	if (key == '-'){
		cubePosition.z += .15;
	}
	if (key == '0'){
		cubePosition.z -= .15;
	}
	glutPostRedisplay();

}

void ReadTexture(const char* filename, GLuint* tex, TextureInfo* info) {
	//Your code here
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);
	glEnable(GL_DEPTH_TEST);
	unsigned int i;
	for (i = 0; i < strlen(filename); i++) {
		if (filename[i] == '.') {
			break;
		}
	}

	//printf("Before: %s", t.filename);
	char extension[5];
	strcpy(extension, &filename[i + 1]);
	//printf(extension);
	if (strcmp(extension, "jpg") == 0 || strcmp(extension, "jpeg") == 0)
		loadTextureFromJPEG(const_cast<char*>(filename), info);
	else if (strcmp(extension, "png") == 0)
		loadTextureFromPNG(const_cast<char*>(filename), info);
	else {
		printf("Invalid format. Only supports JPEG and PNG.\n");
		exit(1);
	}

	//Init the texture storage, and set some parameters.
	//(I high recommend reading up on these commands)
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, tex);
	glBindTexture(GL_TEXTURE_2D, *tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//Only useful for PNG files, since JPEG doesn't support alpha
	if (info->hasAlpha) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, info->width, info->height, 0, GL_RGBA,
				GL_UNSIGNED_BYTE, info->textureData);
	} else {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, info->width, info->height, 0, GL_RGB,
				GL_UNSIGNED_BYTE, info->textureData);
	}
	//Once the texture has been loaded by GL, we don't need this anymore.
	free(info->textureData);
}

int main( int argc, char** argv)
{
	glutInit(&argc,argv);

	VariablesInit();

	//Note the addition of GLUT_ALPHA to the display mode flags
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_ALPHA | GLUT_DEPTH);
	glutInitWindowSize(1000, 800);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Cloth Simulation");

	glutDisplayFunc(Physics);
	glutKeyboardFunc(keyboard);

	// load cloth texture
	ReadTexture("carpetDom.png", &clothTex, &clothTexInfo);
	ReadTexture("cat.png", &cloth2Tex, &cloth2TexInfo);
	ReadTexture("rug.png", &cloth3Tex, &cloth3TexInfo);
	ReadTexture("wood.jpg", &woodTex, &woodTexInfo);

	glutMainLoop();

	return 0;
}
