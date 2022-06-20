#pragma once
#include "stdafx.h"
#include <osg/Vec3>
#include <vector>
#include <osg/Vec4>

class Triangle
{
public:
	void init();
	Triangle();
	~Triangle();
	int vertexIndexs[3];
	osg::Vec3 normal;
	int index;//���������������е�����
	std::vector<int> neighborTriangles;//���ڵ������ε�����
};