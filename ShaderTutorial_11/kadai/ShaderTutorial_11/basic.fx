/*!
 *@brief	���e�V���h�E�B
 */

float4x4 g_worldMatrix;			//���[���h�s��B
float4x4 g_viewMatrix;			//�r���[�s��B
float4x4 g_projectionMatrix;	//�v���W�F�N�V�����s��B


texture g_diffuseTexture;		//�f�B�t���[�Y�e�N�X�`���B
sampler g_diffuseTextureSampler = 
sampler_state
{
	Texture = <g_diffuseTexture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    AddressU = Wrap;
	AddressV = Wrap;
};


struct VS_INPUT{
	float4	pos		: POSITION;
	float4	color	: COLOR0;
	float2	uv		: TEXCOORD0;
};

struct VS_OUTPUT{
	float4	pos				: POSITION;	
	float4	color			: COLOR0;
	float2	uv				: TEXCOORD0;
};

/*!
 *@brief	���_�V�F�[�_�[�B
 */
VS_OUTPUT VSMain( VS_INPUT In )
{
	VS_OUTPUT Out = (VS_OUTPUT)0;
	float4 worldPos; 
	worldPos = mul( In.pos, g_worldMatrix );		//���f���̃��[�J����Ԃ��烏�[���h��Ԃɕϊ��B
	Out.pos = mul( worldPos, g_viewMatrix );		//���[���h��Ԃ���r���[��Ԃɕϊ��B
	Out.pos = mul( Out.pos, g_projectionMatrix );	//�r���[��Ԃ���ˉe��Ԃɕϊ��B
	Out.color = In.color;
	Out.uv = In.uv;
	return Out;
}
/*!
 *@brief	�s�N�Z���V�F�[�_�[�B
 */
float4 PSMain( VS_OUTPUT In ) : COLOR
{
	float4 color = tex2D( g_diffuseTextureSampler, In.uv );
	return color;
}


/*!
 *@brief	�ʏ�`��p�̃e�N�j�b�N
 */
technique SkinModel
{
	pass p0
	{
		VertexShader 	= compile vs_3_0 VSMain();
		PixelShader 	= compile ps_3_0 PSMain();
	}
}
