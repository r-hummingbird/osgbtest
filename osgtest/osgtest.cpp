#include "stdafx.h"
#include <osgViewer/Viewer>
#include <osgDB/ReadFile>
#include <osgGA/StateSetManipulator>
#include <osgViewer/ViewerEventHandlers>//事件响应类，对渲染状态进行控制
#include <osgUtil/Simplifier> //简化几何体
#include <vector>
#include <regex>
#include <windows.h>
#include "PositionVisitor.h"
#include "Utility.h"
#include "OBBbox.h"
#include "osgDB/WriteFile"
#include<osg/Geometry>
#include<osg/Geode>
#include <osg/Node>
#include <osg/Geode>
#include <osg/Group>
#include<osg/DrawPixels>
#include<osg/PositionAttitudeTransform>//位置变换节点类，提供模型的位置变换、大小缩放、原点位置的设置、坐标系的变换  
#include<osg/computeBoundsVisitor>
#include <osgViewer/ViewerEventHandlers> //事件监听  
#include<osg/MatrixTransform>
#include<osg/ShapeDrawable>
#include<osg/Shape>
#include<osg/PolygonMode>
#include <osg/Light>
#include <osg/LightSource>
#include <osg/LineWidth>
#include <osg/BoundingSphere>
#include <osg/ComputeBoundsVisitor>
#include <osg/BoundingBox>
#include <osg/Material>
#include <osgUtil/Optimizer>
#include <string>
#include <iostream> 
#include <QDir>


using namespace std;

//查找文件夹下所有的文件
static  void  findDir(string dir, vector < string > & subDirs)
{
	subDirs.clear();
	QDir fromDir(QString::fromLocal8Bit(dir.c_str()));
	QStringList filters;
	QFileInfoList fileInfoList = fromDir.entryInfoList(filters, QDir::AllDirs | QDir::Files);
	foreach(QFileInfo fileInfo, fileInfoList)
	{
		if (fileInfo.fileName() == "." || fileInfo.fileName() == "..")
		{
			continue;
		}

		if (fileInfo.isDir())
		{
			QByteArray dir = fileInfo.filePath().toLocal8Bit();
			subDirs.push_back(dir.data());
		}
	}
}

//文件路径下面的文件名
static std::string DirOrPathGetName(std::string filePath)
{
	size_t m = filePath.find_last_of('/');
	if (m == string::npos)
	{
		return filePath;
	}

	size_t p = filePath.find_last_of('.');
	if (p != string::npos && p > m) //没有点号
	{
		filePath.erase(p);
	}

	std::string dirPath = filePath;
	dirPath.erase(0, m + 1);
	return dirPath;
}
//创造包围盒线框
osg::ref_ptr<osg::Geode> createBoundingBox(osg::Vec3 center, float lenx, float leny, float lenz)
{
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;

	osg::ref_ptr<osg::ShapeDrawable>  drawable = new osg::ShapeDrawable(new osg::Box(center, lenx, leny, lenz));
	drawable->setColor(osg::Vec4(0.0, 1.0, 1.0, 1.0));
	osg::ref_ptr<osg::StateSet> stateset = drawable->getOrCreateStateSet();
	osg::ref_ptr<osg::PolygonMode> polygon = new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
	stateset->setAttributeAndModes(polygon);

	osg::ref_ptr<osg::LineWidth> linewidth = new osg::LineWidth(1.5);
	stateset->setAttribute(linewidth);
	geode->addDrawable(drawable);
	return geode;
}

//创建OBB包围盒
osg::ref_ptr<osg::Geode> createOBBBox(render_system::OBB obb)
{
	osg::ref_ptr<osg::Vec3Array> point;
	obb.getCorners(point);
	vector<osg::Vec3> obbPnt = point->asVector();
	osg::ref_ptr<osg::Geode> box = new osg::Geode;
	osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
	osg::ref_ptr<osg::Vec3Array> vertexes = new osg::Vec3Array;
	for (int i = 0; i < 3; i++)
		for (size_t j = 0; j < obbPnt.size(); j++)
			vertexes->push_back(obbPnt[j]);
	geometry->setVertexArray(vertexes);

	GLuint elements[] = {
		//front
		0,1,2,
		0,2,3,
		//back
		6,4,7,
		6,5,4,
		//left
		8,9,14,
		8,14,15,
		//right
		11,12,13,
		11,13,10,
		//top
		16,20,19,
		16,23,20,
		//bottom
		17,22,21,
		17,21,18
	};

	osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
	for (int i = 0; i < 4; i++)
		normals->push_back(obb._zAxis);
	for (int i = 0; i < 4; i++)
		normals->push_back(-obb._zAxis);

	normals->push_back(-obb._xAxis);
	normals->push_back(-obb._xAxis);
	for (int i = 0; i<4; i++)
		normals->push_back(obb._xAxis);
	normals->push_back(-obb._xAxis);
	normals->push_back(-obb._xAxis);

	normals->push_back(obb._yAxis);
	normals->push_back(-obb._yAxis);
	normals->push_back(-obb._yAxis);
	normals->push_back(obb._yAxis);
	normals->push_back(obb._yAxis);
	normals->push_back(-obb._yAxis);
	normals->push_back(-obb._yAxis);
	normals->push_back(obb._yAxis);

	geometry->setNormalArray(normals, osg::Array::BIND_PER_VERTEX);
	osg::ref_ptr<osg::Vec4Array> colorArray = new osg::Vec4Array;
	colorArray->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
	geometry->setColorArray(colorArray, osg::Array::BIND_OVERALL);
	osg::ref_ptr<osg::StateSet> stateset = geometry->getOrCreateStateSet();
	osg::ref_ptr<osg::PolygonMode> polygon = new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
	stateset->setAttributeAndModes(polygon);
	osg::ref_ptr<osg::LineWidth> linewidth = new osg::LineWidth(1.5);
	stateset->setAttribute(linewidth);
	int pntNum = sizeof(elements) / sizeof(GLuint);
	osg::ref_ptr<osg::DrawElementsUInt> elementsArray = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, pntNum, elements);
	geometry->addPrimitiveSet(elementsArray);
	box->addDrawable(geometry);
	return box;
}
//向场景中添加光源
osg::ref_ptr<osg::Group> createLight(osg::ref_ptr<osg::Node> node)
{
	osg::ref_ptr<osg::Group> lightRoot = new osg::Group();
	lightRoot->addChild(node);

	//开启光照
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet();
	stateset = lightRoot->getOrCreateStateSet();
	stateset->setMode(GL_LIGHTING, osg::StateAttribute::ON);
	stateset->setMode(GL_LIGHT0, osg::StateAttribute::ON);

	//计算包围盒
	osg::BoundingSphere bs;
	node->computeBound();
	bs = node->getBound();

	//创建一个Light对象
	osg::ref_ptr<osg::Light> light = new osg::Light();
	light->setLightNum(0);
	//设置方向
	light->setDirection(osg::Vec3(0.0f, 0.0f, -1.0f));
	//设置位置
	light->setPosition(osg::Vec4(bs.center().x(), bs.center().y(), bs.center().z() + bs.radius(), 1.0f));
	//设置环境光的颜色
	light->setAmbient(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	//设置散射光的颜色
	light->setDiffuse(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));

	//设置恒衰减指数
	light->setConstantAttenuation(1.0f);
	//设置线形衰减指数
	light->setLinearAttenuation(0.0f);
	//设置二次方衰减指数
	light->setQuadraticAttenuation(0.0f);

	//创建光源
	osg::ref_ptr<osg::LightSource> lightSource = new osg::LightSource();
	lightSource->setLight(light.get());

	lightRoot->addChild(lightSource.get());

	return lightRoot.get();
}









class FindPagedlod : public osg::NodeVisitor
{
public:
	FindPagedlod(osg::Group* g = NULL)
		:osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
		, _g(g)
	{
	}
	~FindPagedlod() {}

	virtual void apply(osg::PagedLOD& lod)
	{
		if (_g.valid())
		{
			string databasePath = lod.getDatabasePath();
			osg::LOD::RangeList list = lod.getRangeList();
			for (unsigned i = 0; i < list.size(); i++)
			{
				string childname = lod.getFileName(i);
				if (!childname.empty())
				{
					osg::Node* node = osgDB::readNodeFile(databasePath + childname);
					if (node)
						_g->addChild(node);
				}
			}
		}

		traverse(lod);
	}

	virtual void apply(osg::Geode& node)
	{
		_geodeCount++;
	}

	int _geodeCount = 0;
	osg::ref_ptr<osg::Group> _g;
};

osg::BoundingBox getBoundBox(osg::Node* node)
{
	osg::ComputeBoundsVisitor cbv;
	node->accept(cbv);
	return cbv.getBoundingBox();
}

osg::Group* tile4(osg::Group* g)
{
	osg::ref_ptr<osg::Group> splitGroup = new osg::Group;

	{
		osg::BoundingBox box = getBoundBox(g);

		float w = box.xMax() - box.xMin();
		float h = box.yMax() - box.yMin();

		osg::BoundingBox box1 = box;
		osg::BoundingBox box2 = box;
		osg::BoundingBox box3 = box;
		osg::BoundingBox box4 = box;

		box1.xMax() = box2.xMax() = box.xMin() + w * 0.5;
		box1.yMax() = box2.yMin() = box.yMin() + h * 0.5;

		box3.xMin() = box4.xMin() = box.xMin() + w * 0.5;
		box3.yMax() = box4.yMin() = box.yMin() + h * 0.5;

		osg::ref_ptr<osg::Group> g1 = new osg::Group;
		osg::ref_ptr<osg::Group> g2 = new osg::Group;
		osg::ref_ptr<osg::Group> g3 = new osg::Group;
		osg::ref_ptr<osg::Group> g4 = new osg::Group;
		for (int i = 0; i < (int)g->getNumChildren(); i++)
		{
			osg::Node* child = g->getChild(i);
			if (box1.contains(child->getBound().center()))
				g1->addChild(child);
			else if (box2.contains(child->getBound().center()))
				g2->addChild(child);
			else if (box3.contains(child->getBound().center()))
				g3->addChild(child);
			else
				g4->addChild(child);
		}
		splitGroup->addChild(g1);
		splitGroup->addChild(g2);
		splitGroup->addChild(g3);
		splitGroup->addChild(g4);
	}

	return splitGroup.release();
}

osg::Group* tile2(osg::Group* g, int maxNode, bool shouldSplit)
{
	osg::ref_ptr<osg::Group> splitGroup = new osg::Group;

	FindPagedlod fpl;
	g->accept(fpl);

	if (g->getNumChildren() < maxNode && fpl._geodeCount < maxNode && !shouldSplit)
	{
		splitGroup->addChild(g);
	}
	else
	{
		shouldSplit = (fpl._geodeCount / maxNode >= 2) && (fpl._geodeCount / g->getNumChildren()) >= 4;

		osg::BoundingBox box =getBoundBox(g);

		float w = box.xMax() - box.xMin();
		float h = box.yMax() - box.yMin();

		bool splitH = w < h;

		osg::BoundingBox box1 = box;
		osg::BoundingBox box2 = box;
		if (splitH)
		{
			box1.yMax() = box2.yMin() = box.yMin() + h * 0.5;
		}
		else
		{
			box1.xMax() = box2.xMin() = box.xMin() + w * 0.5;
		}

		osg::ref_ptr<osg::Group> g1 = new osg::Group;
		osg::ref_ptr<osg::Group> g2 = new osg::Group;
		for (int i = 0; i < (int)g->getNumChildren(); i++)
		{
			osg::Node* child = g->getChild(i);
			if (box1.contains(child->getBound().center()))
				g1->addChild(child);
			else
				g2->addChild(child);
		}

		if (g1->getNumChildren() > maxNode || (shouldSplit && g1->getNumChildren() > 1))
		{
			g1 = tile2(g1, maxNode, shouldSplit);
			for (int i = 0; i < (int)g1->getNumChildren(); i++)
			{
				splitGroup->addChild(g1->getChild(i));
			}
		}
		else
		{
			splitGroup->addChild(g1);
		}
		if (g2->getNumChildren() > maxNode || (shouldSplit && g2->getNumChildren() > 1))
		{
			g2 = tile2(g2, maxNode, shouldSplit);
			for (int i = 0; i < (int)g2->getNumChildren(); i++)
			{
				splitGroup->addChild(g2->getChild(i));
			}
		}
		else
		{
			splitGroup->addChild(g2);
		}
	}

	return splitGroup.release();
}






















//
//从控制台读取模型的路径：所有模型的路径全是绝对路径，中间用逗号分隔
//
//vector<string> readAbsolutPath()
//{
//	vector<string> results;//存储所有要加载的模型的绝对路径
//	cout << "请输入模型的绝对路径（如果有多个模型，中间用空格隔开）" << endl;
//	string modelPaths;
//	getline(std::cin, modelPaths);
//	std::regex re{ split };
//	 调用 std::vector::vector (InputIterator first, InputIterator last,const allocator_type& alloc = allocator_type())
//	 构造函数,完成字符串分割
//	vector<string> splitResult = std::vector<std::string>{
//		std::sregex_token_iterator(modelPaths.begin(), modelPaths.end(), re, -1),std::sregex_token_iterator()
//	};
//	for (string modelName : splitResult)
//	{
//		cout << modelName << endl;
//		if (fileExists(modelName))
//			results.push_back(modelName);
//	}
//	return results;
//}

//
////显示osgb模型和包围网以及光源
//int main()
//{
//	srand((unsigned)time(NULL));
//	int textSize = 5;
//	osg::ref_ptr<osgViewer::Viewer> spViewer = new osgViewer::Viewer();
//	osg::ref_ptr<osg::Group> spGroup = new osg::Group();
//	osg::ref_ptr<osgDB::Options> options = new osgDB::Options("noRotation");
//	vector < string > subDirs;
//	string path = "E:/BaiduNetdiskDownload/12月爆品-极速倾斜摄影数据试用版本/TestData/Data/TileData/Tile_0";
//	findDir(path, subDirs);
//	for (size_t i = 0; i < subDirs.size(); i++) {
//		string name = DirOrPathGetName(subDirs[i]);
//		string path1 = subDirs[i] + "/" + name + ".osgb";
//		osg::ref_ptr<osg::Node> spNode = osgDB::readNodeFile(path1);
//		osg::ref_ptr<osg::PagedLOD> lod = new osg::PagedLOD();
//		auto bs = spNode->getBound();
//		auto c = bs.center();
//		auto r = bs.radius();
//		lod->setCenter(c);
//		lod->setRadius(r);
//		lod->setRangeMode(osg::LOD::RangeMode::PIXEL_SIZE_ON_SCREEN);
//		osg::ref_ptr<osg::Geode> geode = new osg::Geode;
//		geode->getOrCreateStateSet();
//		lod->addChild(geode.get());
//		std::string relativeFilePath = name + ".osgb";  //相对路径
//		lod->setFileName(0, "");
//		lod->setFileName(1, relativeFilePath);
//		lod->setRange(0, 0, 1.0);	//第一层不可见
//		lod->setRange(1, 1.0, FLT_MAX);
//		lod->setDatabasePath("");
//		spGroup->addChild(lod);
//
//		string modelName = Utility::getFileNameFromPath(name);
//		PositionVisitor visitor = PositionVisitor(modelName, textSize);
//		spNode->accept(visitor);
//		spGroup->addChild(createLight(spNode.get()));
//		osgUtil::Optimizer optimizer;
//		optimizer.optimize(spGroup.get());
//		spViewer->setSceneData(spGroup);
//		osg::ComputeBoundsVisitor boundsVist;
//		spNode->accept(boundsVist);
//
//		osg::BoundingBox box = boundsVist.getBoundingBox();
//		OSG_NOTIFY(osg::ALWAYS) << "bound info:" << std::endl;;
//		osg::notify(osg::ALWAYS) << "bouding box info " << name << std::endl;
//		osg::notify(osg::ALWAYS) << "center x:" << box.center().x() << std::endl;
//		osg::notify(osg::ALWAYS) << "center y:" << box.center().y() << std::endl;;
//		osg::notify(osg::ALWAYS) << "center z:" << box.center().z() << std::endl;;
//		osg::notify(osg::ALWAYS) << "x max:" << box.xMax() << std::endl;
//		osg::notify(osg::ALWAYS) << "x min:" << box.xMin() << std::endl;
//		osg::notify(osg::ALWAYS) << "y max:" << box.yMax() << std::endl;
//		osg::notify(osg::ALWAYS) << "y min:" << box.yMin() << std::endl;
//		osg::notify(osg::ALWAYS) << "z max:" << box.zMax() << std::endl;
//		osg::notify(osg::ALWAYS) << "z min:" << box.zMin() << std::endl;
//
//		//画个外框
//		osg::ref_ptr<osg::Geode>spGeodeBox = new osg::Geode();
//		spGeodeBox->addDrawable(new osg::ShapeDrawable(new osg::Box(box.center(), box.xMax() - box.xMin(), box.yMax() - box.yMin(), box.zMax() - box.zMin())));
//		osg::ref_ptr<osg::StateSet> spGeodeStateSet = spGeodeBox->getOrCreateStateSet();
//
//		//采用线框模式，以便能看见里面的模型
//		osg::ref_ptr<osg::PolygonMode> spGeodePolygon = new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
//		spGeodeStateSet->setAttribute(spGeodePolygon);
//		spGroup->addChild(spGeodeBox);
//		spViewer->addEventHandler(new osgGA::StateSetManipulator(spViewer->getCamera()->getOrCreateStateSet()));
//		spViewer->addEventHandler(new osgViewer::StatsHandler);
//
//	}
//	std::string outputLodFile = path + "/Index.osgb";
//	osgDB::writeNodeFile(*spGroup, outputLodFile);
//	spViewer->setUpViewInWindow(10, 10, 800, 600);
//	spViewer->realize();
//	spViewer->run();
//	return 0;
//
//}
//
//////显示osgb模型AABB包围盒三角网
////int main()
////{
////	srand((unsigned)time(NULL));
////	int textSize = 1;
////	while (true)
////	{
////		vector < string > subDirs;
////		string path = "G:/寻甸县羊街镇/Data";
////		findDir(path, subDirs);
////		osg::ref_ptr<osgViewer::Viewer> viewer=new osgViewer::Viewer();
////		osg::ref_ptr<osg::Group> root = new osg::Group;
////		osg::ref_ptr<osgDB::Options> options = new osgDB::Options("noRotation");
////		for (size_t i = 0; i < subDirs.size(); i++) {
////			string name = DirOrPathGetName(subDirs[i]);
////			string path1 = subDirs[i] + "/" + name + ".osgb";
////			osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(path1);
////			string modelName = Utility::getFileNameFromPath(path1);
////
////			PositionVisitor visitor = PositionVisitor(modelName, textSize);
////			node->accept(visitor);
////			//OBB包围盒
////			//for (Geom* geom : visitor.getAllGeom())
////			//{
////			//	osg::ref_ptr<osg::Vec3Array> array = new osg::Vec3Array;
////			//	for (Vertex* vertex : geom->vertices)
////			//		array->push_back(vertex->coor);
////			//	render_system::OBB obb = render_system::OBB(array);
////			//	root->addChild(createOBBBox(obb));
////			//}
////
////			//AABB包围盒
////			root->addChild(visitor.createRandomColorOsgNode(i));
////			osgUtil::Optimizer optimizer;
////			optimizer.optimize(root.get());
////			viewer->setSceneData(root);
////			osg::ComputeBoundsVisitor boundsVist;
////			node->accept(boundsVist);
////			osg::BoundingBox box = boundsVist.getBoundingBox();
////			OSG_NOTIFY(osg::ALWAYS) << "bound info:" << std::endl;;
////			osg::notify(osg::ALWAYS) << "bouding box info " << name << std::endl;
////			osg::notify(osg::ALWAYS) << "center x:" << box.center().x() << std::endl;
////			osg::notify(osg::ALWAYS) << "center y:" << box.center().y() << std::endl;;
////			osg::notify(osg::ALWAYS) << "center z:" << box.center().z() << std::endl;;
////			osg::notify(osg::ALWAYS) << "x max:" << box.xMax() << std::endl;
////			osg::notify(osg::ALWAYS) << "x min:" << box.xMin() << std::endl;
////			osg::notify(osg::ALWAYS) << "y max:" << box.yMax() << std::endl;
////			osg::notify(osg::ALWAYS) << "y min:" << box.yMin() << std::endl;
////			osg::notify(osg::ALWAYS) << "z max:" << box.zMax() << std::endl;
////			osg::notify(osg::ALWAYS) << "z min:" << box.zMin() << std::endl;
////			//画个外框
////			osg::ref_ptr<osg::Geode> geodeBox = new osg::Geode();
////			geodeBox->addDrawable(new osg::ShapeDrawable(new osg::Box(box.center(), box.xMax() - box.xMin(), box.yMax() - box.yMin(), box.zMax() - box.zMin())));
////			osg::ref_ptr<osg::StateSet> geodeStateSet = geodeBox->getOrCreateStateSet();
////
////			//采用线框模式，以便能看见里面的模型
////			osg::ref_ptr<osg::PolygonMode> geodePolygon = new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
////			geodeStateSet->setAttribute(geodePolygon);
////			root->addChild(geodeBox);
////		}
////		
////		//root->addChild(Utility::createCoorAxis(textSize));
////		//cout << "num children" << root->getNumChildren() << endl;
////		viewer->setSceneData(root);
////		viewer->addEventHandler(new osgGA::StateSetManipulator(viewer->getCamera()->getOrCreateStateSet()));
////		viewer->addEventHandler(new osgViewer::StatsHandler);
////		viewer->setUpViewInWindow(10, 10, 800, 600);
////		viewer->run();
////		
////	}
////	return 0;
////}
//深拷贝并简化新模型
osg::Node *deepCopy(osg::Node *node)
{
	/*
	创建简化对象
	simplifier(sampleRatio, maxError)
	参数：样本比率、点的误差或边的长度
	样本比率<1 设置点的误差
	样本比率>1 设置边的长度限制
	比率越大，简化越少
	使用的是边塌陷算法
	*/
	float sampleRatio = 0.3f;
	float maxError = 4.0f;
	osgUtil::Simplifier simplifier(sampleRatio, maxError);

	/*深拷贝*/
	osg::ref_ptr<osg::Node> deepnode = (osg::Node *)(node->clone(osg::CopyOp::DEEP_COPY_ALL));
	//创建一个位置变换节点,将之设置为新位置，将深拷贝的模型移到新位置
	osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform();
	pat->setPosition(osg::Vec3(10.0f, 0.0f, 0.0f));
	pat->addChild(deepnode);

	pat->accept(simplifier);
	return pat.release();

}
//对osgb模型进行简化并且进行对比
int main()
{
	srand((unsigned)time(NULL));
	int textSize = 5;
	osg::ref_ptr<osgViewer::Viewer> spViewer = new osgViewer::Viewer();
	osg::ref_ptr<osg::Group> spGroup = new osg::Group();
	osg::ref_ptr<osgDB::Options> options = new osgDB::Options("noRotation");
	vector < string > subDirs;
	string path = "G:/shiyandata/BH90F/Data";
	findDir(path, subDirs);
	/*
	创建简化对象
	simplifier(sampleRatio, maxError)
	参数：样本比率、点的误差或边的长度
	样本比率<1 设置点的误差
	样本比率>1 设置边的长度限制
	比率越大，简化越少
	使用的是边塌陷算法
	*/
	for (size_t i = 0; i < subDirs.size(); i++) {
		string name = DirOrPathGetName(subDirs[i]);
		string path1 = subDirs[i] + "/" + name + ".osgb";
		osg::ref_ptr<osg::Node> spNode = osgDB::readNodeFile(path1);
		spGroup->addChild(createLight(spNode.get()));
		osg::ref_ptr<osg::Node> pat = deepCopy(spNode);
		spGroup->addChild(pat);
		osgUtil::Optimizer optimizer;
		optimizer.optimize(spGroup.get());
		spViewer->setSceneData(spGroup);
		spViewer->addEventHandler(new osgGA::StateSetManipulator(spViewer->getCamera()->getOrCreateStateSet()));
		spViewer->addEventHandler(new osgViewer::StatsHandler);
	}
	spViewer->setUpViewInWindow(10, 10, 800, 600);
	spViewer->realize();
	spViewer->run();
	return 0;
}

//////显示osgb模型AABB包围盒三角网
////int main()
////{
////	srand((unsigned)time(NULL));
////	int textSize = 1;
////	while (true)
////	{
////		vector < string > subDirs;
////		string path = "G:/寻甸县羊街镇/Data";
////		findDir(path, subDirs);
////		osg::ref_ptr<osgViewer::Viewer> viewer=new osgViewer::Viewer();
////		osg::ref_ptr<osg::Group> root = new osg::Group;
////		osg::ref_ptr<osgDB::Options> options = new osgDB::Options("noRotation");
////		osg::BoundingBox mRootBoundBox;
////		for (size_t i = 0; i < subDirs.size(); i++) {
////			string name = DirOrPathGetName(subDirs[i]);
////			string path1 = subDirs[i] + "/" + name + ".osgb";
////			osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(path1);
////			if (node.valid())
////			{
////				osg::BoundingBox box = getBoundBox(node);
////
////				mRootBoundBox.expandBy(box);
////				root->addChild(node);
////			}
////		}
////		
////		osg::ref_ptr<osg::Group> splitGroup = tile2(root, 32, false);
////
////		for (int i = 0; i < (int)splitGroup->getNumChildren(); i++)
////		{
////			osg::Node* child = splitGroup->getChild(i);
////
////			/*osg::ref_ptr<OC::Cesium::Tile> tile;*/
////			if (splitGroup->getNumChildren() == 1)
////			{
////				tile = tileset->root();
////			}
////			else
////			{
////				tile = new OC::Cesium::Tile;
////				tile->geometricError() = tileset->root()->geometricError().get() * 0.5;
////				tile->boundingVolume()->box() = Cesium::CesiumTool::getBoundBox(child);
////				tileset->root()->children().push_back(tile);
////			}
////
////			string uri = Stringify() << i << ".b3dm";
////			tile->content()->uri() = uri;
////			string filename = mDestDir + uri;
////			osgDB::makeDirectoryForFile(filename);
////
////			FindPagedlod fpl(new osg::Group);
////			child->accept(fpl);
////
////			pushWriteRequest(workqueue, filename, child);
////
////			if (fpl._g->getNumChildren() == 0)
////			{
////				tile->geometricError() = 0.0;
////				continue;
////			}
////
////			pushSplitRequest(workqueue, this, String(), fpl._g, tile, i);
////		}
////		while (workqueue.finished != 0) {
////			workqueue.mWorkQueue->processResponses();
////			Sleep(100);
////		}
////
////		string tilesetFile = mDestDir + "tileset.json";
////		OC_WRITE_Tiltset(tileset, tilesetFile);
////
////
////
////
////		
////		
////		viewer->setSceneData(root);
////		viewer->addEventHandler(new osgGA::StateSetManipulator(viewer->getCamera()->getOrCreateStateSet()));
////		viewer->addEventHandler(new osgViewer::StatsHandler);
////		viewer->setUpViewInWindow(10, 10, 800, 600);
////		viewer->run();
////		
////	}
////	return 0;
////}
//
//#include <osg/Camera>
//#include <osg/Texture2D>
//#include <osgDB/ReadFile>
//#include <osgViewer/Viewer>
//
//osg::Texture* createRttTexture(int texWidth, int texHeight)
//{
//	osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D();
//	texture->setInternalFormat(GL_RGBA);
//	texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
//	texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
//	texture->setTextureSize(texWidth, texHeight);
//
//	return texture.release();
//}
//
//osg::Camera* createRttCamera(int texWidth, int texHeight, const osg::BoundingSphere& bs)
//{
//	osg::ref_ptr<osg::Camera> rttCamera = new osg::Camera();
//	rttCamera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	rttCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
//	rttCamera->setViewport(0, 0, texWidth, texHeight);
//	rttCamera->setRenderOrder(osg::Camera::PRE_RENDER);
//	rttCamera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
//
//	double viewDistance = 2.0 * bs.radius();
//	double znear = viewDistance - bs.radius();
//	double zfar = viewDistance + bs.radius();
//	double top = 0.6 * znear;
//	double right = 0.8 * znear;
//	rttCamera->setProjectionMatrixAsFrustum(-right, right, -top, top, znear, zfar);
//
//	osg::Vec3d upDirection(0.0, 0.0, 1.0);
//	osg::Vec3d viewDirection(0.0, -1.0, 0.0);
//	osg::Vec3d center = bs.center();
//	osg::Vec3d eyePoint = center + viewDirection * viewDistance;
//	rttCamera->setViewMatrixAsLookAt(eyePoint, center, upDirection);
//
//	return rttCamera.release();
//}
//
//int main()
//{
//	osg::Node* model = osgDB::readNodeFile("G:/xundianxianqiaodicun/Data/BlockBABBY/test.osgt");
//	model->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
//
//	int texWidth = 512;
//	int texHeight = 512;
//	osg::Camera* rttCamera = createRttCamera(texWidth, texHeight, model->getBound());
//	osg::Texture* rttTexture = createRttTexture(texWidth, texHeight);
//	rttCamera->addChild(model);
//	rttCamera->attach(osg::Camera::COLOR_BUFFER, rttTexture);
//
//	osg::ref_ptr<osg::Geode> quad = new osg::Geode();
//	quad->addDrawable(osg::createTexturedQuadGeometry(
//		osg::Vec3(0.0, 0.0, 0.0), osg::Vec3(1.0, 0.0, 0.0),
//		osg::Vec3(0.0, 0.0, 1.0)));
//	quad->getOrCreateStateSet()->setTextureAttributeAndModes(0, rttTexture);
//
//	osg::ref_ptr<osg::Group> root = new osg::Group();
//	root->addChild(quad);
//	root->addChild(rttCamera);
//
//	osgViewer::Viewer viewer;
//	viewer.setSceneData(root);
//	return viewer.run();
//}