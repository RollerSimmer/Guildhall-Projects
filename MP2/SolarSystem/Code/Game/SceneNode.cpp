#include "Game/SceneNode.hpp"
#include <iostream>
#include <assert.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <gl/GL.h>

size_t SceneNode::s_nodeCount=0;

SceneNode::SceneNode(const std::string& name,SceneNode* parent)
	:	m_name(name),
		m_parent(parent)
	{
	++s_nodeCount;
	}

SceneNode::~SceneNode()
	{
	destroy();
	assert(s_nodeCount >= 1);  
	s_nodeCount--;
	}

void SceneNode::addChild(SceneNode* child)
	{
	if(child==nullptr)
		return;
	bool isNodeAlreadyInChildren=false;
	for (int i=0;i<(int)m_children.size();i++)
		{
		if(m_children.at(i)==child)
			isNodeAlreadyInChildren=true;
		}
	if(isNodeAlreadyInChildren)
		return;
	child->setParent(this);
	m_children.push_back(child);
	}

void SceneNode::update()
	{
	Children::const_iterator it = m_children.begin(),ite = m_children.end();
	for (; it != ite; ++it)
		(*it)->update();
	}

void SceneNode::cull(Traversal* trav) const
	{
	std::cout << "Node::cull " << m_name << std::endl;
	Children::const_iterator it = m_children.begin(),ite = m_children.end();
	for (; it != ite; ++it)
		(*it)->cull(trav);
	}

const std::string& SceneNode::getName()
	{
	return m_name;
	}

SceneNode::const_iterator_child SceneNode::begin_child()
	{
	return m_children.begin();
	}

SceneNode::const_iterator_child SceneNode::end_child()
	{
	return m_children.end();
	}

size_t SceneNode::getNodeCount()
	{
	return s_nodeCount;
	}

void SceneNode::destroy()
	{
	Children::iterator itc = m_children.begin(),itce = m_children.end();
	for (; itc != itce; ++itc)
		{
		(*itc)->destroy();
		delete *itc;
		}
	m_children.clear();
	}

void SceneNode::checkNodeCount()
	{
	assert(s_nodeCount == 0);
	}

void SceneNode::setParent(SceneNode* parent)
	{
	m_parent=parent;
	}


SceneNode::Traversal::Traversal()
	{
	m_view.makeIdentity();
	}

void SceneNode::Traversal::initToViewMatrix(const Matrix4& view)
	{
	m_stack.erase(m_stack.begin(),m_stack.end());
	m_stack.push_back(view);
	}

void SceneNode::Traversal::pushMatrixStack(Matrix4& modelMatrix)
	{
	m_stack.push_back(modelMatrix);
	}

void SceneNode::Traversal::popMatrixStack()
	{
	m_stack.pop_back();
	}

const Matrix4& SceneNode::Traversal::topMatrixStack() const
	{
	return m_stack.back();
	}

void TransformNode::update()
	{
	if (m_strategy)
		m_strategy->update(m_transform);
	SceneNode::update();
	}

void TransformNode::cull(Traversal* trav) const
	{
	std::cout << "TransformNode::cull " << m_name << std::endl;
	Matrix4 transform(m_transform);
	trav->pushMatrixStack(transform);
	Children::const_iterator it = m_children.begin(),ite = m_children.end();
	for (; it != ite; ++it)
		(*it)->cull(trav);
	trav->popMatrixStack();
	}

void GeometryNode::cull(Traversal* trav) const
	{
	std::cout << "GeometryNode::cull" << std::endl;
	SceneNode::cull(trav);
	glLoadMatrixf(trav->topMatrixStack().getAsFloatArray());
// 	switch (m_type)
// 		{
// 		case Teapot:
// 			glColor3f(1.0f,1.0f,0.0);
// 			renderWireTeapot(2.0);
// 			break;
// 		case Sphere:
// 			glColor3f(0.0f,1.0f,1.0f);
// 			renderWireSphere(2.0,10.0,10.0);
// 			break;
// 		default:
// 			;
// 		}
	}

RotateStrategy::~RotateStrategy()
	{
	std::cout << "RotateStrategy::~RotateStrategy" << std::endl;
	}

void RotateStrategy::update(Matrix4& m)
	{
	m_angle += 0.01;
	std::cout << "RotateStrategy::update with angle=" << m_angle << std::endl;
	//Say, rotation about Z axis
	m.m_values[0] = cos(m_angle); m.m_values[1] = sin(m_angle); m.m_values[2] = 0.0; m.m_values[3] = 0.0;
	m.m_values[4] = -sin(m_angle); m.m_values[5] = cos(m_angle); m.m_values[6] = 0.0; m.m_values[7] = 0.0;
	m.m_values[8] = 0.0; m.m_values[9] = 0.0; m.m_values[10] = 1.0; m.m_values[11] = 0.0;
	m.m_values[12] = m.m_values[13] = m.m_values[14] = 0.0; m.m_values[15] = 1.0;
	}

TranslateStrategy::~TranslateStrategy()
	{
	std::cout << "TranslateStrategy::~TranslateStrategy" << std::endl;
	}

void TranslateStrategy::update(Matrix4& m)
	{
	m_angle += 0.02;
	std::cout << "TranslateStrategy::update with angle=" << m_angle << std::endl;
	//Say, rotation about Z axis
	m.m_values[0] = 1.0;  m.m_values[1] =      m.m_values[2] =       m.m_values[3] = 0.0;
	m.m_values[4] = 0.0;  m.m_values[5] = 1.0; m.m_values[6] = 0.0;  m.m_values[7] = 0.0;
	m.m_values[8] = 0.0;  m.m_values[9] = 0.0; m.m_values[10] = 1.0; m.m_values[11] = 0.0;
	m.m_values[12] = m_radius*cos(m_angle);  m.m_values[13] = m_radius*sin(m_angle);  m.m_values[14] = 0.0; m.m_values[15] = 1.0;
	}
