#define _USE_MATH_DEFINES
#include <cmath>
#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

static const float MaxVerticalAngle = 85.0f; //must be less than 90 to avoid gimbal lock

Camera::Camera():
  position_(0.0f, 0.0f, 1.0f),
  horizontal_angle_(0.0f),
  vertical_angle_(0.0f),
  field_of_view_(50.0f),
  near_plane_(0.01f),
  far_plane_(100.0f),
  viewport_aspect_ratio_(4.0f/3.0f)
{
}

const glm::vec3& Camera::GetPosition() const {
    return position_;
}

void Camera::SetPosition(const glm::vec3& position) {
    position_ = position;
}

void Camera::OffsetPosition(const glm::vec3& offset) {
    position_ += offset;
}

float Camera::GetFieldOfView() const {
    return field_of_view_;
}

void Camera::SetFieldOfView(float field_of_view) {
    assert(field_of_view > 0.0f && field_of_view < 180.0f);
    field_of_view_ = field_of_view;
}

float Camera::GetNearPlane() const {
    return near_plane_;
}

float Camera::GetFarPlane() const {
    return far_plane_;
}


void Camera::SetNearAndFarPlanes(float near_plane, float far_plane) {
    assert(near_plane > 0.0f);
    assert(far_plane > near_plane);
    near_plane_ = near_plane;
    far_plane_ = far_plane;
}

glm::mat4 Camera::GetMatrix() const
{
    return GetProjection() * GetView();
}

glm::mat4 Camera::GetProjection() const
{
    return glm::perspective(glm::radians(field_of_view_), viewport_aspect_ratio_, near_plane_, far_plane_);
}

glm::mat4 Camera::GetView() const
{
    return GetOrientation() * glm::translate(glm::mat4(), -position_);
}

glm::mat4 Camera::GetOrientation() const
{
    glm::mat4 orientation;
    orientation = glm::rotate(orientation, glm::radians(vertical_angle_), glm::vec3(1,0,0));
    orientation = glm::rotate(orientation, glm::radians(horizontal_angle_), glm::vec3(0,1,0));
    return orientation;
}


glm::mat4 Camera::GetReversedOrientation() const
{
    glm::mat4 orientation;
    orientation = glm::rotate(orientation, glm::radians(-vertical_angle_), glm::vec3(1,0,0));
    orientation = glm::rotate(orientation, glm::radians(-horizontal_angle_), glm::vec3(0,1,0));
    return orientation;
}





//////////////////


void Camera::OffsetOrientation(float up_angle, float right_angle)
{
    horizontal_angle_ += right_angle;
    vertical_angle_ += up_angle;
    NormalizeAngles();
}

void Camera::GetLookAtVector(glm::vec3 position)
{
    assert(position != position_);
    glm::vec3 direction = glm::normalize(position - position_);
    vertical_angle_ = glm::radians(asinf(-direction.y));
    horizontal_angle_ = -glm::radians(atan2f(-direction.x, -direction.z));
    NormalizeAngles();
}

float Camera::GetViewportAspectRatio() const
{
    return viewport_aspect_ratio_;
}

void Camera::SetViewportAspectRatio(float viewport_aspect_ratio)
{
    assert(viewport_aspect_ratio > 0.0);
    viewport_aspect_ratio_ = viewport_aspect_ratio;
}

glm::vec3 Camera::GetForwardVector() const
{
    glm::vec4 forward = glm::inverse(GetOrientation()) * glm::vec4(0,0,-1,1);
    return glm::vec3(forward);
}

glm::vec3 Camera::GetRightVector() const
{
    glm::vec4 right = glm::inverse(GetOrientation()) * glm::vec4(1,0,0,1);
    return glm::vec3(right);
}

glm::vec3 Camera::GetUpVector() const
{
    glm::vec4 up = glm::inverse(GetOrientation()) * glm::vec4(0,1,0,1);
    return glm::vec3(up);
}

void Camera::NormalizeAngles()
{
    horizontal_angle_ = fmodf(horizontal_angle_, 360.0f);
    //fmodf can return negative values, but this will make them all positive
    if(horizontal_angle_ < 0.0f)
        horizontal_angle_ += 360.0f;

    if(vertical_angle_ > MaxVerticalAngle)
        vertical_angle_ = MaxVerticalAngle;
    else if(vertical_angle_ < -MaxVerticalAngle)
        vertical_angle_ = -MaxVerticalAngle;
}
