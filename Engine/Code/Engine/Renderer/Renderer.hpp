#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
// #include <gl/GLU.h>
#include <string>
#include <vector>

#include "Engine/Math/Vector2.hpp"

const int AMOUNT_CIRCLE_SIDES = 64;

#include "Engine/Renderer/Rgba.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/LineSegment2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/Bitmap32Bit.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/LineSegment3.hpp"
#include "Engine/Math/Snowman.hpp"
#include "Engine/Math/Matrix4.hpp"
// #include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Vertex3.hpp"

const int DEFAULT_CIRCLE_SIDE_AMT = 64;
const int AMT_FONT_SHEET_TILES_X = 16;
const int AMT_FONT_SHEET_TILES_Y = AMT_FONT_SHEET_TILES_X;

extern const int PRIMITIVE_POINTS; 
extern const int PRIMITIVE_LINES;
extern const int PRIMITIVE_LINE_LOOP; 
extern const int PRIMITIVE_TRIANGLES; 
extern const int PRIMITIVE_QUADS;
extern const int PRIMITIVE_TRIANGLE_FAN;

extern const int GFXSTATE_BLEND;
extern const int BLENDMODE_SRC_ALPHA;
extern const int BLENDMODE_ONE_MINUS_SRC_ALPHA;
extern const int BLENDMODE_SMOOTH;

class Texture;
class AABB2;
class BitmapFont;

struct Vertex2
	{
	Vector2 m_pos;
	Rgba m_color;
	Vector2 m_texcoords;
	Vertex2(const Vector2& pos,const Rgba& color,const Vector2& texcoords): m_pos(pos),m_color(color),m_texcoords(texcoords)
		{}
	};


class Renderer
	{
	public: // constants

	public:
		Renderer();

		static float calcRadiansEndForCircle();
	
		void drawLineLoop(std::vector<Vertex3>& verts,float thickness=2.f);
		void drawLine(float startx, float starty, float endx, float endy, const Rgba& color = Rgba::WHITE, float thickness=2.f);
		void drawLine(const LineSegment2& line,const Rgba& color=Rgba::WHITE,float thickness=2.f);
		void drawLine3(const LineSegment3& line,const Rgba& color=Rgba::WHITE,float thickness=2.f);
		void clearScreen(float red, float green, float blue);
		void clearScreen(const Rgba& color);
		void clearDepth();
		void clearScreenAndDepth(const Rgba& color);

		void rotate3D(float angleDegrees,const Vector3& axis);
		void lookAt(const Vector3& fromPos,const Vector3& toPos,const Vector3& up);

		void rotateDrawingSurface(float xcenter, float ycenter,float angleDegrees);
		void drawRotatedRegularPolygon(float x, float y, int amtsides, float radius, float orientationDegrees, const Rgba& color, float lineThickness=2.f);
		void drawStarFivePointed(float x, float y, float radius, float orientationDegrees, const Rgba& color, float lineThickness=2.f);
		void drawRegularPolygon(float x, float y, int amtsides, float radius, const Rgba& color = Rgba::WHITE, float lineThickness=2.f);
		void drawCircle(float x, float y, float radius, int numsides,const Rgba& color=Rgba::WHITE, float lineThickness=2.f);
		void drawFilledDisc2(float x,float y, float radius,int numsides,const Rgba& color= Rgba::WHITE);
		void drawDisc2WithLinearGradient(float x, float y, float radius, int numsides = DEFAULT_CIRCLE_SIDE_AMT, const Rgba& colorStart = Rgba::BLACK,
			                             const Rgba& colorEnd = Rgba::WHITE, const LineSegment2& direction = LineSegment2(0.f, 0.f, 1.f, 0.f));
		void drawLiquidDisc2(float x, float y, float radius, int amtedges, const Rgba& color);
		void drawShinyDisc2(float x,float y,float radius,int numsides,const Rgba color=Rgba::GRAY);
		void drawBox(float left,float top,float right,float bottom, const Rgba& color= Rgba::WHITE, float lineThickness=2.f);
		void drawBox(const AABB2& bounds, const Rgba& outlinecolor);
		void drawBox(const AABB2& box,const Rgba& color,float borderThickness);
		void drawFilledBox(const AABB2& box,const Rgba& color);
		void drawFilledBox3(const AABB3& box,const Rgba& color);
			void drawFilledQuad(const Vector3& A,const Vector3& B,const Vector3& C,const Vector3& D,const Rgba& color);
		void drawWireframeSphere3(const Sphere3& sphere,const Rgba& color=Rgba::WHITE, float lineThickness=1.f);
		void drawWireframeSnowman(const Snowman& snowman,const Rgba& color=Rgba::WHITE, float lineThickness=1.f);

		void drawOffsetRotatedRegularPolygon(float x,float y,int amtsides,float radius,float orientationDegrees,const Vector2* vertexOffsets, const Rgba& color= Rgba::WHITE, float lineThickness=2.f);
		void setDrawingColor(Rgba color= Rgba::WHITE);
		void drawNGon(const Vector2& pos, float orientationDegrees, const Vector2* vertices, int amtVertices, const Rgba& color, float lineThickness=2.f);
		
		void pushGraphicsMatrix();
		void popGraphicsMatrix();
		Matrix4 getCurrentModelViewMatrix();

		void setLineWidth(float width);
		void setOrthoProjection(const Vector2& mins, const Vector2& maxs);
		void setOrthoProjection(const AABB2& viewport);
		void setPerspectiveProjection(float fovVerticalDegrees,float aspectRatio, float nearDistance, float FarDistance);
		void blendFunc(int sfactor,int dfactor);
		void enable(int cap);		
		void disable(int cap);

		void scale3D(const Vector3& scalingFactors);
		void translate2D(const Vector2& displacement);
		void translate3D(const Vector3& displacement);

		void loadGraphicsMatrix(const Matrix4& view);


		Rgba calcLinearGradient(const LineSegment2& direction, const Vector2& point, const Rgba& colorStart, const Rgba& colorEnd);
		Texture* CreateTextureFromFile(const std::string& imageFilePath);
		Texture* GetTexture(const std::string& imageFilePath);
		unsigned int CreateOpenGLTexture(unsigned char* imageTexelBytes, int width, int height, int bytesPerTexel);
		Texture* Renderer::CreateTextureFromRawData(std::vector<unsigned char>& imageTexelBytes,int width,int height,int bytesPerTexel);
		Texture* CreateOrGetTexture(const std::string& imageFilePath);
		void bindTexture(const Texture* texture);
		
		unsigned int getBoundedVbo(const std::vector<Vertex3>& verts);
		void unbindBuffer(unsigned int& vboID);
		void drawVbo(unsigned int m_vboID,int amtVerticesInVbo, int primitiveType=PRIMITIVE_QUADS);

		SpriteSheet* getSpriteSheetNamed(const std::string& spriteSheetFilePath);
		SpriteSheet* createSpriteSheetFromFile(const std::string& spriteSheetFilePath, int amtTilesWide, int amtTilesHigh);
		SpriteSheet* createOrGetSpriteSheet(const std::string& spriteSheetFilePath, int amtTilesWide, int amtTilesHigh);

		Bitmap32Bit* getBitmapNamed(const std::string& bitmapFilePath);
		Bitmap32Bit* createBitmapFromFile(const std::string& bitmapFilePath);
		Bitmap32Bit* createOrGetBitmap(const std::string& bitmapFilePath);

		void drawTexturedAABB3(const AABB3& bounds,const Texture& texture,const AABB2& texcoords=AABB2(Vector2(0.f,0.f),Vector2(1.f,1.f)),const Rgba& tint=Rgba::WHITE,float orientationDegrees = 0.f);
		void drawTexturedAABB2(const AABB2& bounds, const Texture& texture, const AABB2& texcoords, const Rgba& tint, float orientationDegrees = 0.f);
		void drawTexturedAABB2(const AABB2& bounds, const Texture& texture, const Vector2& texCoordsAtMins,
			const Vector2& texCoordsAtMaxs, const Rgba& tint, float orientationDegrees = 0.f);
		void drawBoxWithOutline(const AABB2& box,const Rgba& fillcolor, const Rgba& outlinecolor);
		
		void drawTextLineCentered(const BitmapFont* theFont, const std::string& text, const Vector2& bottomCenterStartPos, float fontHeight,float aspect, const Rgba& color=Rgba::WHITE);
		void drawTextLine(const BitmapFont* theFont, const std::string& text, const Vector2& bottomLeftStartPos, float fontHeight, float aspect, const Rgba& color=Rgba::WHITE);
		void drawTextLines(const BitmapFont* theFont, const std::string& text, const Vector2& bottomLeftStartPos, float fontHeight, float aspect, const Rgba& color=Rgba::WHITE,bool isCentered=true);
		void drawButtonTextLine(const BitmapFont* theFont, const std::string& text, const Vector2& bottomLeftStartPos, float fontHeight, float aspect, const Rgba& color = Rgba::WHITE,
			const Rgba& shadowcolor = Rgba(0x0000007F), const Vector2& shadowDisplacement = Vector2(1.f, 1.f));
		void drawTextLineWithShadow(const BitmapFont* theFont, const std::string& text, const Vector2& bottomLeftStartPos, float fontHeight, float aspect, const Rgba& color = Rgba::WHITE,
			const Rgba& shadowcolor = Rgba(0x0000007F), const Vector2& shadowDisplacement = Vector2(1.f, 1.f));

		void drawDots(const std::vector<Vector3>& dotPoints, const Rgba& color, float width);

		void drawAxes(float axisLength);

		void drawQuads(const Vertex3* vertexArray, int amtVertices);
		void drawQuadsVector(const std::vector<Vertex3> vertexVector);
		void drawQuads3D(const std::vector<Vertex3> vertexVector);

		void drawVertexArray3( const Vertex3* verts, int numVerts, int primitiveType );
	
		void enableBackFaceCulling(bool isNowBackfaceCulling);
		void enableDepthTesting(bool isNoewDepthTesting);

		void initWhiteTexture();
	private:

		std::vector< Texture* >		m_alreadyLoadedTextures;
		std::vector< SpriteSheet* > m_alreadyLoadedSpriteSheets;
		std::vector< Bitmap32Bit* > m_alreadyLoadedBitmaps;
		const Texture* m_currentTexture;
		Texture* m_whiteTexture;
	public:
	};

