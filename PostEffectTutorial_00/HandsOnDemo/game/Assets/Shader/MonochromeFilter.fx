/*!
 * @brief	モノクロフィルター。
 */

struct VS_INPUT{
	float4		pos		: POSITION;
	float2		uv		: TEXCOORD0;
};

struct VS_OUTPUT{
	float4		pos		: POSITION;
	float2		uv		: TEXCOORD0;
};

texture g_tex;
sampler TextureSampler = 
sampler_state
{
    Texture = <g_tex>;
    MipFilter = NONE;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};


VS_OUTPUT VSMain( VS_INPUT In )
{
	VS_OUTPUT Out;
	Out.pos = In.pos;
	Out.uv	= In.uv;
	return Out;
}
float4 PSMain( VS_OUTPUT In ) : COLOR0
{
	float4 color = tex2D( TextureSampler, In.uv );
	float Y  =  0.29900 * color.r + 0.58700 * color.b + 0.11400 * color.b;
	return Y;
}

technique Default
{
	pass p0
	{
		VertexShader 	= compile vs_3_0 VSMain();
		PixelShader		= compile ps_3_0 PSMain();
	}
}
