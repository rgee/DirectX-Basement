#include "Camera.h"

void Camera::SetPerspectiveProjection(float fov, float aspectRatio, float nearPlane, float farPlane)
{
    D3DXMatrixPerspectiveFovLH(&projectionMatrix, fov * TO_RAD, aspectRatio, nearPlane, farPlane);
}

Camera::Camera()
    : v(D3DXVECTOR3(0,0,1)),
      u(D3DXVECTOR3(0,1,0)),
      eye(D3DXVECTOR3(0,0,0)),
      view(D3DXVECTOR3(0,0,1)),
      up(D3DXVECTOR3(0,1,0)),
      forward(D3DXVECTOR3(0,0,1)),
      yaw(0),
      pitch(0)
{
    D3DXMatrixIdentity(&viewMatrix);
    D3DXMatrixIdentity(&projectionMatrix);

    timers.createTimer();


    movementToggles[0] = 0;
    movementToggles[1] = 0;
    movementToggles[2] = 0;
    movementToggles[3] = 0;
}

void Camera::AdjustYawPitch(float newYaw, float newPitch)
{
    yaw += newYaw;
    pitch += newPitch;

    if(yaw > 2*D3DX_PI)
    {
        yaw -= (float)D3DX_PI*2;
    } 
    else if (yaw < 0)
    {
        yaw = (float)(D3DX_PI*2) + yaw;
    }

    if(pitch > 2*D3DX_PI)
    {
        pitch -= (float)D3DX_PI*2;
    }
    else if(pitch < 0)
    {
        pitch = (float)(D3DX_PI*2) + pitch;
    }
}

void Camera::UpdateView()
{
    D3DXMatrixRotationYawPitchRoll(&rotationMatrix, yaw, pitch, 0);

    D3DXVec3TransformCoord(&view, &v, &rotationMatrix);
    D3DXVec3TransformCoord(&up, &u, &rotationMatrix);

    D3DXVec3Normalize(&forward, &view);
    D3DXVec3Cross(&right, &up, &view);
    D3DXVec3Normalize(&right, &right);

    view = eye + view;

    D3DXMatrixLookAtLH(&viewMatrix, &eye, &view, &up);
}

void Camera::SetPositionAndView(float x, float y, float z, float newYaw, float newPitch)
{
    eye.x = x;
    eye.y = y;
    eye.z = z;

    yaw = newYaw * (float)TO_RAD;
    pitch = newPitch * (float)TO_RAD;

    UpdateView();
}

void Camera::SetMovementToggles(int i, int v)
{
    movementToggles[i] = v;
}

void Camera::Update()
{
    double t = timers[0]->getElapsedTimeSeconds();

    eye += t * (movementToggles[0] + movementToggles[1]) * 1.5f * forward +
           t * (movementToggles[2] + movementToggles[3]) * 1.5f * right;

    UpdateView();

    timers[0]->reset();
}
D3DXMATRIX* Camera::GetProjectionMatrix()
{
    return &projectionMatrix;
}

D3DXMATRIX* Camera::GetViewMatrix()
{
    return &viewMatrix;
}

D3DXMATRIX* Camera::GetRotationMatrix()
{
    return &rotationMatrix;
}

Camera::~Camera()
{

}