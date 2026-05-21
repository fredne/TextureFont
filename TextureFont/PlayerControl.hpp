#pragma once
#include "ObjectBase.hpp"

// -----------------------------------------------------------------------------
// [�÷��̾� ��Ʈ�ѷ� ������Ʈ]
// - Ű���� �Է��� �޾� ���� �����ϴ� ���� ������Ʈ
// -----------------------------------------------------------------------------
class PlayerController : public Component
{
    // �Է� ���¸� �����ϱ� ���� ��� ���� (���ο�)
    XMFLOAT2 moveDir;  // x: �¿�, y: ����
    float    rotDir;   // ȸ�� ����
    float    zoomDir;  // Ȯ��/��� ����

public:
    PlayerController() : Component()
    {
        moveDir = { 0, 0 };
        rotDir = 0.0f;
        zoomDir = 0.0f;
    }

    ~PlayerController()
    {
    }

    void Start(GraphicsContext* gfx) override
    {
    }

    // [Step 1] �Է� ���� �� ���� ����
    void Input() override
    {
        // �� ������ �Է� ���� �ʱ�ȭ
        moveDir = { 0, 0 };
        rotDir = 0.0f;
        zoomDir = 0.0f;

        // ����Ű �Է� (�̵�)
        if (GetAsyncKeyState(VK_UP) & 0x8000)    moveDir.y += 1.0f;
        if (GetAsyncKeyState(VK_DOWN) & 0x8000)  moveDir.y -= 1.0f;
        if (GetAsyncKeyState(VK_LEFT) & 0x8000)  moveDir.x -= 1.0f;
        if (GetAsyncKeyState(VK_RIGHT) & 0x8000) moveDir.x += 1.0f;

        // AD Ű �Է� (ȸ��)
        if (GetAsyncKeyState('A') & 0x8000) rotDir += 1.0f;
        if (GetAsyncKeyState('D') & 0x8000) rotDir -= 1.0f;

        // WS Ű �Է� (��)
        if (GetAsyncKeyState('W') & 0x8000) zoomDir -= 1.0f;
        if (GetAsyncKeyState('S') & 0x8000) zoomDir += 1.0f;
    }

    // [Step 2] ����� ���¸� �������� ������ ����
    void Update(float dt) override
    {
        // 1. �ӵ� ���� (������ ��� �ӵ� ���� ����)
        float speedFactor = pOwner->scale.x;
        float moveSpeed = 2.0f * speedFactor;
        float rotateSpeed = 3.0f * speedFactor;
        float zoomSpeed = 5.0f * speedFactor;

        // 2. ��ġ ������Ʈ
        pOwner->pos.x += moveDir.x * moveSpeed * dt;
        pOwner->pos.y += moveDir.y * moveSpeed * dt;

        // 3. ȸ�� ������Ʈ
        pOwner->rot.z += rotDir * rotateSpeed * dt;

        // 4. ��(Z��) ������Ʈ �� ����
        pOwner->pos.z += zoomDir * zoomSpeed * dt;

        if (pOwner->pos.z < -0.9f)
        {
            pOwner->pos.z = -0.9f;
        }
    }

    void Render(GraphicsContext* gfx) override
    {
    }
};