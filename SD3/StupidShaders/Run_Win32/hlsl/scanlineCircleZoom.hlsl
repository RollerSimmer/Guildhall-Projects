Texture2D <float4> tImage : register(t0);


SamplerState sSampler : register(s0);

struct vertex_in_t
{
   float3 position : POSITION;
   float2 uv : TEXCOORDS; 
};

struct vertex_to_fragment_t
{
   float4 position : SV_Position; 
   float2 uv : TEXCOORDS;
};

cbuffer zoom_cb : register(b1) 
{
   float ZOOM;
   float TIME;
   float2 PADDING;
};



float distFromCircle(float x,float y,float radius)
	{
	float xc=x-0.5f;
	float yc=y-0.5f;

	float distFromCenter=xc*xc+yc*yc;
	float result=distFromCenter-radius*radius;
	return result;
	}

float4 circleEdgeDiffuse()
	{
	return float4(1.f,0.f,0.f,1.f);
	}

float4 circleDiffuse(float x,float y)
	{
	float scale=1.f/ZOOM;
	float xc=x-0.5;
	float yc=y-0.5f;
	float xScaled=xc*scale;
	float yScaled=yc*scale;
	float xSampled=xScaled+0.5f;
	float ySampled=yScaled+0.5f;
	float4 result=tImage.Sample(sSampler,float2(xSampled,ySampled));
	return result;
	}

float4 outsideDiffuse(float x,float y)
	{
	float4 timeDispFactor=0.1f;
	float4 yDisp=TIME*timeDispFactor;
	float4 result=0.f;
	float amtScanlines=100.f;
	bool shouldDrawScanline=trunc(abs((y-yDisp)*amtScanlines))%2==1;
	if(shouldDrawScanline)
		result=tImage.Sample(sSampler,float2(x,y));
	return result;
	}


/* YES */

// NOP VERTEX FUNCTION
vertex_to_fragment_t VertexFunction( vertex_in_t vertex ) 
	{
	vertex_to_fragment_t out_data = (vertex_to_fragment_t)0;
	float x=vertex.position.x;
	float y=vertex.position.y;
	float z=vertex.position.z;   
	out_data.position = float4( x,y,z,1.0f);
	out_data.uv = vertex.uv;
	return out_data;
	}

// COLOR (PIXEL/FRAGMENT) FUNCTION
float4 FragmentFunction( vertex_to_fragment_t data ) : SV_Target0
	{
	float tileScale=5.f;
	float x = data.uv.x;
	float y = data.uv.y; 

	float4 diffuse=0.f;
	float delta=0.001f;
	const float radius = 0.33f;
	float dist=distFromCircle(x,y,radius);
	if(abs(dist)<delta)
		return  circleEdgeDiffuse();
	else if(dist>0.f)
		return outsideDiffuse(x,y);
	else 
		return circleDiffuse(x,y);

	return diffuse;
	}



//raster state
