#pragma once
#include "stdafx.h"
#include <osg/ref_ptr>
#include <osg/Array>
#include <osg/TriangleIndexFunctor>

class TriangleIndex
{
public:
	osg::ref_ptr<osg::UIntArray> indexs;
	int triangleNum;
	TriangleIndex();
	~TriangleIndex();
	void operator()(const unsigned int& v1, const unsigned int& v2, const unsigned int& v3);
};
