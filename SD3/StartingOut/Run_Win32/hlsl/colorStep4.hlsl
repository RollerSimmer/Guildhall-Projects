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
   // output red
   
   float x = data.uv.x;
   float y = data.uv.y;
   float newx=x;
   float newy=y;
   
      
   float4 diffuse = tImage.Sample( sSampler, float2(newx, newy) );
   float amtSteps = 4.f;
   diffuse.x=round(diffuse.x*amtSteps)/amtSteps;
   diffuse.y=round(diffuse.y*amtSteps)/amtSteps;
   diffuse.z=round(diffuse.z*amtSteps)/amtSteps;
   
   return diffuse;
}



//raster state
