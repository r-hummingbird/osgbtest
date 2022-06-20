#pragma once
#include "stdafx.h"
#include <osg/Drawable>

class ModelAttributeFunctor
	:public osg::Drawable::AttributeFunctor
{
public:
	osg::ref_ptr<osg::Vec3Array> vertexList;//�洢���������
	osg::ref_ptr<osg::Vec3Array> normalList;//�洢������
	osg::ref_ptr<osg::Vec2Array> textCoordList;//��������
	virtual void apply(osg::Drawable::AttributeType, unsigned, osg::Vec2*) override;
	virtual void apply(osg::Drawable::AttributeType, unsigned, osg::Vec3*) override;
	ModelAttributeFunctor();
	~ModelAttributeFunctor();
};