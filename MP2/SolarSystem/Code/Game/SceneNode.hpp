#pragma once
#include <vector>
#include "Engine/Math/Matrix4.hpp"

class SceneNode
	{
	public:
		typedef std::vector<SceneNode*> Children;
		typedef Children::const_iterator const_iterator_child;

		struct Traversal
			{
			Matrix4 m_view;
			std::vector<Matrix4> m_stack;
			
			Traversal();

			void initToViewMatrix(const Matrix4& view);
			void pushMatrixStack(Matrix4& modelMatrix);
			void popMatrixStack();
			const Matrix4& topMatrixStack() const;
			};

		SceneNode(const std::string& name,SceneNode* parent);
		virtual ~SceneNode();
		void addChild(SceneNode* child);
		virtual void update();
		virtual void cull(Traversal* trav) const;
		const std::string& getName();
		const_iterator_child   begin_child();
		const_iterator_child   end_child();
		static size_t getNodeCount();
		void destroy();		
		static void checkNodeCount();
		void    setParent(SceneNode* parent);
		SceneNode* getParent() { return m_parent; }
		Children getChildren() { return m_children; };
	protected:
		SceneNode*           m_parent;
		Children        m_children;
		std::string     m_name;
		//static variables are BAD for mulithreading, btw.
		static size_t s_nodeCount;
	};


class TransformNode: public SceneNode
	{
	public:

		//The Strategy Design pattern
		class Strategy
			{
			public:
				Strategy() {}
				virtual ~Strategy() {}
				virtual void update(Matrix4& m) = 0;
			};

		TransformNode(const char* name,SceneNode* parent=nullptr): SceneNode(name,parent),m_strategy(nullptr) {}
		~TransformNode() {}

		void setStrategy(Strategy* s) { m_strategy = s; }
		virtual void update();

		virtual void cull(Traversal* trav) const;


	protected:
		Matrix4  m_transform;
		Strategy*  m_strategy;
	};

class GeometryNode: public SceneNode
	{
	public:

		enum Type { Teapot,Sphere };

		GeometryNode(const char* name,SceneNode* parent=nullptr,Type type = Teapot): SceneNode(name,parent),m_type(type) 		{}
		virtual void cull(Traversal* trav) const;
	protected:
		Type    m_type;
	};

//Some sample transformation strategies, to play with
class RotateStrategy: public TransformNode::Strategy
	{
	public:
		RotateStrategy(): m_angle(0.0) {}
		~RotateStrategy(); 
		virtual void update(Matrix4& m);
	private:
		double  m_angle;
	};

class TranslateStrategy: public TransformNode::Strategy
	{
	public:
		TranslateStrategy(): m_angle(0.0),m_radius(1.0) {}
		~TranslateStrategy();

		virtual void update(Matrix4& m);
	private:
		double  m_angle;
		double  m_radius;
	};
