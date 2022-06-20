#include "stdafx.h"
#include "ModelAttributeFunctor.h"
#include <iostream>
using namespace std;
//类ModelAttributeFunctor继承自osg::Drawable::AttributeFunctor来获取顶点属性

//获取顶点坐标位置和纹理坐标
//顶点位置是osg::Vec3类型
//纹理坐标是osg::Vec2类型
ModelAttributeFunctor::ModelAttributeFunctor()
{
	vertexList = new osg::Vec3Array;
	normalList = new osg::Vec3Array;
	textCoordList = new osg::Vec2Array;
}


ModelAttributeFunctor::~ModelAttributeFunctor()
{
}

void ModelAttributeFunctor::apply(osg::Drawable::AttributeType type, unsigned size, osg::Vec2* front)
{
	if (type == osg::Drawable::TEXTURE_COORDS_0)
	{
		//cout << "textcoor" << endl;
		for (unsigned i = 0; i<size; i++)
		{
			textCoordList->push_back(*(front + i));
		}
	}
}

void ModelAttributeFunctor::apply(osg::Drawable::AttributeType type, unsigned size, osg::Vec3* front)
{
	if (type == osg::Drawable::VERTICES)
	{
		//cout << "vertice" << endl;
		for (unsigned i = 0; i<size; i++)
		{
			vertexList->push_back(*(front + i));
		}
	}
	else if (type == osg::Drawable::NORMALS)
	{
		//cout << "normals" << endl;
		for (unsigned i = 0; i<size; i++)
		{
			normalList->push_back(*(front + i));
		}
	}
}
