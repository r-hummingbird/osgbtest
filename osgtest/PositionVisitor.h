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
//节点访问器遍历节点，获取模型的各类数据继承自osg::NodeVisitor
using namespace std;
class PositionVisitor
	:public osg::NodeVisitor
{
protected:
	vector<Geom*> allGeom;//所有的geom
	osg::Vec4 geomColor;//geom的颜色
	string modelName;//模型名称
	int textSize;//提示文字的大小
	osg::BoundingBox boundingBox;//包围盒
public:
	virtual  void apply(osg::Geode& node) override;
	void dealTriangleInfo(ModelAttributeFunctor attributeFunctor, osg::TriangleIndexFunctor<TriangleIndex> indexFunctor);//处理访问器得到的信息，构建三角形关系
	osg::ref_ptr<osg::Node> createOsgNode(osg::Vec4 color, int order);//根据指定的颜色，将geom中的数据创建成Geode
	osg::ref_ptr<osg::Node> createRandomColorOsgNode(int order);//将geom中的数据创建成osg节点，颜色随机
	osg::ref_ptr<osgText::Text> createTipText(short direction);//创建提示文字
	//osg::ref_ptr<osgText::Text> createCenterText(osg::BoundingBox);//创建geom的中心点
	PositionVisitor(string ModelName);
	PositionVisitor(string ModelName, int TextSize);
	vector<Geom*> getAllGeom();
	~PositionVisitor();
};
