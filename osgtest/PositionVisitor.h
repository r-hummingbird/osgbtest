#pragma once
#include "stdafx.h"
#include <osg/NodeVisitor>
#include <vector>
#include "ModelAttributeFunctor.h"
#include <osg/TriangleIndexFunctor>
#include "TriangleIndex.h"
#include "Geom.h"
#include <osgText/Text>
#include<osg/LineWidth>
//�ڵ�����������ڵ㣬��ȡģ�͵ĸ������ݼ̳���osg::NodeVisitor
using namespace std;
class PositionVisitor
	:public osg::NodeVisitor
{
protected:
	vector<Geom*> allGeom;//���е�geom
	osg::Vec4 geomColor;//geom����ɫ
	string modelName;//ģ������
	int textSize;//��ʾ���ֵĴ�С
	osg::BoundingBox boundingBox;//��Χ��
public:
	virtual  void apply(osg::Geode& node) override;
	void dealTriangleInfo(ModelAttributeFunctor attributeFunctor, osg::TriangleIndexFunctor<TriangleIndex> indexFunctor);//����������õ�����Ϣ�����������ι�ϵ
	osg::ref_ptr<osg::Node> createOsgNode(osg::Vec4 color, int order);//����ָ������ɫ����geom�е����ݴ�����Geode
	osg::ref_ptr<osg::Node> createRandomColorOsgNode(int order);//��geom�е����ݴ�����osg�ڵ㣬��ɫ���
	osg::ref_ptr<osgText::Text> createTipText(short direction);//������ʾ����
	//osg::ref_ptr<osgText::Text> createCenterText(osg::BoundingBox);//����geom�����ĵ�
	PositionVisitor(string ModelName);
	PositionVisitor(string ModelName, int TextSize);
	vector<Geom*> getAllGeom();
	~PositionVisitor();
};
