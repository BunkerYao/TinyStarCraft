
/** Multi-Rendertargets output structure for Gbuffer-Filling pixel shaders. */
struct GbufferOutput
{
	float4 c0 : COLOR0; // RGB for albedo, alpha for emission
	float4 c1 : COLOR1; // RGB for world space normals
	float4 c2 : COLOR2; // RF16 for world space height
	float h : DEPTH; // World space height
};

shared float    _gTime;
shared float4x3 _gViewProjMatrix;
shared float4   _gViewPoint;
shared float2   _gRenderTargetInvSize;
shared texture  _gGbuffer0;
shared texture  _gGbuffer1;
shared texture  _gHeightBuffer;
shared texture  _gRefraction;

/** 
 *	Matrix that transform vertex from clip space to normalized viewport space in which the 
 *  x, y are the u, v component of viewport texture coordinate.
 */
static const float4x3 VIEWPORT_TEXCOORD_MATRIX =
{
    { 0.5f,  0.0f,  0.0f },
    { 0.0f, -0.5f,  0.0f },
    { 0.0f,  0.0f,  0.0f },
    { 0.5f,  0.5f,  0.0f }
};

/** Sun light direction */
static const float3 SUN_LIGHT_DIR = { 0.0f, -0.707f, 0.707f };

/** View direction */
static const float3 VIEW_DIR = { -0.61237f, -0.5f, -0.61237f };

/** Adjust a vertex in clip space to compensate half pixel offset */
#define CLIP_SPACE_HALF_PIXEL_ADJUST(v) (v) + float4(_gRenderTargetInvSize * (v).w, 0.0f, 0.0f)

/** Transform a vertex from world space to clip space(Projection). */
#define WORLD_TO_CLIP(v) CLIP_SPACE_HALF_PIXEL_ADJUST(float4(mul((v), _gViewProjMatrix), 1.0f))

/** Calculate viewport texcoord from vertex's clip space position. */
#define CLIP_TO_VIEWPORT_TEXCOORD(v) mul((v) / (v).w, VIEWPORT_TEXCOORD_MATRIX).xy

/** Maximum height of the world */
#define MAX_WORLD_HEIGHT 1024.0f;
