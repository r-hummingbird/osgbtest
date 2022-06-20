#include "stdafx.h"
#include "Geom.h"
#include <iostream>
#include <queue>
#include "Utility.h"
#include <osg/Geometry>
#include "Triangle.h"
using namespace std;
//定义Geom类存储模型中的部件
Geom::Geom()
{
}

Geom::~Geom()
{
	for (Vertex* vertex : vertices)
		delete vertex;
	for (Triangle* triangle : triangles)
		delete triangle;
}

//两个三角形是否相邻
bool Geom::isTwoTriangleNeighbor(int triangle1Index, int triangle2Index)
{
	Triangle* triangle1 = triangles.at(triangle1Index);
	Triangle* triangle2 = triangles.at(triangle2Index);

	osg::Vec3 pnt11 = vertices.at(triangle1->vertexIndexs[0])->coor;
	osg::Vec3 pnt12 = vertices.at(triangle1->vertexIndexs[1])->coor;
	osg::Vec3 pnt13 = vertices.at(triangle1->vertexIndexs[2])->coor;

	osg::Vec3 pnt21 = vertices.at(triangle2->vertexIndexs[0])->coor;
	osg::Vec3 pnt22 = vertices.at(triangle2->vertexIndexs[1])->coor;
	osg::Vec3 pnt23 = vertices.at(triangle2->vertexIndexs[2])->coor;

	if ((Utility::isVec3Same(pnt11, pnt21) && Utility::isVec3Same(pnt12, pnt22))//第一条边
		|| (Utility::isVec3Same(pnt11, pnt22) && Utility::isVec3Same(pnt12, pnt21))
		|| (Utility::isVec3Same(pnt11, pnt22) && Utility::isVec3Same(pnt12, pnt23))//第二条边
		|| (Utility::isVec3Same(pnt11, pnt23) && Utility::isVec3Same(pnt12, pnt22))
		|| (Utility::isVec3Same(pnt11, pnt21) && Utility::isVec3Same(pnt12, pnt23))//第三条边
		|| (Utility::isVec3Same(pnt11, pnt23) && Utility::isVec3Same(pnt12, pnt21))

		|| (Utility::isVec3Same(pnt12, pnt21) && Utility::isVec3Same(pnt13, pnt22))//第一条边
		|| (Utility::isVec3Same(pnt12, pnt22) && Utility::isVec3Same(pnt13, pnt21))
		|| (Utility::isVec3Same(pnt12, pnt22) && Utility::isVec3Same(pnt13, pnt23))//第二条边
		|| (Utility::isVec3Same(pnt12, pnt23) && Utility::isVec3Same(pnt13, pnt22))
		|| (Utility::isVec3Same(pnt12, pnt21) && Utility::isVec3Same(pnt13, pnt23))//第三条边
		|| (Utility::isVec3Same(pnt12, pnt23) && Utility::isVec3Same(pnt13, pnt21))

		|| (Utility::isVec3Same(pnt11, pnt21) && Utility::isVec3Same(pnt13, pnt22))//第一条边
		|| (Utility::isVec3Same(pnt11, pnt22) && Utility::isVec3Same(pnt13, pnt21))
		|| (Utility::isVec3Same(pnt11, pnt22) && Utility::isVec3Same(pnt13, pnt23))//第二条边
		|| (Utility::isVec3Same(pnt11, pnt23) && Utility::isVec3Same(pnt13, pnt22))
		|| (Utility::isVec3Same(pnt11, pnt21) && Utility::isVec3Same(pnt13, pnt23))//第三条边
		|| (Utility::isVec3Same(pnt11, pnt23) && Utility::isVec3Same(pnt13, pnt21)))
		return true;
	return  false;
}
//模型三角形之间的拓扑
void Geom::createTriangleTopo()
{
	cout << "开始创建三角形之间的拓扑关系：" << endl;
	for (size_t i = 0; i<triangles.size(); ++i)
	{
		Triangle* trianglei = triangles.at(i);
		for (size_t j = i + 1; j<triangles.size(); ++j)
		{
			Triangle* trianglej = triangles.at(j);
			if (isTwoTriangleNeighbor(i, j))
			{
				trianglei->neighborTriangles.push_back(j);
				trianglej->neighborTriangles.push_back(i);
			}
		}
		cout << "\t当前进度" << int(i*100.0 / triangles.size()) << "%\r";
	}
	cout << endl;
}
//创建顶点之间的拓扑
void Geom::createVertexTopo()
{
	//点周围的三角形
	for (size_t i = 0; i<triangles.size(); ++i)
	{
		Vertex *vertex1 = (Vertex*)vertices.at(triangles.at(i)->vertexIndexs[0]);
		vertex1->neighborTriangle.push_back(i);
		Vertex *vertex2 = (Vertex*)vertices.at(triangles.at(i)->vertexIndexs[1]);
		vertex2->neighborTriangle.push_back(i);
		Vertex *vertex3 = (Vertex*)vertices.at(triangles.at(i)->vertexIndexs[2]);
		vertex3->neighborTriangle.push_back(i);
		cout << "	点周围的三角形：" << int(i*1.0 / triangles.size() * 100) << "%\r";
	}
}

//将Geom中的数据创建成osg节
osg::ref_ptr<osg::Geode> Geom::createOsgNode(osg::Vec4 color)
{
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
	//顶点、法向量
	osg::ref_ptr<osg::Vec3Array> vertexArray = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec3Array> normalArray = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec4Array> colorArray = new osg::Vec4Array;
	osg::ref_ptr<osg::Vec2Array> textCoorArray = new osg::Vec2Array;

	//索引
	osg::ref_ptr<osg::DrawElementsUInt> indexs = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
	for (Triangle* triangle : triangles)
	{
		indexs->push_back(triangle->vertexIndexs[0]);
		indexs->push_back(triangle->vertexIndexs[1]);
		indexs->push_back(triangle->vertexIndexs[2]);
		//顶点的法向量数据全是0，由于一个三角形是2d的图形，所以我直接让一个三角形的三个顶点的法向量数据等于三角形的法向量
		vertices[triangle->vertexIndexs[0]]->normal = triangle->normal;
		vertices[triangle->vertexIndexs[1]]->normal = triangle->normal;
		vertices[triangle->vertexIndexs[2]]->normal = triangle->normal;
	}

	for (Vertex* vertex : vertices)
	{
		vertexArray->push_back(vertex->coor);
		normalArray->push_back(vertex->normal);
		textCoorArray->push_back(vertex->texCoor);
	}
	//颜色
	colorArray->push_back(color);

	//cout << "triangles:" << triangles.size() << endl;
	//cout << "vertices:" << vertices.size() << endl;
	geometry->setVertexArray(vertexArray);
	geometry->setNormalArray(normalArray, osg::Array::BIND_PER_VERTEX);
	geometry->setColorArray(colorArray, osg::Array::BIND_OVERALL);
	geometry->setColorBinding(osg::Geometry::BIND_OVERALL);
	geometry->addPrimitiveSet(indexs);
	geometry->setTexCoordArray(0, textCoorArray);
	geode->addDrawable(geometry);
	return geode;
}


osg::ref_ptr<osg::Geode> Geom::createOsgNode_Point(osg::Vec4 color) {
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;

	osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
	osg::ref_ptr<osg::Vec3Array> vertexArray = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec3Array> normalArray = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec4Array> colorArray = new osg::Vec4Array;


	//顶点的法向量数据全是0，由于一个三角形是2d的图形，所以我直接让一个三角形的三个顶点的法向量数据等于三角形的法向量
	for (Triangle* triangle : triangles)
	{
		vertices[triangle->vertexIndexs[0]]->normal = triangle->normal;
		vertices[triangle->vertexIndexs[1]]->normal = triangle->normal;
		vertices[triangle->vertexIndexs[2]]->normal = triangle->normal;
	}

	for (Vertex* vertex : vertices)
	{
		vertexArray->push_back(vertex->coor);
		normalArray->push_back(vertex->normal);

	}
	//颜色
	colorArray->push_back(color);

	//cout << "triangles:" << triangles.size() << endl;
	//cout << "vertices:" << vertices.size() << endl;
	geometry->setVertexArray(vertexArray);
	geometry->setNormalArray(normalArray, osg::Array::BIND_PER_VERTEX);
	geometry->setColorArray(colorArray, osg::Array::BIND_OVERALL);
	geometry->setColorBinding(osg::Geometry::BIND_OVERALL);

	//将geom显示成点
	osg::ref_ptr<osg::PrimitiveSet> primitiveSet = new osg::DrawArrays(osg::PrimitiveSet::POINTS, 0, vertices.size());
	geometry->addPrimitiveSet(primitiveSet);
	osg::StateSet* stateSet = geometry->getOrCreateStateSet();
	osg::Point* pointSize = new osg::Point;
	pointSize->setSize(4.0);
	stateSet->setAttribute(pointSize);
	geometry->addPrimitiveSet(primitiveSet);
	geode->addDrawable(geometry);
	return geode;
}


osg::ref_ptr<osg::Geode> Geom::createOsgNode_Triangle(osg::Vec4 color)
{

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	list<osg::ref_ptr<osg::Geometry>> geometry_list;
	list<osg::ref_ptr<osg::Vec3Array>> vertexArray_list;
	list<osg::ref_ptr<osg::Vec3Array>> normalArray_list;
	list<osg::ref_ptr<osg::Vec4Array>> colorArray_list;



	//每一个三角形都是一个geometry 针对于每一个三角形画出三个边
	for (int i = 0; i<triangles.size(); i++)
	{
		geometry_list.push_back(new osg::Geometry);
		vertexArray_list.push_back(new osg::Vec3Array);
		normalArray_list.push_back(new osg::Vec3Array);
		colorArray_list.push_back(new osg::Vec4Array);

		//顶点的法向量数据全是0，由于一个三角形是2d的图形，所以我直接让一个三角形的三个顶点的法向量数据等于三角形的法向量
		vertices[triangles[i]->vertexIndexs[0]]->normal = triangles[i]->normal;
		vertices[triangles[i]->vertexIndexs[1]]->normal = triangles[i]->normal;
		vertices[triangles[i]->vertexIndexs[2]]->normal = triangles[i]->normal;

		vertexArray_list.back()->push_back(vertices[triangles[i]->vertexIndexs[0]]->coor);
		vertexArray_list.back()->push_back(vertices[triangles[i]->vertexIndexs[1]]->coor);
		vertexArray_list.back()->push_back(vertices[triangles[i]->vertexIndexs[2]]->coor);
		normalArray_list.back()->push_back(vertices[triangles[i]->vertexIndexs[0]]->normal);
		normalArray_list.back()->push_back(vertices[triangles[i]->vertexIndexs[1]]->normal);
		normalArray_list.back()->push_back(vertices[triangles[i]->vertexIndexs[2]]->normal);

		colorArray_list.back()->push_back(color);
		geometry_list.back()->setVertexArray(vertexArray_list.back());
		geometry_list.back()->setNormalArray(normalArray_list.back(), osg::Array::BIND_PER_VERTEX);
		geometry_list.back()->setColorArray(colorArray_list.back(), osg::Array::BIND_OVERALL);
		geometry_list.back()->setColorBinding(osg::Geometry::BIND_OVERALL);
		osg::ref_ptr<osg::PrimitiveSet> primitiveSet = new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP, 0, 3);
		geometry_list.back()->addPrimitiveSet(primitiveSet);
		geode->addDrawable(geometry_list.back());


	}
	return geode;
}
