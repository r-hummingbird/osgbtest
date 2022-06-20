#include "stdafx.h"
#include "Triangle.h"
Triangle::Triangle()
{
	init();
}


Triangle::~Triangle()
{
}

void Triangle::init()
{
	this->vertexIndexs[0] = this->vertexIndexs[1] = this->vertexIndexs[2] = -1;
	this->normal.set(0.0f, 0.0f, 0.0f);
}