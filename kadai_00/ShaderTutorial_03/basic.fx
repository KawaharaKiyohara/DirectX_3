/*!
 *@brief	�V���v���Ȏˉe�ϊ��V�F�[�_�[�B
 */

float4x4 mWorld;	//���[���h�s��B
float4x4 mView;		//�r���[�s��B
float4x4 mProj;		//�v���W�F�N�V�����s��B



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
	float4 pos = mul(In.pos, mWorld);
	pos = mul(pos, mView);
	pos = mul(pos, mProj);
	Out.pos = pos;
	Out.color = In.color;
	return Out;
}
/*!
 *@brief	�s�N�Z���V�F�[�_�[�B
 */
float4 PSMain( VS_OUTPUT In ) : COLOR
{
	return float4(1.0f, 0.0f, 0.0f, 1.0f);
}

technique SkinModel
{
	pass p0
	{
		VertexShader 	= compile vs_2_0 VSMain();
		PixelShader 	= compile ps_2_0 PSMain();
	}
}