/*
 * [���� ��Ʈ: Material �ý����� ����]
 *
 * 1. Material�� ���� (������ ������)
 *    - Mesh�� "����(����)"���, Material�� "�Ǻ�(����)"��.
 *    - �������� �ʿ��� ��� ����(���̴�, �ؽ�ó, ��� ��)�� ���� �����ϴ� ������ �����̳���.
 *
 * 2. �Ϲ�ȭ�� ������ ���� (uint8_t�� �е�)
 *    - ��� �����ʹ� '����Ʈ ����(uint8_t)'�� ������. �̴� ������ �������� ��ü���� Ÿ��(����ü)��
 *      �𸣴��� �����ϰ� GPU �޸𸮷� ������ �� �ְ� ����.
 *    - ���߿�� DX11�� 16����Ʈ ���� ��Ģ: GPU�� �ϵ���� Ư���� ��� ���۴� 16�� ��� ũ�⿩�� ��.
 *      �� Ŭ������ �ڵ� �е�(Padding)�� ���� �л����� ���� ���� ������ ���� �������� ������.
 *
 * 3. ��Ű��ó: �� Bind()�� �߻� �޼����ΰ�?
 *    - �ܼ��� �����Ͷ��������� �ֽ� Compute Shader����, ������ ��Ŀ� ���� GPU�� �����ؾ� ��
 *      ���ҽ��� ������ ��ġ(Slot)�� �޶���.
 *    - '� �����͸� ������ ���ΰ�(AddTexture/AddConstantData)'�� ���� ����������,
 *      '��� ���ε��� ���ΰ�(Bind)'�� ����ϴ� ���̴��� Ư���� ���� �ٸ��� ������
 *      �ڽ� Ŭ�������� ��ü������ ������(������ Ȱ��).
 *    - �ҽ��ڵ忡�� �⺻������ ��� ���̴��� ���� �������ͷ� ���ε��ϴ� �⺻���� ž��� ������ ������� �״�� ������ ����
 *
 * 4. Bind �ý����� Ȯ�强
 *    - ���� UAV(Unordered Access View)�� SamplerState�� �߰��Ǿ, Bind() ���� ������
 *      [���� �ε� -> GPU ���� -> ���ҽ� ���ε�] �帧�� �����ϸ� ���� ������ �ھ��� �ʰ� Ȯ�� ������.
 *
 * 5. ����:
 *    class MyMat : public Material { ... Bind ���� ... };
 *    MyMat* mat = new MyMat(shaderSet);
 *    mat->AddTexture(tex);      // t0 ���� ���ε� ����
 *    mat->AddConstantData(data);// b2 ���� ���ε� ����
 */

#pragma once
#include "Framework.hpp"


class Material
{
public:
    ShaderSet* shaders;
    
    // �ؽ�ó ����Ʈ: AddTexture�� ȣ���� ������� t0, t1, t2... ���Կ� ��
    std::vector<Texture*> textureList;

    // ��� ���� ������ ����Ʈ: ����ü �����͸� ����
    // (�����͸� ����Ʈ ������ ����) - CPU �޸𸮿��� ����
    std::vector<std::vector<uint8_t>> constantDataList;
    // ������� b1, b2, b3... ���Կ� ���ε��� (b0: ������Ŀ����� ����) - GPU�� ���ε�� ����
    std::vector<ID3D11Buffer*> constantBuffers;

    Material() {};
    Material(ShaderSet* s) : shaders(s) {}
    virtual ~Material() {
        for (auto cb : constantBuffers) if (cb) cb->Release();
    }

    //ShaderSet ����
    void SetShaderSet(ShaderSet* shaderSet)
    {
        shaders = shaderSet;
    }

    //�ؽ�ó�� ��Ƽ���� �߰��� (ȣ�� ������ �� ���̴� �������� ��ȣ)
    void AddTexture(Texture* tex)
    {
        textureList.push_back(tex);
    }
    //Ŀ���� ����ü �����͸� ��Ƽ���� �߰���
    //���ø��� ����Ͽ� � ������ ����ü�� '����Ʈ ���'�� ��ȯ�Ͽ� ������.
    template<typename T>
    void AddConstantData(const T& data)
    {
        size_t originalSize = sizeof(T);

        // [�ڵ� 16����Ʈ ���� ����]
        // DX11 �䱸���׿� ���� 16�� ����� ũ�⸦ �ø��� (��: 12����Ʈ -> 16����Ʈ)
        // ���� ~15( �������� 11110000) �� &�������� 16/32/48/64/.... �� 16�� ����� ����� ����ִ� ��
        size_t alignedSize = (originalSize + 15) & ~15;

       
        // ����Ʈ(uint8_t) ������ �ӽ� ���� ����
        std::vector<uint8_t> byteBuffer(alignedSize, 0);

        // ���� �����͸� ����Ʈ ������ �� �պκп� ���� (������ ������ 0���� ä����)
        memcpy(byteBuffer.data(), &data, originalSize);

        // �ϼ��� ����Ʈ ����� ����Ʈ�� ����
        constantDataList.push_back(byteBuffer);

        

    }

    // �⺻������ �ڵ� ���ε� ������ ������.
    virtual void Bind(ID3D11DeviceContext* context)
    {
        if (!shaders) return;

        // [�߿�] �ʿ��� ������ GPU ���� ���� (Lazy Initialization)
        for (int i = 0; i < (int)constantBuffers.size(); ++i) {
            if (constantBuffers[i] == nullptr) {
                // Device�� context�� ���� ���� (ID3D11DeviceContext -> Device)
                ID3D11Device* device = nullptr;
                context->GetDevice(&device);

                D3D11_BUFFER_DESC desc = {};
                desc.Usage = D3D11_USAGE_DEFAULT;
                desc.ByteWidth = (UINT)constantDataList[i].size();
                desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

                D3D11_SUBRESOURCE_DATA initData = {};
                initData.pSysMem = constantDataList[i].data();

                device->CreateBuffer(&desc, &initData, &constantBuffers[i]);
                device->Release(); // GetDevice�� ���� ī��Ʈ�� ������Ű�Ƿ� ����
            }
        }

        // 1. ���̴� ����
        context->IASetInputLayout(shaders->layout);
        context->VSSetShader(shaders->vs, nullptr, 0);
        context->PSSetShader(shaders->ps, nullptr, 0);
        
        // 2. �ؽ�ó ���� ���ε�
        for (int i = 0; i < (int)textureList.size(); ++i)
        {
            ID3D11ShaderResourceView* srv = textureList[i]->pSRV;
            context->VSSetShaderResources(i, 1, &srv);
            context->PSSetShaderResources(i, 1, &srv);

            if (textureList[i]->pSampler)
            {
                context->VSSetSamplers(0, 1, &textureList[i]->pSampler);
                context->PSSetSamplers(0, 1, &textureList[i]->pSampler);
            }
        }


        // 3. ������� ���� ���ε� (b1���� ���ε���)
        for (int i = 0; i < (int)constantBuffers.size(); ++i)
        {
            context->VSSetConstantBuffers(i + 1, 1, &constantBuffers[i]);
            context->PSSetConstantBuffers(i + 1, 1, &constantBuffers[i]);
        }


    }

    template<typename T>
    void UpdateConstantData(ID3D11DeviceContext* context, int index, const T& data) {
        if (index >= constantBuffers.size()) return;

        // �̹� GPU�� �ö� �ִ� �׸�(Buffer)�� ���ο� ������(data)�� ���
        context->UpdateSubresource(constantBuffers[index], 0, nullptr, &data, 0, 0);
    }
};
