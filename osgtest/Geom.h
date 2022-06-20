#pragma once
#include <vector>
#include "Vertex.h"
#include "Triangle.h"
#include <osg/ref_ptr>
#include <osg/Geode>
#include <osg/LineWidth>
#include <osg/Point>
using namespace std;


class Geom
{
public:
	vector<Vertex*> vertices;//一个geom中所有的顶点信息
	vector<Triangle*> triangles;//一个geom中的所有三角形信息
	osg::BoundingBox  boundingBox;//包围盒
	osg::ref_ptr<osg::Geode> createOsgNode(osg::Vec4 color);//创建新的osg结点，将得到的数据用纯色重新绘制出来并返回一个osg的Geod节点的函数
	osg::ref_ptr<osg::Geode> Geom::createOsgNode_Point(osg::Vec4 color);//创建新的osg结点，将得到的数据用纯色重新绘制出点并返回一个osg的Geod节点的函数
	osg::ref_ptr<osg::Geode> Geom::createOsgNode_Triangle(osg::Vec4 color);//创建新的osg结点，将得到的数据用纯色重新绘制出三角网并返回一个osg的Geod节点的函数
	bool isTwoTriangleNeighbor(int triangle1Index, int triangle2Index);//两个三角形是否相邻
	void createTriangleTopo();//创建三角形之间的拓扑关系
	void createVertexTopo();//创建顶点之间的拓扑
	Geom();
	~Geom();
};

