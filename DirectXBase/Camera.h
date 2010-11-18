#ifndef CAMERA
#define CAMERA

#include <string>
#include <d3d10.h>
#include <d3dx10.h>
#include "MHRTimer.h"

// Convert degrees to radians
#define TO_RAD 0.0174532925199432957692369076848861271344287188854172545609


class Camera
{
public:
    Camera();
    ~Camera();

    void SetPerspectiveProjection(float fov, float aspectRatio, float nearPlane, float farPlane);

    void SetPositionAndView(float x, float y, float z, float yaw, float pitch);
    void AdjustYawPitch(float yaw, float pitch);

    void SetMovementToggles(int i, int v);

    void Update();

    D3DXMATRIX* GetProjectionMatrix();
    D3DXMATRIX* GetViewMatrix();
    D3DXMATRIX* GetRotationMatrix();
private:
    float yaw, pitch;

    D3DXMATRIX viewMatrix;
    D3DXMATRIX projectionMatrix;
    D3DXMATRIX rotationMatrix;

    D3DXVECTOR3 v, u;
    D3DXVECTOR3 eye, view, up;

    D3DXVECTOR3 forward, right;
    HRTimerController timers;

    int movementToggles[4];

    void UpdateView();
};

#endif