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
	vector<Vertex*> vertices;//һ��geom�����еĶ�����Ϣ
	vector<Triangle*> triangles;//һ��geom�е�������������Ϣ
	osg::BoundingBox  boundingBox;//��Χ��
	osg::ref_ptr<osg::Geode> createOsgNode(osg::Vec4 color);//�����µ�osg��㣬���õ��������ô�ɫ���»��Ƴ���������һ��osg��Geod�ڵ�ĺ���
	osg::ref_ptr<osg::Geode> Geom::createOsgNode_Point(osg::Vec4 color);//�����µ�osg��㣬���õ��������ô�ɫ���»��Ƴ��㲢����һ��osg��Geod�ڵ�ĺ���
	osg::ref_ptr<osg::Geode> Geom::createOsgNode_Triangle(osg::Vec4 color);//�����µ�osg��㣬���õ��������ô�ɫ���»��Ƴ�������������һ��osg��Geod�ڵ�ĺ���
	bool isTwoTriangleNeighbor(int triangle1Index, int triangle2Index);//�����������Ƿ�����
	void createTriangleTopo();//����������֮������˹�ϵ
	void createVertexTopo();//��������֮�������
	Geom();
	~Geom();
};

