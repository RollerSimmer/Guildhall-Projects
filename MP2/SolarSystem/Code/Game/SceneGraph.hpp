#pragma once
#include "Game/SceneNode.hpp"

class SceneGraph
	{
	public:
		SceneGraph(SceneNode* root);

		void setRoot(SceneNode* newRoot) { m_root = newRoot; }

		void update(float deltaSeconds);
		void render() const;

	public:
		SceneNode* m_root;

	};

