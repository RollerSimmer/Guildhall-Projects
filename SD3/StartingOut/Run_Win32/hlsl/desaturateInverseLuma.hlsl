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

float calcLuma(float4 color)
	{
	float r=color.x;
	float g=color.y;
	float b=color.z;
	return 0.3f*r + 0.6f*g + 0.1f*b;
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
	float x = data.uv.x;
	float y = data.uv.y;   
	float luma     = calcLuma(tImage.Sample(sSampler, float2(x,y)));
	float invLuma=1.f-luma;
	float4 diffuse = float4(1.f,1.f,1.f,1.f);
	diffuse.x*=invLuma;
	diffuse.y*=invLuma;
	diffuse.z*=invLuma;
	return diffuse;
	}



//raster state
