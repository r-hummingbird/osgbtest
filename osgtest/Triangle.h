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
	int index;//该三角形在数组中的索引
	std::vector<int> neighborTriangles;//相邻的三角形的索引
};