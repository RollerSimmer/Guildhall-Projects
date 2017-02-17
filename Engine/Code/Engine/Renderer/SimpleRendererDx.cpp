
#include "Engine/Renderer/SimpleRendererDx.hpp"
#include "Engine/Renderer/RgbaFloat.hpp"
#include "Engine/Renderer/RHI/RHIOutput.hpp"
#include "Engine/Renderer/RHI/RHIDeviceContext.hpp"
#include "Engine/Renderer/RHI/ShaderProgram.hpp"
#include "Engine/Renderer/RHI/VertexBuffer.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Gui/Window.hpp"
#include "Engine/Renderer/RHI/RasterState.hpp"
#include "Engine/Renderer/RHI/RHITexture2D.hpp"
#include "Engine/Renderer/RHI/RHIInstance.hpp"
#include "Engine/Renderer/RHI/ConstantBuffer.hpp"
#include "Engine/Core/Types.hpp"
#include "Engine/Renderer/Vertex3.hpp"


SimpleRendererDx::SimpleRendererDx()
	: m_rhiDevice(nullptr)
	, m_rhiOutput(nullptr)
	, m_rhiContext(nullptr)
	, m_currentTarget(nullptr)
	, m_matrixCB(nullptr)
	, m_timeCB(nullptr)
	, m_tempVbo(nullptr)
	, m_currentBlendState(nullptr)
	{
	
	}

SimpleRendererDx::~SimpleRendererDx()
	{

	}

void SimpleRendererDx::setup(WindowMessageHandlerCallback msgProc, const std::string& windowTitle, unsigned int left, unsigned int top, unsigned int width,unsigned int height)
	{
	RHIInstance* ri=RHIInstance::getInstance();
	ri->createOutput(msgProc, m_rhiContext, m_rhiOutput, windowTitle, left, top, width, height);

	m_rhiDevice=m_rhiContext->m_device;
	m_rhiOutput->m_window->show();
	m_rhiOutput->m_window->setTitle(windowTitle);

	setRenderTarget(nullptr);

	m_defaultRasterState=new RasterState(m_rhiDevice);
	m_rhiContext->setRasterState(m_defaultRasterState);

	memZero(&m_timeData);

	m_matrixCB=new ConstantBuffer(m_rhiDevice,&m_matrixData,sizeof(m_matrixData));
	m_timeCB=new ConstantBuffer(m_rhiDevice,&m_timeData,sizeof(m_timeData));

	setConstantBuffer(MATRIX_BUFFER_INDEX,m_matrixCB);
	setConstantBuffer(TIME_BUFFER_INDEX,m_timeCB);

	m_tempVbo=new VertexBuffer(m_rhiDevice,nullptr,32,BUFFERUSAGE_DYNAMIC);
	}

void SimpleRendererDx::setTitle(char const *newTitle)
	{

	}

void SimpleRendererDx::destroy()
	{
	SAFE_DELETE(m_currentBlendState);
	SAFE_DELETE(m_defaultRasterState);
	SAFE_DELETE(m_matrixCB);
	SAFE_DELETE(m_timeCB);
	SAFE_DELETE(m_tempVbo);

	delete m_rhiOutput;
	delete m_rhiContext;
	delete m_rhiDevice;
	}

void SimpleRendererDx::processMessages()
	{

	}

bool SimpleRendererDx::isClosed() const
	{
	return m_rhiOutput->m_window->isClosed();
	}

void SimpleRendererDx::createRenderTarget()
	{

	}

void SimpleRendererDx::setRenderTarget(RHITexture2D *color_target)
	{
	if (color_target != nullptr) {
		m_currentTarget = color_target;
		}
	else {
		m_currentTarget = m_rhiOutput->getRenderTarget();
		}

	m_rhiContext->m_dxContext->OMSetRenderTargets(1,
		&m_currentTarget->m_dxRtv,
		nullptr);
	}

void SimpleRendererDx::setViewport(unsigned int x,unsigned int y,unsigned int width,unsigned int height)
	{
	// Also, set which region of the screen we're rendering to, in this case, all of it 
	D3D11_VIEWPORT viewport;
	memset(&viewport,0,sizeof(viewport));
	viewport.TopLeftX = (FLOAT)x;
	viewport.TopLeftY = (FLOAT)y;
	viewport.Width = (FLOAT)width;
	viewport.Height = (FLOAT)height;
	viewport.MinDepth = 0.0f;        // must be between 0 and 1 (defualt is 0);
	viewport.MaxDepth = 1.0f;        // must be between 0 and 1 (default is 1)

	m_rhiContext->m_dxContext->RSSetViewports(1,&viewport);
	}

void SimpleRendererDx::setViewportAsPct(float x,float y,float w,float h)
	{
	UNUSED(x);
	UNUSED(y);
	UNUSED(w);
	UNUSED(h);
	}

void SimpleRendererDx::clearColor(const RgbaFloat& color)
	{
	clearTargetColor(color);
	}

void SimpleRendererDx::clearTargetColor(const RgbaFloat& color)
	{

	m_rhiContext->clearColorTarget(this->m_currentTarget,color);
	}

void SimpleRendererDx::present()
	{
	m_rhiOutput->present();
	}

void SimpleRendererDx::close()
	{
	m_rhiOutput->close();
	}

RHITexture2D* SimpleRendererDx::getRenderTarget()
	{
	return m_rhiOutput->getRenderTarget();
	}

float SimpleRendererDx::getWidth() const
	{
	return m_rhiOutput->getWidth();
	}

float SimpleRendererDx::getHeight() const
	{
	return m_rhiOutput->getHeight();
	}

float SimpleRendererDx::getAspectRatio() const
	{
	return m_rhiOutput->getAspectRatio();
	}

bool SimpleRendererDx::setDisplayMode(RHIOutputMode mode,float width /*= 0.0f*/,float height /*= 0.0f*/)
	{
	return m_rhiOutput->setDisplayMode(mode,width,height);
	}

void SimpleRendererDx::setShader(ShaderProgram *program)
	{
	m_rhiContext->m_dxContext->VSSetShader(program->m_dxVertexShader,nullptr,0U);
	m_rhiContext->m_dxContext->PSSetShader(program->m_dxFragmentShader,nullptr,0U);
	m_rhiContext->m_dxContext->IASetInputLayout(program->m_dxInputLayout);
	}

void SimpleRendererDx::setTexture(unsigned int textureIndex,RHITexture2D *texture)
	{
	m_rhiContext->setTexture(textureIndex,texture);
	}

void SimpleRendererDx::setTexture(RHITexture2D *texture)
	{
	setTexture(0,texture);
	}

void SimpleRendererDx::setSampler(unsigned int samplerIndex,Sampler *SamplerDx)
	{
	m_rhiContext->setSampler(samplerIndex,SamplerDx);
	}


void SimpleRendererDx::drawIndexed(PrimitiveType topology,VertexBuffer *vbo,IndexBuffer *ibo,unsigned int const vertexCount)
	{
	UNUSED(topology);
	UNUSED(vbo);
	UNUSED(ibo);
	UNUSED(vertexCount);
	}

void SimpleRendererDx::draw(PrimitiveType topology,VertexBuffer *vbo,unsigned int const vertex_count)
	{
	D3D11_PRIMITIVE_TOPOLOGY d3dPrim;
	switch (topology)
		{
		case DX_PRIMITIVE_TRIANGLES:
		default:
			d3dPrim = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		}

	m_rhiContext->m_dxContext->IASetPrimitiveTopology(d3dPrim);
	unsigned int stride = sizeof(Vertex3F);
	unsigned int offsets = 0;
	m_rhiContext->m_dxContext->IASetVertexBuffers(0,1,&vbo->m_dxBuffer,&stride,&offsets);

	m_rhiContext->draw(vertex_count,0);
	}

void SimpleRendererDx::drawQuad2D(const Vector2& bl, const Vector2& tr, const Vector2& uvMins, const Vector2& uvMaxs, const RgbaFloat& color/*=RgbaFloat(1.f, 1.f, 1.f, 1.f)*/)
	{
	RgbaFloat colorCopy=color;

	Vertex3F vertices[] ={
		Vertex3F(Vector3(bl.x, bl.y, 0.0f), colorCopy, Vector2(uvMins.x, uvMaxs.y)),
		Vertex3F(Vector3(tr.x, tr.y, 0.0f), colorCopy, Vector2(uvMaxs.x, uvMins.y)),
		Vertex3F(Vector3(bl.x, tr.y, 0.0f), colorCopy, Vector2(uvMins.x, uvMins.y)),
		Vertex3F(Vector3(bl.x, bl.y, 0.0f), colorCopy, Vector2(uvMins.x, uvMaxs.y)),
		Vertex3F(Vector3(tr.x, bl.y, 0.0f), colorCopy, Vector2(uvMaxs.x, uvMaxs.y)),
		Vertex3F(Vector3(tr.x, tr.y, 0.0f), colorCopy, Vector2(uvMaxs.x, uvMins.y)),
		};

	drawVertexArray(DX_PRIMITIVE_TRIANGLES, vertices, 6);
	}

void SimpleRendererDx::setOrthoProjection(const Vector2& bottomLeft, const Vector2& topRight)
	{

	Matrix4 proj=Matrix4::makeOrthoProjection(bottomLeft.x,topRight.x,
		bottomLeft.y,topRight.y,0.f,1.f);

	setProjectionMatrix(proj);
	}

void SimpleRendererDx::setProjectionMatrix(const Matrix4& proj)
	{
	m_matrixData.m_projection=proj.getTransposed();
	m_matrixCB->update(m_rhiContext,&m_matrixData);
	}

void SimpleRendererDx::drawVertexArray(PrimitiveType topology, const Vertex3F* vertices, uint count)
	{
	m_tempVbo->update(m_rhiContext,vertices,count);
	draw(topology,m_tempVbo,count);
	}

void SimpleRendererDx::drawText2D(const Vector2& pos,float scale,const RgbaFloat& color,const FontDx& font,const std::string& text)
	{
	//TODO: implement this
	}

void SimpleRendererDx::setConstantBuffer(int index, ConstantBuffer* cb)
	{
	m_rhiContext->m_dxContext->VSSetConstantBuffers(index, 1, &cb->m_dxBuffer);
	m_rhiContext->m_dxContext->PSSetConstantBuffers(index, 1, &cb->m_dxBuffer);
	}

void SimpleRendererDx::enableBlend(BlendFactor src, BlendFactor dst)
	{
	if(m_currentBlendState!=nullptr)
		{
		if((m_blendState.m_enabled==true)
			&& (m_blendState.m_srcFactor)
			&& (m_blendState.m_dstFactor))
			{
			return;
			}
		}

	SAFE_DELETE(m_currentBlendState);

	BlendState* bs=new BlendState(m_rhiDevice,true,src,dst);
	m_rhiContext->setBlendState(bs);

	m_blendState.m_enabled=true;
	m_blendState.m_srcFactor=src;
	m_blendState.m_dstFactor=dst;

	m_currentBlendState = bs;
	}

void SimpleRendererDx::disableBlend()
	{
	if (m_blendState.m_enabled)
		return;
	}
