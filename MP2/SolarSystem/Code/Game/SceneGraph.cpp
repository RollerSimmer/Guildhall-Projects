#include "Game/SceneGraph.hpp"
#include "Engine/Core/EngineBase.hpp"
#include <stack>
#include "Game/CelestialNode.hpp"
#include "Game/GameCommon.hpp"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <gl/GL.h>

SceneGraph::SceneGraph(SceneNode* root)
	{
	m_root=root;
	}

void SceneGraph::update(float deltaSeconds)
	{
	if (m_root==nullptr)
		return;
	SceneNode::Traversal trav;
	trav.m_view.makeIdentity();
	std::stack<CelestialNode*> nodeStack;
	CelestialNode* curNode=nullptr;
// 	trav.pushMatrixStack(trav.m_view);
	trav.pushMatrixStack(trav.m_view);
	nodeStack.push((CelestialNode*)m_root);

	while (!nodeStack.empty())
		{
		curNode=nodeStack.top();	
		nodeStack.pop();
		if(curNode==nullptr)
			continue;

		curNode->orbit(deltaSeconds);
		curNode->rotate(deltaSeconds);

// 		trav.m_view=trav.topMatrixStack();
// 		trav.popMatrixStack();
// 		Matrix4 parentView=trav.m_view;

		SceneNode::Children children=curNode->getChildren();
		for (SceneNode* child: children)
			{
// 			Matrix4 childView(parentView);
			CelestialNode* celChild=(CelestialNode*)child;
			nodeStack.push(celChild);
// 			Matrix4 childRotation = celChild->rotate(deltaSeconds);
// 			childView = childView.getTransformed(celChild->orbit(deltaSeconds));
// 			trav.pushMatrixStack(childView);
			}
		}
	}

void SceneGraph::render() const
	{
	if (m_root==nullptr)
		return;
	SceneNode::Traversal trav;
	float* M=trav.m_view.getAsFloatArray();
	glGetFloatv(GL_MODELVIEW_MATRIX,M);

	std::stack<CelestialNode*> nodeStack;
	CelestialNode* curNode=nullptr;
	trav.pushMatrixStack(trav.m_view);
	nodeStack.push((CelestialNode*)m_root);

	if(g_theDebugMessagePrinter==nullptr)
		return;

	g_theRenderer->pushGraphicsMatrix();

	while (!nodeStack.empty())
		{
		curNode=nodeStack.top();
		nodeStack.pop();
		if (curNode==nullptr)
			continue;

		trav.m_view=trav.topMatrixStack();
		trav.popMatrixStack();

		g_theRenderer->loadGraphicsMatrix(trav.m_view);

// 		while (curNode->getName()=="Jupiter" || curNode->getName()=="Earth")
// 			break;

// 		Vector3 rotAxis=curNode->getRotationAxis();
// 		float rotAngleDegrees=curNode->getRotationAngleDegrees();




		Vector3 posOfs=curNode->getRelativePosition();

		g_theRenderer->translate3D(posOfs);
//  		trav.m_view=curNode->getOrbitalMatrix().getTransformed(trav.m_view);
// 		g_theRenderer->loadGraphicsMatrix(trav.m_view);

		trav.m_view=g_theRenderer->getCurrentModelViewMatrix();
		g_theRenderer->pushGraphicsMatrix();

// 		g_theRenderer->rotate3D(rotAngleDegrees,rotAxis);
/*		trav.m_view=;*/
 		g_theRenderer->loadGraphicsMatrix(curNode->getRotationMatrix().getTransformed(trav.m_view));

		curNode->render();
		g_theRenderer->popGraphicsMatrix();

		Matrix4 parentView=trav.m_view;

		SceneNode::Children children=curNode->getChildren();
		for (SceneNode* child: children)
			{
			Matrix4 childView(parentView);
			CelestialNode* celChild=(CelestialNode*)child;
			nodeStack.push(celChild);
			trav.pushMatrixStack(childView);
			}
		}

	g_theRenderer->popGraphicsMatrix();

	}
