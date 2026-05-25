Texture2D tex : register(t0);
SamplerState sam : register(s0);

// ���� ����� b0�� �̵�
cbuffer WorldBuffer : register(b0)
{
    matrix matWorld;
};
// ��Ƽ���� ������ (b1���� ����)
cbuffer CustomData : register(b1)
{
    float4 colorTint;
};

struct VS_INPUT
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};