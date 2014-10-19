
#include "VECTOR3D.h"

#ifndef BALL_H
#define BALL_H


class BALL
{
public:
	VECTOR3D position;
	VECTOR3D velocity;

	float mass;

	//Is this ball held in position?
	bool fixed;

	//Vertex normal for this ball
	VECTOR3D normal;
};


#endif	//BALL_H
