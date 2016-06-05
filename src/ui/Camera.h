#ifndef UI_CAMERA_H
#define UI_CAMERA_H

#include <glm/glm.hpp>


class Camera
{
 public:
  Camera();

  const glm::vec3& GetPosition() const;

 void SetPosition(const glm::vec3& position);

 void OffsetPosition(const glm::vec3& offset);

 float GetFieldOfView() const;
 
 void SetFieldOfView(float field_of_view);
 
 float GetNearPlane() const;
 
 float GetFarPlane() const;

 void SetNearAndFarPlanes(float near_plane, float far_plane);
 
 glm::mat4 GetMatrix() const;
 
 glm::mat4 GetProjection() const;
 
 glm::mat4 GetView() const;

 glm::mat4 GetOrientation() const;

  glm::mat4 GetReversedOrientation() const;
 

 //////////
 
 void OffsetOrientation(float up_angle, float right_angle);

 void GetLookAtVector(glm::vec3 position);

 float GetViewportAspectRatio() const;

 void SetViewportAspectRatio(float viewport_aspect_ratio);
 
 glm::vec3 GetForwardVector() const;
 
 glm::vec3 GetRightVector() const;
 
 glm::vec3 GetUpVector() const;
 
 
 
 private:
  
  glm::vec3 position_;
  float horizontal_angle_;
  float vertical_angle_;
  float field_of_view_;
  float near_plane_;
  float far_plane_;
  float viewport_aspect_ratio_;

  void NormalizeAngles();
};


#endif //UI_CAMERA_H
