#include "Engine/Renderer/Renderer.hpp"





/*#define GL_GLEXT_PROTOTYPES*/
#include "ThirdParty/OpenGL/OpenGLExtensions.hpp"

#pragma comment( lib, "opengl32" ) // Link in the OpenGL32.lib static library
#pragma comment( lib, "Glu32" ) // Link in the OpenGL32.lib static library

#include "Engine/Math/MathUtil.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "ThirdParty/stb/stb_image.h"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Math/Matrix4.hpp"
#include <iostream>

#include <math.h>

const int PRIMITIVE_POINTS				= GL_POINTS;
const int PRIMITIVE_LINES				= GL_LINES;
const int PRIMITIVE_LINE_LOOP			= GL_LINE_LOOP;
const int PRIMITIVE_TRIANGLES			= GL_TRIANGLES;
const int PRIMITIVE_QUADS				= GL_QUADS;
const int PRIMITIVE_TRIANGLE_FAN		= GL_TRIANGLE_FAN;

const int GFXSTATE_BLEND				= GL_BLEND;
const int BLENDMODE_SRC_ALPHA			= GL_SRC_ALPHA;
const int BLENDMODE_ONE_MINUS_SRC_ALPHA	= GL_ONE_MINUS_SRC_ALPHA;
const int BLENDMODE_SMOOTH				= GL_SMOOTH;

float Renderer::calcRadiansEndForCircle()
	{
	const float TWO_PI_THRESHOLD = 0.01f;
	float result = (M_PI * 2.f) + TWO_PI_THRESHOLD;
	return result;
	}


Renderer::Renderer()
	: m_alreadyLoadedTextures()
	, m_currentTexture(nullptr)
	, m_whiteTexture(nullptr)
	{
	glGenBuffers		= (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
	glBindBuffer		= (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
	glBufferData		= (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
	glDeleteBuffers		= (PFNGLDELETEBUFFERSPROC)wglGetProcAddress("glDeleteBuffers");
	glGenerateMipmap	= (PFNGLGENERATEMIPMAPPROC)wglGetProcAddress("glGenerateMipmap");
	
	initWhiteTexture();
	enable(GL_TEXTURE_2D);
	}

void Renderer::drawLineLoop(std::vector<Vertex3>& verts,float thickness/*=2.f*/)
	{
	bindTexture(nullptr);
	setLineWidth(thickness);
	bindTexture(nullptr);
	int primitiveType=PRIMITIVE_LINE_LOOP;
	
	drawVertexArray3(verts.data(),(int)verts.size(),primitiveType);
	}

void Renderer::drawLine(float startx, float starty, float endx, float endy, const Rgba& color, float thickness) 
	{
	bindTexture(nullptr);
	setLineWidth(thickness);
	setDrawingColor(color);

	std::vector<Vertex3> verts;

	Vector2 texCoords(0.f,0.f);

	verts.push_back(Vertex3(Vector3(startx,starty,0.f),color,texCoords));
	verts.push_back(Vertex3(Vector3(endx, endy,0.f),color,texCoords));

	int primitiveType=PRIMITIVE_LINES;
	drawVertexArray3(verts.data(),(int)verts.size(),primitiveType);
	}

void Renderer::drawLine(const LineSegment2& line, const Rgba& color/*=Rgba::WHITE*/, float thickness/*=2.f*/)
	{
	drawLine(line.m_start.x,line.m_start.y,line.m_end.x,line.m_end.y,color,thickness);
	}

void Renderer::drawLine3(const LineSegment3& line,const Rgba& color/*=Rgba::WHITE*/,float thickness/*=2.f*/)
	{
	bindTexture(nullptr);
	glLineWidth(thickness);
	setDrawingColor(color);

	std::vector<Vertex3> verts;

	Vector2 texCoords(0.f,0.f);

	verts.push_back(Vertex3(line.m_start,color,texCoords));
	verts.push_back(Vertex3(line.m_end,color,texCoords));

	int primitiveType=PRIMITIVE_LINES;
	drawVertexArray3(verts.data(),(int)verts.size(),primitiveType);
	}

void Renderer::clearScreen(float red, float green, float blue)
	{
	glClearColor((GLclampf)red, (GLclampf)green, (GLclampf)blue, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);
	}

void Renderer::clearScreen(const Rgba& color)
	{
	float r,g,b,a;
	color.getAsFloats(r,g,b,a);
	glClearColor(r,g,b,a);
	glClear(GL_COLOR_BUFFER_BIT);
	}

void Renderer::clearDepth()
	{
	glClear(GL_DEPTH_BUFFER_BIT);
	}

void Renderer::clearScreenAndDepth(const Rgba& color)
	{
	clearScreen(color);
	clearDepth();
	}

void Renderer::rotate3D(float angleDegrees,const Vector3& axis)
	{
	glRotatef(angleDegrees,axis.x,axis.y,axis.z);
	}

void Renderer::lookAt(const Vector3& fromPos,const Vector3& toPos,const Vector3& up)
	{
	gluLookAt(fromPos.x,fromPos.y,fromPos.z,
			  toPos.x,toPos.y,toPos.z,
			  up.x,up.y,up.z);
	}

void Renderer::rotateDrawingSurface(float xcenter,float ycenter, float angleDegrees)
	{
	glTranslatef(xcenter, ycenter, 0.f);
	glRotatef(angleDegrees, 0.f, 0.f, 1.f);
	glTranslatef(-xcenter, -ycenter, 0.f);
	}

void Renderer::drawRegularPolygon(float x, float y, int amtsides, float radius, const Rgba& color /*= Rgba::WHITE*/, float lineThickness/*=2.f*/)
	{
	drawRotatedRegularPolygon(x,y,amtsides,radius,0.f, color,lineThickness);
	}


void Renderer::drawCircle(float x, float y, float radius, int numsides, const Rgba& color/*=Rgba::WHITE*/, float lineThickness/*=2.f*/)
	{
	drawRegularPolygon(x, y, numsides, radius, color,lineThickness);
	}

void Renderer::drawFilledDisc2(float x, float y, float radius, int numsides, const Rgba& color/*= Rgba::WHITE*/)
	{
	bindTexture(nullptr);
	std::vector<Vertex3> verts;
	float tx,ty;
	Vector2 texCoords(0.f,0.f);
	verts.push_back(Vertex3(Vector3(x,y,0.f),color,texCoords));
	float radianStep = (M_PI * 2.f) / (float)numsides;
	float radiansEnd = calcRadiansEndForCircle();
	for (float radians = 0.f; radians <= radiansEnd; radians += radianStep)
		{
		tx = x + radius*(float)cos(radians);
		ty = y + radius*(float)sin(radians);
		verts.push_back(Vertex3(Vector3(tx,ty,0.f),color,texCoords));
		}
	int primitiveType=PRIMITIVE_TRIANGLE_FAN;
	drawVertexArray3(verts.data(),(int)verts.size(),primitiveType);
	}

void Renderer::drawShinyDisc2(float x, float y, float radius, int numsides, const Rgba color/*=Rgba::GRAY*/)
	{
	bindTexture(nullptr);
	std::vector<Vertex3> verts;
	float tx, ty;
	Vector2 texCoords(0.f,0.f);
	verts.push_back(Vertex3(Vector3(x,y,0.f),Rgba::WHITE,texCoords));
	float radianStep = (M_PI * 2.f) / (float)numsides;
	float radiansEnd = calcRadiansEndForCircle();
	for (float radians = 0.f; radians <= radiansEnd; radians += radianStep)
		{
		tx = x + radius*(float)cos(radians);
		ty = y + radius*(float)sin(radians);
		verts.push_back(Vertex3(Vector3(tx,ty,0.f),color,texCoords));
		}
	int primitiveType=PRIMITIVE_TRIANGLE_FAN;
	drawVertexArray3(verts.data(),(int)verts.size(),primitiveType);
	}

void Renderer::drawBox(float left, float top, float right, float bottom, const Rgba& color/*= Rgba::WHITE*/, float lineThickness/*=2.f*/)
	{
	bindTexture(nullptr);
	std::vector<Vertex3> verts;
	setLineWidth(lineThickness);
	Vector2 texCoords(0.f,0.f);

	verts.push_back(Vertex3(Vector3(left ,top   ,0.f),color,texCoords));
	verts.push_back(Vertex3(Vector3(left ,bottom,0.f),color,texCoords));
	verts.push_back(Vertex3(Vector3(right,bottom,0.f),color,texCoords));
	verts.push_back(Vertex3(Vector3(right,top   ,0.f),color,texCoords));

	int primitiveType=PRIMITIVE_LINE_LOOP;
	drawVertexArray3(verts.data(),(int)verts.size(),primitiveType);
	}

void Renderer::drawBox(const AABB2& box,const Rgba& color,float borderThickness)
	{
	drawBox(box.m_mins.x,box.m_mins.y,box.m_maxs.x,box.m_maxs.y,color,borderThickness);
	}

void Renderer::drawFilledBox(const AABB2& box, const Rgba& color)
	{
	bindTexture(nullptr);
	std::vector<Vertex3> verts;
	Vector2 texCoords(0.f,0.f);
	setLineWidth(5.0);
	Vector2 center=box.CalcCenter();

	verts.push_back(Vertex3(Vector3(center.x,center.y,0.f),color,texCoords));
	verts.push_back(Vertex3(Vector3(box.m_mins.x,box.m_mins.y,0.f),color,texCoords));
	verts.push_back(Vertex3(Vector3(box.m_maxs.x,box.m_mins.y,0.f),color,texCoords));
	verts.push_back(Vertex3(Vector3(box.m_maxs.x,box.m_maxs.y,0.f),color,texCoords));
	verts.push_back(Vertex3(Vector3(box.m_mins.x,box.m_maxs.y,0.f),color,texCoords));
	verts.push_back(Vertex3(Vector3(box.m_mins.x, box.m_mins.y,0.f),color,texCoords));
	
	int primitiveType=PRIMITIVE_TRIANGLE_FAN;
	drawVertexArray3(verts.data(),(int)verts.size(),primitiveType);

	}

void Renderer::drawFilledBox3(const AABB3& box,const Rgba& color)
	{
	UNUSED(color);
	UNUSED(box);
// 	drawFilleQuad(box.m_mins,)
// 
	}

void Renderer::drawFilledQuad(const Vector3& A,const Vector3& B,const Vector3& C,const Vector3& D,const Rgba& color)
	{
	bindTexture(nullptr);
	std::vector<Vertex3> verts;
	Vector2 texCoords(0.f,0.f);
	enableBackFaceCulling(false);
	// 	setLineWidth(5.0);
	Vector3 center=(A+B+C+D)*0.25f;

	verts.push_back(Vertex3(Vector3(center.x,center.y,center.z),color,texCoords));
	verts.push_back(Vertex3(Vector3(A.x,A.y,A.z),color,texCoords));
	verts.push_back(Vertex3(Vector3(B.x,B.y,B.z),color,texCoords));
	verts.push_back(Vertex3(Vector3(C.x,C.y,C.z),color,texCoords));
	verts.push_back(Vertex3(Vector3(D.x,D.y,D.z),color,texCoords));
	verts.push_back(Vertex3(Vector3(A.x,A.y,A.z),color,texCoords));

	int primitiveType=PRIMITIVE_TRIANGLE_FAN;
	drawVertexArray3(verts.data(),(int)verts.size(),primitiveType);
	}

void Renderer::drawWireframeSphere3(const Sphere3& sphere,const Rgba& color, float lineThickness)
	{
	std::vector<Vertex3> lastRow;
	std::vector<Vertex3> curRow;
	Vector2 texCoords(0.f,0.f);	
	pushGraphicsMatrix();
	translate3D(sphere.m_center);
	Vertex3 bottomVertex(Vector3(-sphere.m_radius,0.f,0.f),color,texCoords);
	float r,x,y,z;

	bool wasInitialized=false;
	for (float a=-90.f;a<=90.f;a+=22.5f)
		{
		r=sphere.m_radius*cosDegrees(a);
		z=sphere.m_radius*sinDegrees(a);
		if(curRow.size()>0)
			curRow.clear();
		for (float b=0.f;b<=360.f;b+=22.5f)
			{
			x=r*cosDegrees(b);
			y=r*sinDegrees(b);
			if (!wasInitialized)
				{
				lastRow.push_back(bottomVertex);
				}
			curRow.push_back(Vertex3(Vector3(x,y,z),color,texCoords));
			}
		wasInitialized=true;

		int amtVerts = curRow.size()>lastRow.size() ? (int)curRow.size() : (int)lastRow.size();
		for (int i=1;i<amtVerts;i++)
			{
			std::vector<Vertex3> faceVerts;
			faceVerts.push_back(lastRow.at(i-1));
			faceVerts.push_back(curRow.at(i));
			faceVerts.push_back(curRow.at(i-1));
			faceVerts.push_back(lastRow.at(i-1));
			faceVerts.push_back(lastRow.at(i));
			faceVerts.push_back(curRow.at(i));
			drawLineLoop(faceVerts,lineThickness);
			}
		lastRow=curRow;
		}
	translate3D(-sphere.m_center);
	popGraphicsMatrix();
	}

void Renderer::drawWireframeSnowman(const Snowman& snowman,const Rgba& color, float lineThickness/*=1.f*/)
	{
	for (int i=0;i<3;i++)
		{
		drawWireframeSphere3(snowman.m_spheres[i],color,lineThickness);
		}
	}

void Renderer::drawOffsetRotatedRegularPolygon(float x, float y, int amtsides, float radius, float orientationDegrees, const Vector2* vertexOffsets, const Rgba& color/*= Rgba::WHITE*/, float lineThickness/*=2.f*/)
	{
	pushGraphicsMatrix();
	bindTexture(nullptr);
	std::vector<Vertex3> verts;
	Vector2 texCoords(0.f,0.f);
	setLineWidth(lineThickness);
	rotateDrawingSurface(x,y,orientationDegrees);

	float tx, ty;
	float radianStep = (M_PI * 2.f) / (float)amtsides;
	int ivertex=0;
	float radiansEnd = calcRadiansEndForCircle();
	for (float radians = 0.f; radians <= radiansEnd; radians += radianStep)
		{
		tx = x + radius*(float)cos(radians);
		tx += vertexOffsets[ivertex].x;
		ty = y + radius*(float)sin(radians);
		ty += vertexOffsets[ivertex].y;
		verts.push_back(Vertex3(Vector3(tx, ty,0.f),color,texCoords));
		ivertex++;
		}

	int primitiveType=PRIMITIVE_LINE_LOOP;
	drawVertexArray3(verts.data(),(int)verts.size(),primitiveType);
	popGraphicsMatrix();
	}

void Renderer::setDrawingColor(Rgba color/*=WHITE*/)
	{
	glColor4ub(color.r,color.g,color.b,color.a);
	}

void Renderer::drawNGon(const Vector2& pos, float orientationDegrees, const Vector2* vertices, int amtVertices, const Rgba& color, float lineThickness/*=2.f*/)
	{
	pushGraphicsMatrix();
	bindTexture(nullptr);
	std::vector<Vertex3> verts;
	Vector2 texCoords(0.f,0.f);
	setLineWidth(lineThickness);
	float x=pos.x;
	float y=pos.y;
	rotateDrawingSurface(x, y, orientationDegrees);

	float tx, ty;
	for (int ivertex=0;ivertex<amtVertices;ivertex++)
		{
		tx = x+vertices[ivertex].x;
		ty = y+vertices[ivertex].y;
		verts.push_back(Vertex3(Vector3(tx,ty,0.f),color,texCoords));
		}

	int primitiveType=PRIMITIVE_LINE_LOOP;
	drawVertexArray3(verts.data(),(int)verts.size(),primitiveType);
	popGraphicsMatrix();
	}

void Renderer::pushGraphicsMatrix()
	{
	glPushMatrix();
	}

void Renderer::popGraphicsMatrix()
	{
	glPopMatrix();
	}

Matrix4 Renderer::getCurrentModelViewMatrix()
	{
	Matrix4 result;
	glGetFloatv(GL_MODELVIEW_MATRIX,result.getAsFloatArray());
	return result;
	}

void Renderer::setLineWidth(float width)
	{
	glLineWidth(width);
	}

void Renderer::setOrthoProjection(const Vector2& mins, const Vector2& maxs)
	{
	glLoadIdentity();
	glOrtho(mins.x,maxs.x,mins.y,maxs.y, 0.f, 1.f);
	}

void Renderer::setOrthoProjection(const AABB2& viewport)
	{
	setOrthoProjection(viewport.m_mins,viewport.m_maxs);
	}

void Renderer::setPerspectiveProjection(float fovVerticalDegrees,float aspectRatio, float nearDistance, float farDistance)
	{
	glLoadIdentity();
	gluPerspective(fovVerticalDegrees,aspectRatio,nearDistance,farDistance);
	}

void Renderer::blendFunc(int sfactor,int dfactor)
	{
	glBlendFunc(sfactor,dfactor);
	}

void Renderer::enable(int cap)
	{
	glEnable(cap);
	}

void Renderer::disable(int cap)
	{
	glDisable(cap);
	}

void Renderer::scale3D(const Vector3& scalingFactors)
	{
	glScalef(scalingFactors.x,scalingFactors.y,scalingFactors.z);
	}

void Renderer::translate2D(const Vector2& displacement)
	{
	glTranslatef(displacement.x,displacement.y,0.f);
	}

void Renderer::translate3D(const Vector3& displacement)
	{
	glTranslatef(displacement.x,displacement.y,displacement.z);
	}

void Renderer::loadGraphicsMatrix(const Matrix4& view)
	{
	const float* M=view.getAsFloatArray();
	glLoadMatrixf(M);
	}

void Renderer::drawRotatedRegularPolygon(float x, float y, int amtsides, float radius, float orientationDegrees, const Rgba& color, float lineThickness/*=2.f*/)
	{
	Vector2 vertexOffsets[1000];
	for(int i=0;i<amtsides;i++)
		vertexOffsets[i].SetXY(0.f,0.f);
	drawOffsetRotatedRegularPolygon(x, y, amtsides, radius, orientationDegrees, vertexOffsets, color, lineThickness);
	}

void Renderer::drawStarFivePointed(float x, float y, float radius, float orientationDegrees, const Rgba& color, float lineThickness/*=2.f*/)
	{
	const int AMT_POINTS=10;
	Vector2 points[AMT_POINTS];
	float scale=0.f;
	float angleDegrees=90.f;
	const float ANGLE_STEP=360.f/10.f;
	for (int ipoint = 0; ipoint < AMT_POINTS; ipoint++)
		{
		scale = (ipoint%2==0)? 1.f: 0.5f;
		points[ipoint].SetXY(scale*radius*cosDegrees(angleDegrees),scale*radius*sinDegrees(angleDegrees));		
		angleDegrees+=36.f;
		}
	drawNGon(makeVector2(x,y),orientationDegrees,points,AMT_POINTS,color, lineThickness);
	}

Rgba Renderer::calcLinearGradient(const LineSegment2& direction, const Vector2& point, const Rgba& colorStart, const Rgba& colorEnd)
	{
	Vector2 sp = point - direction.m_start;
	Vector2 se = direction.calcVector();
	Vector2 seNormalized = se;
	seNormalized.Normalize();
	float scale=1.23456789f;
	scale = sp.dotProduct(seNormalized) / direction.calcLength();
	scale=clampFloat(scale,0.f,1.f);
	Rgba result = (1.f-scale)*colorStart + scale*colorEnd;
	return result;
	}


void Renderer::drawTexturedAABB2(const AABB2& bounds, const Texture& texture, const AABB2& texcoords, 
	                             const Rgba& tint, float orientationDegrees /*= 0.f*/)
	{
	drawTexturedAABB2(bounds, texture, texcoords.m_mins, texcoords.m_maxs, tint, orientationDegrees);
	}

void Renderer::drawTexturedAABB2(const AABB2& bounds, const Texture& texture, 
	                             const Vector2& texCoordsAtMins, const Vector2& texCoordsAtMaxs, 
	                             const Rgba& tint, float orientationDegrees/*=0.f*/)
	{
	pushGraphicsMatrix();
	Vector2 center=bounds.CalcCenter();
	rotateDrawingSurface(center.x, center.y, orientationDegrees);
	bindTexture(&texture);

	float texleft = texCoordsAtMins.x;
	float texbottom = texCoordsAtMins.y;
	float texright = texCoordsAtMaxs.x;
	float textop = texCoordsAtMaxs.y;
	float left = bounds.m_mins.x;
	float top = bounds.m_mins.y;
	float right = bounds.m_maxs.x;
	float bottom = bounds.m_maxs.y;

	Vertex3 curVert;
	std::vector<Vertex3> verts;

	curVert.m_color=tint;
	curVert.m_texcoords=Vector2(texleft, texbottom);
	curVert.m_pos=Vector3(left,bottom,0.f);
	verts.push_back(curVert);

	curVert.m_color=tint;
	curVert.m_texcoords=Vector2(texleft,textop);
	curVert.m_pos=Vector3(left,top,0.f);
	verts.push_back(curVert);

	curVert.m_color=tint;
	curVert.m_texcoords=Vector2(texright, textop);
	curVert.m_pos=Vector3(right, top,0.f);
	verts.push_back(curVert);

	curVert.m_color=tint;
	curVert.m_texcoords=Vector2(texright,texbottom);
	curVert.m_pos=Vector3(right,bottom,0.f);
	verts.push_back(curVert);

	int primitiveType=PRIMITIVE_QUADS;
	drawVertexArray3(verts.data(),(int)verts.size(),primitiveType);

	popGraphicsMatrix();
	}


void Renderer::drawBoxWithOutline(const AABB2& box, const Rgba& fillcolor, const Rgba& outlinecolor)
	{
	drawFilledBox(box,fillcolor);
	drawBox(box,outlinecolor);
	}

void Renderer::drawTextLineCentered(const BitmapFont* theFont, const std::string& text,const Vector2& bottomCenterStartPos,float fontHeight,float aspect,
									const Rgba& color/*=Rgba::WHITE*/)
	{
	float fontWidth=aspect*fontHeight;
	float displacement=0.5f*fontWidth*(float)text.length();
	Vector2 newpos=bottomCenterStartPos;
	newpos.x-=displacement;
	drawTextLine(theFont,text,newpos,fontHeight,aspect,color);
	}

void Renderer::drawTextLine(const BitmapFont* theFont,const std::string& text, const Vector2& bottomLeftStartPos, float fontHeight, float aspect, const Rgba& color)
	{

	char curchar;
	AABB2 texcoords;
	float x;
	float y;
	float fontWidth=fontHeight*aspect;
	x=bottomLeftStartPos.x;
	y=bottomLeftStartPos.y-fontHeight;
	AABB2 boxbounds;
	float fontOrientationDegrees=0.f;
	Texture* fontTex=theFont->getTexture();
	if (fontTex)
		{
		for (int ichar = 0; ichar < (int)text.length(); ichar++)
			{
			boxbounds = AABB2(x, y, x + fontWidth, y + fontHeight);
			curchar = text.at(ichar);
			texcoords = theFont->getTexCoordsForChar(curchar);
			this->drawTexturedAABB2(boxbounds,*fontTex,texcoords,color,fontOrientationDegrees);
			x += fontWidth;
			}
		}
	}

void Renderer::drawTextLines(const BitmapFont* theFont, const std::string& text,
							 const Vector2& bottomLeftStartPos,float fontHeight,float aspect,const Rgba& color/*=Rgba::WHITE*/,
							 bool isCentered)
	{
	static const std::string TAB_SPACE="    ";
	int len=(int)text.length();
	std::string line("");
	Vector2 pos=bottomLeftStartPos;
	for (int i = 0; i < len ; i++)
		{
		char c=text.at(i);
		if(c=='\n')
			{
			if(isCentered)
				drawTextLineCentered(theFont,line,pos,fontHeight,aspect,color);
			else
				drawTextLine(theFont,line,pos,fontHeight,aspect,color);
			pos.y-=fontHeight;
			line="";
			}
		else if (c=='\t')
			line+=TAB_SPACE;
		else if (c<' ')
			;
		else
			line+=c;
		}
	if (isCentered)
		drawTextLineCentered(theFont,line,pos,fontHeight,aspect,color);
	else
		drawTextLine(theFont,line,pos,fontHeight,aspect,color);
	}

void Renderer::drawButtonTextLine(const BitmapFont* theFont, const std::string& text, const Vector2& bottomLeftStartPos, float fontHeight, float aspect, 
	           const Rgba& color /*= Rgba::WHITE*/, const Rgba& shadowcolor /*= Rgba(0x0000007F)*/, const Vector2& shadowDisplacement /*= Vector2(1.f, 1.f)*/)
	{
	Vector2 shadowBottomLeftStartPos=bottomLeftStartPos;
	shadowBottomLeftStartPos+=shadowDisplacement;
	drawTextLine(theFont,text, bottomLeftStartPos, fontHeight, aspect, color);
	drawTextLine(theFont,text, shadowBottomLeftStartPos, fontHeight, aspect, shadowcolor);
	}

void Renderer::drawTextLineWithShadow(const BitmapFont* theFont, const std::string& text, const Vector2& bottomLeftStartPos, float fontHeight, float aspect, const Rgba& color /*= Rgba::WHITE*/, const Rgba& shadowcolor /*= Rgba(0x0000007F)*/, const Vector2& shadowDisplacement /*= Vector2(1.f, 1.f)*/)
	{
	Vector2 shadowBottomLeftStartPos = bottomLeftStartPos;
	shadowBottomLeftStartPos += shadowDisplacement;
	drawTextLine(theFont,text, shadowBottomLeftStartPos, fontHeight, aspect, shadowcolor);
	drawTextLine(theFont,text, bottomLeftStartPos, fontHeight, aspect, color);
	}

void Renderer::enableBackFaceCulling(bool isNowBackfaceCulling)
	{
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	if (isNowBackfaceCulling)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);
	}

void Renderer::enableDepthTesting(bool isNowDepthTesting)
	{
	if (isNowDepthTesting)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
	}

void Renderer::initWhiteTexture()
	{
	std::vector<unsigned char> imgTexelBytes;
	for(int i=0;i<4;i++)
		imgTexelBytes.push_back(255);
	int width=1;
	int height=1;
	int bytesPerTexel=1;
	m_whiteTexture=CreateTextureFromRawData(imgTexelBytes,width,height,bytesPerTexel);
	}

void Renderer::drawVbo(unsigned int m_vboID,int amtVerticesInVbo, int primitiveType/*=PRIMITIVE_QUADS*/)
	{
	glBindBuffer(GL_ARRAY_BUFFER,m_vboID);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3,GL_FLOAT,sizeof(Vertex3),(const GLvoid*)offsetof(Vertex3,m_pos));
	glColorPointer(4,GL_UNSIGNED_BYTE,sizeof(Vertex3),(const GLvoid*)offsetof(Vertex3,m_color));
	glTexCoordPointer(2,GL_FLOAT,sizeof(Vertex3),(const GLvoid*)offsetof(Vertex3,m_texcoords));

	glDrawArrays(primitiveType,0,amtVerticesInVbo);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER,0);
	}

unsigned int Renderer::getBoundedVbo(const std::vector<Vertex3>& verts)
	{
	unsigned int result;
	glGenBuffers(1,&result);

	size_t vertsAmtBytes = sizeof(Vertex3)*verts.size();
	glBindBuffer(GL_ARRAY_BUFFER, result);
	glBufferData(GL_ARRAY_BUFFER,vertsAmtBytes,verts.data(),GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER,0);

	return result;
	}

void Renderer::unbindBuffer(unsigned int& vboID)
	{
	glDeleteBuffers(1,&vboID); 
	}

void Renderer::drawDots(const std::vector<Vector3>& dotPoints, const Rgba& color, float width)
	{
	std::vector<Vertex3> verts;
	Vector2 texCoords(0.f,0.f);

	enableDepthTesting(true);
	glPointSize(width);
	for (Vector3 dotPoint: dotPoints)
		{
		verts.push_back(Vertex3(dotPoint,color,texCoords));
		}
	Rgba halfcolor=color;
	halfcolor.scaleAlpha(0.5f);
	enableDepthTesting(false);
	glDisable(GL_TEXTURE_2D);

	for (Vector3 dotPoint: dotPoints)
		{
		verts.push_back(Vertex3(dotPoint,halfcolor,texCoords));
		}

	enableDepthTesting(true);
	int primitiveType=PRIMITIVE_POINTS;
	drawVertexArray3(verts.data(),(int)verts.size(),primitiveType);

	}

void Renderer::drawAxes(float axisLength)
	{
	bindTexture(nullptr);
	std::vector<Vertex3> verts;
	Vector2 texCoords(0.f,0.f);
	Vector3 pos;
	Rgba color;

	enableDepthTesting(true);
	color=Rgba::RED;
	pos=Vector3(0.f,0.f,0.f);
	verts.push_back(Vertex3(pos,color,texCoords));
	pos=Vector3(axisLength,0.f,0.f);
	verts.push_back(Vertex3(pos,color,texCoords));
	color=Rgba::GREEN;
	pos=Vector3(0.f,0.f,0.f);
	verts.push_back(Vertex3(pos,color,texCoords));
	pos=Vector3(0.f,axisLength,0.f);
	verts.push_back(Vertex3(pos,color,texCoords));
	color=Rgba::BLUE;
	pos=Vector3(0.f,0.f,0.f);
	verts.push_back(Vertex3(pos,color,texCoords));
	pos=Vector3(0.f,0.f,axisLength);
	verts.push_back(Vertex3(pos,color,texCoords));

	int primitiveType=PRIMITIVE_LINES;
	drawVertexArray3(verts.data(),(int)verts.size(),primitiveType);
	verts.clear();
	enableDepthTesting(false);

	color=Rgba(0xff00001f);
	pos=Vector3(0.f,0.f,0.f);
	verts.push_back(Vertex3(pos,color,texCoords));
	pos=Vector3(axisLength,0.f,0.f);
	verts.push_back(Vertex3(pos,color,texCoords));
	color=Rgba(0x00ff001f);
	pos=Vector3(0.f,0.f,0.f);
	verts.push_back(Vertex3(pos,color,texCoords));
	pos=Vector3(0.f,axisLength,0.f);
	verts.push_back(Vertex3(pos,color,texCoords));
	color=Rgba(0x0000ff1f);
	pos=Vector3(0.f,0.f,0.f);
	verts.push_back(Vertex3(pos,color,texCoords));
	pos=Vector3(0.f,0.f,axisLength);
	verts.push_back(Vertex3(pos,color,texCoords));

	primitiveType=PRIMITIVE_LINES;
	drawVertexArray3(verts.data(),(int)verts.size(),primitiveType);
	enableDepthTesting(true);
	}

void Renderer::drawQuads(const Vertex3* vertexArray,int amtVertices)
	{
	int primitiveType=PRIMITIVE_QUADS;
	drawVertexArray3(vertexArray,amtVertices,primitiveType);
	}

void Renderer::drawQuadsVector(const std::vector<Vertex3> vertexVector)
	{
	int primitiveType=PRIMITIVE_QUADS;
	drawVertexArray3(vertexVector.data(),(int)vertexVector.size(),primitiveType);
	}

void Renderer::drawQuads3D(const std::vector<Vertex3> vertexVector)
	{

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	int dimensionality=3;
	int colordepth=4;
	int stride=sizeof(Vertex3);
// 	int colorstride=sizeof(Vertex3)-sizeof(Rgba);
// 	int texstride=sizeof(Vertex3)-sizeof(Vector2);

	glVertexPointer(dimensionality,GL_FLOAT,stride,&vertexVector[0].m_pos);
	glColorPointer(colordepth,GL_UNSIGNED_BYTE,stride,&vertexVector[0].m_color);
	glTexCoordPointer(2,GL_FLOAT,stride,&vertexVector[0].m_texcoords);

	glDrawArrays(GL_QUADS,0,(int)vertexVector.size());

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	}

void Renderer::drawVertexArray3(const Vertex3* verts,int numVerts,int primitiveType)
	{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	int dimensionality=3;
	int colordepth=4;
	int stride=sizeof(Vertex3);

	glVertexPointer(dimensionality,GL_FLOAT,stride,&verts[0].m_pos);
	glColorPointer(colordepth,GL_UNSIGNED_BYTE,stride,&verts[0].m_color);
	glTexCoordPointer(2,GL_FLOAT,stride,&verts[0].m_texcoords);

	glDrawArrays(primitiveType,0,numVerts);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}

void Renderer::bindTexture(const Texture* texture)
	{
// 	if(texture==m_currentTexture)
// 		return;
	if (texture)
		{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,texture->m_textureID);
		m_currentTexture=texture;
		}
	else
		{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,m_whiteTexture->m_textureID);
		m_currentTexture=m_whiteTexture;
		}
	}

void Renderer::drawBox(const AABB2& box, const Rgba& outlinecolor)
	{
	drawBox(box.m_mins.x, box.m_mins.y, box.m_maxs.x, box.m_maxs.y, outlinecolor);
	}

void Renderer::drawDisc2WithLinearGradient(float x, float y, float radius, int numsides /*= DEFAULT_CIRCLE_SIDE_AMT*/, 
	                                       const Rgba& colorStart /*= Rgba::BLACK*/, const Rgba& colorEnd /*= Rgba::WHITE*/, 
	                                       const LineSegment2& direction /*= LineSegment2(0.f, 0.f, 1.f, 0.f)*/)
	{
	bindTexture(nullptr);
	std::vector<Vertex3> verts;
	Vector2 texCoords(0.f,0.f);
	Rgba color;

	float tx, ty;
	color = calcLinearGradient(direction,Vector2(x,y),colorStart,colorEnd);
	setDrawingColor(color);
	verts.push_back(Vertex3(Vector3(x, y,0.f),color,texCoords));
	float radianStep = (M_PI * 2.f) / (float)numsides;
	float radiansEnd = calcRadiansEndForCircle();
	for (float radians = 0.f; radians <= radiansEnd; radians += radianStep)
		{
		tx = x + radius*(float)cos(radians);
		ty = y + radius*(float)sin(radians);
		color = calcLinearGradient(direction, Vector2(tx, ty), colorStart, colorEnd);
		setDrawingColor(color);
		verts.push_back(Vertex3(Vector3(tx, ty,0.f),color,texCoords));
		}

	int primitiveType=PRIMITIVE_TRIANGLE_FAN;
	drawVertexArray3(verts.data(),(int)verts.size(),primitiveType);
	}

//-----------------------------------------------------------------------------------------------
Texture* Renderer::CreateOrGetTexture(const std::string& imageFilePath)
	{
	// Try to find that texture from those already loaded
	Texture* texture = GetTexture(imageFilePath);
	if (texture)
		return texture;

	texture = CreateTextureFromFile(imageFilePath);
	return texture;
	}

SpriteSheet* Renderer::getSpriteSheetNamed(const std::string& spriteSheetFilePath)
	{
	SpriteSheet* result = nullptr;
	for (std::size_t iSheet = 0; iSheet < m_alreadyLoadedSpriteSheets.size() ; iSheet++)
		{
		SpriteSheet* cursheet = m_alreadyLoadedSpriteSheets.at(iSheet);
		if (cursheet && cursheet->getTextureFilePath()==spriteSheetFilePath)
			{
			result=cursheet;
			break;
			}
		}
	return result;
	}

SpriteSheet* Renderer::createSpriteSheetFromFile(const std::string& spriteSheetFilePath, int amtTilesWide, int amtTilesHigh)
	{
	SpriteSheet* newSpriteSheet=new SpriteSheet(this,spriteSheetFilePath,amtTilesWide,amtTilesHigh);
	if(newSpriteSheet)
		m_alreadyLoadedSpriteSheets.push_back(newSpriteSheet);
	return newSpriteSheet;
	}

SpriteSheet* Renderer::createOrGetSpriteSheet(const std::string& spriteSheetFilePath, int amtTilesWide, int amtTilesHigh)
	{
	SpriteSheet* getResult=getSpriteSheetNamed(spriteSheetFilePath);
	SpriteSheet* result;
	if (getResult)
		result=getResult;
	else
		result=createSpriteSheetFromFile(spriteSheetFilePath,amtTilesWide,amtTilesHigh);
	return result;
	}

Bitmap32Bit* Renderer::getBitmapNamed(const std::string& bitmapFilePath)
	{
	Bitmap32Bit* result=nullptr;
	for (std::size_t ibitmap=0;ibitmap<m_alreadyLoadedBitmaps.size();ibitmap++)
		{
		Bitmap32Bit* bitmap=m_alreadyLoadedBitmaps[ibitmap];
		if(bitmap && bitmap->getFilePath()==bitmapFilePath)
			{
			result=bitmap;
			break;
			}
		}
	return result;
	}

Bitmap32Bit* Renderer::createBitmapFromFile(const std::string& bitmapFilePath)
	{
	Bitmap32Bit* result=nullptr;
	result=new Bitmap32Bit(bitmapFilePath);
	return result;
	}

Bitmap32Bit* Renderer::createOrGetBitmap(const std::string& bitmapFilePath)
	{
	Bitmap32Bit* result=nullptr;
	result=getBitmapNamed(bitmapFilePath);
	if(result)
		return result;
	result=createBitmapFromFile(bitmapFilePath);
	return result;
	}

void Renderer::drawTexturedAABB3(const AABB3& bounds,const Texture& texture,const AABB2& texcoords/*=AABB2(Vector2(0.f,0.f),Vector2(1.f,1.f))*/,const Rgba& tint/*=Rgba::WHITE*/,float /*orientationDegrees*/ /*= 0.f*/)
	{
	std::vector<Vertex3> verts;

	pushGraphicsMatrix();
//	Vector3 center=bounds.CalcCenter();
//	rotateDrawingSurface(center.x,center.y,orientationDegrees);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,texture.m_textureID);
	float texleft = texcoords.m_mins.x;
	float texbottom = texcoords.m_mins.y;
	float texright = texcoords.m_maxs.x;
	float textop = texcoords.m_maxs.y;
	float left = bounds.m_mins.x;
	float top = bounds.m_mins.y;
	float right = bounds.m_maxs.x;
	float bottom = bounds.m_maxs.y;
	float front = bounds.m_mins.z;
	float back = bounds.m_maxs.z;

	Vector3 pos;
	Rgba color;
	Vector2 texCoords;

	color=Rgba(tint);
	texCoords=Vector2(texleft,texbottom);
	pos=Vector3(left,bottom,front);
	verts.push_back(Vertex3(pos,color,texCoords));

	color=Rgba(tint);
	texCoords=Vector2(texright,texbottom);
	pos=Vector3(right,bottom,front);
	verts.push_back(Vertex3(pos,color,texCoords));

	color=Rgba(tint);
	texCoords=Vector2(texright,textop);
	pos=Vector3(right,top,back);
	verts.push_back(Vertex3(pos,color,texCoords));

	color=Rgba(tint);
	texCoords=Vector2(texleft,textop);
	pos=Vector3(left,top,back);
	verts.push_back(Vertex3(pos,color,texCoords));

	int primitiveType=PRIMITIVE_QUADS;
	drawVertexArray3(verts.data(),(int)verts.size(),primitiveType);

	popGraphicsMatrix();
	}

//-----------------------------------------------------------------------------------------------

Texture* Renderer::CreateTextureFromRawData(std::vector<unsigned char>& imageTexelBytes,int width,int height,int bytesPerTexel)
	{
	// Create texture on video card, and send image (texel) data
	unsigned int openGLTextureID = CreateOpenGLTexture(imageTexelBytes.data(),width,height,bytesPerTexel);

	// Create (new) a Texture object
	Texture* texture = new Texture();
	texture->m_textureID = openGLTextureID;
	texture->m_imageFilePath = std::string("GENERATED FROM RAW DATA");
	texture->m_texelDimensions.setXY(width,height);

	m_alreadyLoadedTextures.push_back(texture);
	return texture;
	}

//-----------------------------------------------------------------------------------------------
Texture* Renderer::CreateTextureFromFile(const std::string& imageFilePath)
	{
	// Load image data
	int width = 0;
	int height = 0;
	int bytesPerTexel = 0;
	unsigned char* imageTexelBytes = stbi_load(imageFilePath.c_str(), &width, &height, &bytesPerTexel, 0);
	GUARANTEE_OR_DIE(imageTexelBytes != nullptr, stringf("Failed to load image file \"%s\" - file not found!", imageFilePath.c_str()));
	GUARANTEE_OR_DIE(bytesPerTexel == 3 || bytesPerTexel == 4, stringf("Failed to load image file \"%s\" - image had unsupported %i bytes per texel (must be 3 or 4)", imageFilePath.c_str(), bytesPerTexel));

	// Create texture on video card, and send image (texel) data
	unsigned int openGLTextureID = CreateOpenGLTexture(imageTexelBytes, width, height, bytesPerTexel);
	stbi_image_free(imageTexelBytes);

	// Create (new) a Texture object
	Texture* texture = new Texture();
	texture->m_textureID = openGLTextureID;
	texture->m_imageFilePath = imageFilePath;
	texture->m_texelDimensions.setXY(width, height);

	m_alreadyLoadedTextures.push_back(texture);
	return texture;
	}

//-----------------------------------------------------------------------------------------------
Texture* Renderer::GetTexture(const std::string& imageFilePath)
	{
	for (int textureIndex = 0; textureIndex < (int)m_alreadyLoadedTextures.size(); ++textureIndex)
		{
		Texture* texture = m_alreadyLoadedTextures[textureIndex];
		if (imageFilePath == texture->m_imageFilePath)
			return texture;
		}

	return nullptr;
	}


//-----------------------------------------------------------------------------------------------
unsigned int Renderer::CreateOpenGLTexture(unsigned char* imageTexelBytes, int width, int height, int bytesPerTexel)
	{
	// Create a texture ID (or "name" as OpenGL calls it) for this new texture
	unsigned int openGLTextureID = 0xFFFFFFFF;
	glGenTextures(1, &openGLTextureID);

	// Enable texturing
	glEnable(GL_TEXTURE_2D);

	// Tell OpenGL that our pixel data is single-byte aligned
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Tell OpenGL to bind (set) this as the currently active texture
	glBindTexture(GL_TEXTURE_2D, openGLTextureID);

	// Set texture clamp vs. wrap (repeat)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // one of: GL_CLAMP or GL_REPEAT
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // one of: GL_CLAMP or GL_REPEAT

																  // Set magnification (texel > pixel) and minification (texel < pixel) filters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // one of: GL_NEAREST, GL_LINEAR
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);  // one of: GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_LINEAR

	GLenum bufferFormat = GL_RGBA; // the format our source pixel data is currently in; any of: GL_RGB, GL_RGBA, GL_LUMINANCE, GL_LUMINANCE_ALPHA, ...
	if (bytesPerTexel == 3)
		bufferFormat = GL_RGB;

	GLenum internalFormat = bufferFormat; // the format we want the texture to me on the card; allows us to translate into a different texture format as we upload to OpenGL

	glTexImage2D(			// Upload this pixel data to our new OpenGL texture
		GL_TEXTURE_2D,		// Creating this as a 2d texture
		0,					// Which mipmap level to use as the "root" (0 = the highest-quality, full-res image), if mipmaps are enabled
		internalFormat,		// Type of texel format we want OpenGL to use for this texture internally on the video card
		width,				// Texel-width of image; for maximum compatibility, use 2^N + 2^B, where N is some integer in the range [3,10], and B is the border thickness [0,1]
		height,				// Texel-height of image; for maximum compatibility, use 2^M + 2^B, where M is some integer in the range [3,10], and B is the border thickness [0,1]
		0,					// Border size, in texels (must be 0 or 1)
		bufferFormat,		// Pixel format describing the composition of the pixel data in buffer
		GL_UNSIGNED_BYTE,	// Pixel color components are unsigned bytes (one byte per color/alpha channel)
		imageTexelBytes);	// Location of the actual pixel data bytes/buffer

	glHint( GL_GENERATE_MIPMAP_HINT, GL_NICEST );
	glGenerateMipmap( GL_TEXTURE_2D );
	return openGLTextureID;
	}

void Renderer::drawLiquidDisc2(float x, float y, float radius, int amtedges, const Rgba& color)
	{
	bindTexture(nullptr);
	std::vector<Vertex3> verts;
	Vector2 texCoords(0.f,0.f);

	const float BLUR_RADIUS_FACTOR=3.f;
	Rgba edgecolor=color;
	edgecolor.setAlphaAsFloat(0.f);
// 	beginBasicShape(GL_TRIANGLE_FAN);
	{
	float tx, ty;
	setDrawingColor(color);
	verts.push_back(Vertex3(Vector3(x,y,0.f),color,texCoords));
	float radianStep = (M_PI * 2.f) / (float)amtedges;
	float radiansEnd = calcRadiansEndForCircle();
	for (float radians = 0.f; radians <= radiansEnd; radians += radianStep)
		{
		setDrawingColor(edgecolor);
		tx = x + radius*BLUR_RADIUS_FACTOR*(float)cos(radians);
		ty = y + radius*BLUR_RADIUS_FACTOR*(float)sin(radians);
		verts.push_back(Vertex3(Vector3(tx,ty,0.f),color,texCoords));
		}
	}
	int primitiveType=PRIMITIVE_TRIANGLE_FAN;
	drawVertexArray3(verts.data(),(int)verts.size(),primitiveType);
	}

