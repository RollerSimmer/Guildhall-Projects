

#pragma once
#include "Engine/Core/EngineBase.hpp"
#include "Engine/Renderer/RHI/RHIOutputMode.hpp"
#include "Engine/Renderer/RgbaFloat.hpp"
#include "Engine/Math/Matrix4.hpp"
#include "Engine/Gui/Window.hpp"
#include "Engine/Renderer/BlendState.hpp"
#include "Engine/Core/Types.hpp"

class ShaderProgram;
class RHITexture2D;
class RgbaFloat;
class Sampler;
class ConstantBuffer;
class Vertex3F;

class RHIDevice;
class RHIDeviceContext;
class RHIOutput;
// enum RHIOutputMode;
class VertexBuffer;
class IndexBuffer;
class Vector2;
class RasterState;
class FontDx;

enum {
	MATRIX_BUFFER_INDEX=0,
	TIME_BUFFER_INDEX
	};

struct MatrixBuffer
	{
	Matrix4 m_model;
	Matrix4 m_view;
	Matrix4 m_projection;
	};

struct TimeBuffer
	{
	float m_gameTime;
	float m_systemTime;
	float m_gameFrameTime;
	float m_systemFrameTime;
	};

struct BlendStateStruct
	{
	bool m_enabled;
	BlendFactor m_srcFactor;
	BlendFactor m_dstFactor;
	};


enum PrimitiveType: unsigned int
	{
	DX_PRIMITIVE_NONE,
	DX_PRIMITIVE_POINTS,
	DX_PRIMITIVE_LINES,
	DX_PRIMITIVE_TRIANGLES,
	};


class SimpleRendererDx
	{
	public:
		SimpleRendererDx();
// 		SimpleRendererDx(RHIDevice* rhiDevice,RHIOutput* rhiOutput,
// 						 RHIDeviceContext* rhiDeviceContext,RHITexture2D* renderTarget);
		~SimpleRendererDx();
		void setup(WindowMessageHandlerCallback msgProc, const std::string& windowTitle, unsigned int left, unsigned int top, unsigned int width, unsigned int height);
		void setTitle(char const *newTitle);
		void destroy();
		void processMessages();
		bool isClosed() const;

		//-------------------------------------------------------

		void present();
		void close();

		RHITexture2D* getRenderTarget();
		float getWidth() const;
		float getHeight() const;
		float getAspectRatio() const;

// 		// [OPTIONAL]
		bool setDisplayMode(RHIOutputMode mode,float  width = 0.0f,float  height = 0.0f);

		void createRenderTarget();

		// Setting nullptr should default to the output. 
		void setRenderTarget(RHITexture2D *color_target);

		// [A02]
		void setViewport(unsigned int x,unsigned int y,unsigned int width,unsigned int height);

		// [A02: Optional]
		// Set the viewport using percentages of the size of the current render target.
		void setViewportAsPct(float x,float y,float w,float h);


		// [A02] CLEARING 
		// Clears currently bound target
		void clearColor(const RgbaFloat& color);

		// Clears specified target
		void clearTargetColor(const RgbaFloat& color);

		// [A02] CLEARING 
		// Calls Outputs Present

		// [A02]
		void setShader(ShaderProgram* program);

		// [A02]
		void setTexture(unsigned int textureIndex,RHITexture2D *texture);


		void setTexture(RHITexture2D *texture);

	// [A02]
		inline void setSampler(Sampler *SamplerDx)  { setSampler(0,SamplerDx); }

		void setSampler(unsigned int sampler_index,Sampler *sampler);

		void setConstantBuffer(int index, ConstantBuffer* cb);

		void enableBlend(BlendFactor src,BlendFactor dst);
		void disableBlend();


	// [A02]
	// Indexed is optional - though highly encouraged to do 
	// during this assignment as it is the easiest assignment to 
	// implement them with
		void drawIndexed(PrimitiveType topology,VertexBuffer *vbo,IndexBuffer *ibo,unsigned int const vertexCount);

		void draw(PrimitiveType topology, VertexBuffer *vbo, unsigned int const vertexCount);


		void drawQuad2D(const Vector2& lowerLeft, const Vector2& topRight, const Vector2& uvMins, const Vector2& uvMaxs, const RgbaFloat& color=RgbaFloat(1.f, 1.f, 1.f, 1.f));

		void setOrthoProjection(const Vector2& bottomLeft, const Vector2& topRight);
		void setProjectionMatrix(const Matrix4& proj);
		void drawVertexArray(PrimitiveType topology, const Vertex3F* vertices, uint count);

		void drawText2D(const Vector2& pos,float scale,const RgbaFloat& color,const FontDx& font,const std::string& text);

	public:
		RHIDevice *m_rhiDevice;
		RHIDeviceContext *m_rhiContext;
		RHIOutput *m_rhiOutput;

		RHITexture2D *m_currentTarget;

		MatrixBuffer m_matrixData;
		ConstantBuffer* m_matrixCB;

		TimeBuffer m_timeData;
		ConstantBuffer* m_timeCB;

		VertexBuffer *m_tempVbo;

		BlendStateStruct m_blendState;
		BlendState *m_currentBlendState;

		RasterState* m_defaultRasterState;


	};