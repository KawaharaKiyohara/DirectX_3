/*!
 *@brief	�萔���W�X�^�Ƀ��[�h���ꂽ�J���[���g�p���āA�o�̓J���[��ύX���Ă݂�B
 */


float4 g_color;		//�J���[�B���ꂪ�V�F�[�_�[�萔�BCPU����l���]������Ă���B

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
	Out.color = In.color * g_color;	//���_�J���[�ɒ萔���W�X�^�ɐݒ肳�ꂽ�J���[����Z���Ă݂�B
	return Out;
}
/*!
 *@brief	���_�V�F�[�_�[�B
 */
float4 PSMain( VS_OUTPUT In ) : COLOR
{
	return In.color;
}

technique ColorPrim
{
	pass p0
	{
		VertexShader 	= compile vs_2_0 VSMain();
		PixelShader 	= compile ps_2_0 PSMain();
	}
}