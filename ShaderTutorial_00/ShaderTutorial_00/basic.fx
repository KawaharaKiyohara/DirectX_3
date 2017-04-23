/*!
 *@brief	�P���Ȓ��_�V�F�[�_�[�B
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
 *@brief	���_�V�F�[�_�[�B
 */
VS_OUTPUT VSMain( VS_INPUT In )
{
	VS_OUTPUT Out;
	Out.pos = In.pos;
	Out.color = In.color;
	return Out;
}
/*!
 *@brief	�s�N�Z���V�F�[�_�[�B
 */
float4 PSMain( VS_OUTPUT In ) : COLOR
{
	return In.color;
}


/*!
 *@brief	���_�V�F�[�_�[�B
 */
VS_OUTPUT VSMain2( VS_INPUT In )
{
	VS_OUTPUT Out;
	Out.pos = In.pos;
	Out.color = In.color;
	return Out;
}
/*!
 *@brief	���_�V�F�[�_�[�B
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