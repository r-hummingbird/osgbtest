#pragma once
#include "stdafx.h"
#include <osg/Vec3>
#include <string>
#include <osgText/Text>
using namespace std;

class Utility
{
public:
	static bool isVec3Same(osg::Vec3 v1, osg::Vec3 v2);//�Ƚ�������ά�����Ƿ����
	static string getFileNameFromPath(string path);//��ģ��·���л�ȡ������
	static void string_replace(std::string &strBig, const std::string &strsrc, const std::string &strdst);
	static osg::ref_ptr<osgText::Text> createText(string info, int textSize, osg::ref_ptr<osgText::Font>font, osg::Vec3 position, int direction, osg::Vec4 textColor);
	static osg::ref_ptr<osg::Node> createCoorAxis(int textSize);
	Utility();
	~Utility();
};