/*!
 *@brief	���_�V�F�[�_�[�̍ł���{�ƂȂ�A�ˉe�ϊ����s���T���v���B
 */


/*float4x4 g_worldMatrix;			//���[���h�s��B
float4x4 g_viewMatrix;			//�r���[�s��B
float4x4 g_projectionMatrix;	//�v���W�F�N�V�����s��B
*/
float4x4 g_wvpMatrix;		//���[���h�r���[�v���W�F�N�V�����s��B
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
	float4 pos; 
/*	pos = mul( In.pos, g_worldMatrix );		//���f���̃��[�J����Ԃ��烏�[���h��Ԃɕϊ��B
	pos = mul( pos, g_viewMatrix );			//���[���h��Ԃ���r���[��Ԃɕϊ��B
	pos = mul( pos, g_projectionMatrix );	//�r���[��Ԃ���ˉe��Ԃɕϊ��B*/
	pos = mul( In.pos, g_wvpMatrix );			//���[���h�r���[�v���W�F�N�V�����s�����Z�B
	Out.pos = pos;
	Out.color = In.color;
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