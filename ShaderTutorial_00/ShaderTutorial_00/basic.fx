/*!
 *@brief	単純な頂点シェーダー。
 */


struct VS_INPUT{
	float4	pos		: POSITION;
	float4	color	: COLOR0;
};

struct VS_OUTPUT{
	float4	pos		: POSITION;
	float4	color	: COLOR0;
};

/*!
 *@brief	頂点シェーダー。
 */
VS_OUTPUT VSMain( VS_INPUT In )
{
	VS_OUTPUT Out;
	Out.pos = In.pos;
	Out.color = In.color;
	return Out;
}
/*!
 *@brief	ピクセルシェーダー。
 */
float4 PSMain( VS_OUTPUT In ) : COLOR
{
	return In.color;
}


/*!
 *@brief	頂点シェーダー。
 */
VS_OUTPUT VSMain2( VS_INPUT In )
{
	VS_OUTPUT Out;
	Out.pos = In.pos;
	Out.color = In.color;
	return Out;
}
/*!
 *@brief	頂点シェーダー。
 */
float4 PSMain2( VS_OUTPUT In ) : COLOR
{
	return float4(1.0f, 0.0f, 0.0f, 1.0f);
}

technique ColorPrim
{
	pass p0
	{
		VertexShader 	= compile vs_2_0 VSMain();
		PixelShader 	= compile ps_2_0 PSMain();
	}
}

technique ColorPrim2
{
	pass p0
	{
		VertexShader 	= compile vs_2_0 VSMain2();
		PixelShader 	= compile ps_2_0 PSMain2();
	}
}