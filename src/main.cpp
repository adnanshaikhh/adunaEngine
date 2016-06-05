#define TINYOBJLOADER_IMPLEMENTATION

#include <stdio.h>
#include <iostream>
#include <string>
#include <list>
#include <utility>

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>

#include "ui/Program.h"
#include "util/FileUtil.h"
#include "ui/ShaderUtil.h"
#include "ui/Texture.h"
#include "ui/Camera.h"
#include "util/SDLTimer.h"
#include "util/ObjLoader.h"

#include <glm/gtc/matrix_transform.hpp>

#include "./thirdparty/tiny_obj_loader.h"

bool Init();
bool InitGL();
void Update();
void Render();
void Close();

struct BufferGroup
{
  GLuint vao;
  GLuint index;
  GLuint vertex;
  GLuint uv;
  GLuint normal;
  GLuint draw_count;
};

struct ModelAsset
{
  Program *shaders;
  Texture *texture;
  GLuint vert_vbo;
  GLuint normal_vbo;
  GLuint tex_vbo;
  GLuint element_vbo;
  GLuint vao;
  GLenum draw_type;
  GLint draw_start;
  GLint draw_count;
  GLfloat shininess;
  glm::vec3 specular_color;
  std::vector<BufferGroup> buffer_ids;
};

struct ModelInstance
{
  ModelAsset *asset;
  glm::mat4 transform; //Model Matrix
};

struct Light
{
  glm::vec3 position;
  glm::vec3 intensities; //Light Color
  float attenuation;
  float ambient_coefficient;
};

const int g_screen_width = 1920;
const int g_screen_height = 1080;

SDL_Window *g_window = NULL;

SDL_GLContext g_context;

//New Globals
ModelAsset g_wooden_crate;
ModelAsset g_world;
//std::list<ModelInstance> g_instances;
std::list< std::list<ModelInstance> > g_objects;
std::list< std::list<ModelInstance> > g_hud_objects;

Light g_light;

GLfloat g_degrees_rotated = 0.0f;

float g_time_elapsed = 0;

Camera g_camera;

bool has_focus = false;



void Update(float g_time_elapsed) {
  //rotate by 1 degree

  
  
  const GLfloat degrees_per_second = 0.1f;

  g_degrees_rotated += g_time_elapsed * degrees_per_second;
  
  //g_light.position = glm::vec3(15 + g_degrees_rotated, 15, 15);

  ///g_light.position = g_camera.GetPosition();
  //printf("%f, %f, %f\n", g_camera.GetPosition().x, g_camera.GetPosition().y, g_camera.GetPosition().z);
  g_light.position = glm::vec3(2,50,1);
  
  //don't go over 360 degrees
  while(g_degrees_rotated > 360.0f)
    g_degrees_rotated -= 360.0f;

  int middle_x = g_screen_width / 2;
  int middle_y = g_screen_height / 2; 
  const float mouse_sensitivity = 50.0f;
  int mouse_X, mouse_Y;
  SDL_ShowCursor(SDL_DISABLE); 
  SDL_GetMouseState(&mouse_X, &mouse_Y);
  SDL_WarpMouseInWindow(g_window, middle_x, middle_y);
   
  float x_rotation = -(float)(middle_x - mouse_X) / (float) (g_screen_width);
  float y_rotation = -(float)(middle_y - mouse_Y) / (float) (g_screen_height);
  g_camera.OffsetOrientation(mouse_sensitivity * (float)(y_rotation), mouse_sensitivity * (float)(x_rotation));
} 




Program* LoadShaders(const char *vertex_filepath, const char *frag_filepath)
{
  std::vector< std::pair<std::string, GLenum> > shaders;

  shaders.push_back(std::make_pair(vertex_filepath, GL_VERTEX_SHADER));
  shaders.push_back(std::make_pair(frag_filepath, GL_FRAGMENT_SHADER));

  return new Program(shaders);
}


Texture* LoadTexture(const char *texture_filepath)
{
  Bitmap bmp = Bitmap::bitmapFromFile(texture_filepath);
  bmp.flipVertically();
  return new Texture(bmp);
}

template <typename T>
GLuint prepareVertexBuffer(const std::vector<T>& data,
			   GLenum target = GL_ARRAY_BUFFER,
			   GLenum usage = GL_STATIC_DRAW)
{
  GLuint buffername = 0;
  glGenBuffers(1, &buffername);
  glBindBuffer(target, buffername);
  glBufferData(target,
	       data.size() * sizeof(data[0]),
	       &data[0],
	       usage);
  return buffername;
}

ModelAsset LoadWoodenCrateAsset(const char * model ,const char * texture)
{
  ModelAsset model_asset;
  model_asset.shaders = LoadShaders("./assets/shaders/vertex-shader.txt",
				       "./assets/shaders/fragment-shader.txt");
  model_asset.draw_type = GL_TRIANGLES;
  model_asset.draw_start = 0;
  //g_wooden_crate.draw_count = 6 * 2 * 3;
  model_asset.texture = LoadTexture(texture);

  model_asset.shininess = 80.0f;
  model_asset.specular_color = glm::vec3(1.0f, 1.0f, 1.0f);

  size_t vertex_buffer_size = 0;

  std::string inputfile = model;
  std::vector <tinyobj::shape_t> shapes;
  std::vector <tinyobj::material_t> materials;

  std::string err;
  if(tinyobj::LoadObj(shapes, materials, err, inputfile.c_str(), "./assets/textures/"), true)
  {
    std::cout << err << "\n";
    printf("File read\n");

    std::cout << "# of shapes    : " << shapes.size() << std::endl;
    std::cout << "# of materials : " << materials.size() << std::endl;
    /*
    
    for (size_t i = 0; i < shapes.size(); i++) {
      vertex_buffer_size += sizeof(float)* shapes[i].mesh.positions.size();
    }

    printf("Vertex Buffer Size is: %lu\n", vertex_buffer_size);

    for (size_t i = 0; i < shapes.size(); i++)
      {
	printf("shape[%ld].name = %s\n", i, shapes[i].name.c_str());
	printf("Size of shape[%ld].indices: %ld\n", i, shapes[i].mesh.indices.size());
	printf("Size of shape[%ld].material_ids: %ld\n", i, shapes[i].mesh.material_ids.size());
	assert((shapes[i].mesh.indices.size() % 3) == 0);
	for (size_t f = 0; f < shapes[i].mesh.indices.size() / 3; f++)
	  {
	    printf("  idx[%ld] = %d, %d, %d. mat_id = %d\n", f, shapes[i].mesh.indices[3*f+0], shapes[i].mesh.indices[3*f+1], shapes[i].mesh.indices[3*f+2], shapes[i].mesh.material_ids[f]);
	  }

	printf("shape[%ld].vertices: %ld\n", i, shapes[i].mesh.positions.size());
	assert((shapes[i].mesh.positions.size() % 3) == 0);
	for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++)
	  {
	    printf("  v[%ld] = (%f, %f, %f)\n", v,
		   shapes[i].mesh.positions[3*v+0],
		   shapes[i].mesh.positions[3*v+1],
		   shapes[i].mesh.positions[3*v+2]);
	  }
      }
    */

    model_asset.buffer_ids.resize(shapes.size(), {0, 0, 0, 0, 0});
      
    for (size_t i = 0 ; i < shapes.size() ; ++i)
      {
	glGenVertexArrays(1, &model_asset.buffer_ids[i].vao);
	glBindVertexArray(model_asset.buffer_ids[i].vao);
	// Indices
	model_asset.buffer_ids[i].index = prepareVertexBuffer(shapes[i].mesh.indices, GL_ELEMENT_ARRAY_BUFFER);
	model_asset.buffer_ids[i].draw_count = (int)shapes[i].mesh.indices.size();

	// Vertices
	model_asset.buffer_ids[i].vertex = prepareVertexBuffer(shapes[i].mesh.positions);
	glEnableVertexAttribArray(model_asset.shaders->GetAttribLocation("vert"));
	glVertexAttribPointer(model_asset.shaders->GetAttribLocation("vert"), 3, GL_FLOAT, GL_FALSE, 0, NULL);
  

	// UVs
	if (!shapes[i].mesh.normals.empty())
	  {
	    model_asset.buffer_ids[i].uv = prepareVertexBuffer(shapes[i].mesh.texcoords);
	    glEnableVertexAttribArray(model_asset.shaders->GetAttribLocation("vert_tex_coord"));
	    glVertexAttribPointer(model_asset.shaders->GetAttribLocation("vert_tex_coord"), 2, GL_FLOAT, GL_FALSE, 0, NULL);
	  }

	// Normals
	if (!shapes[i].mesh.normals.empty())
	  {
	    model_asset.buffer_ids[i].normal = prepareVertexBuffer(shapes[i].mesh.normals);
	    glEnableVertexAttribArray(model_asset.shaders->GetAttribLocation("vert_normal"));
	    glVertexAttribPointer(model_asset.shaders->GetAttribLocation("vert_normal"), 3, GL_FLOAT, GL_FALSE, 0, NULL);
	  }
	
	glBindBuffer (GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
      }

    return model_asset;
    
      
  }
  else
    {
      std::cout << err << "\n";
      printf("shit son we messed up\n");
    }

}

static void CreateInstances() {
  std::list <ModelInstance> gun_instances;
  /*
  ModelInstance dot;
  dot.asset = &g_wooden_crate;
  dot.transform = glm::translate(glm::scale(glm::mat4(), glm::vec3(0.1,0.1,0.1)), glm::vec3(0,0,10));
  gun_instances.push_back(dot);
  */
  
  ModelInstance i;
  i.asset = &g_wooden_crate;
  i.transform = glm::translate(glm::mat4(), glm::vec3(1, 1, 1)) * glm::scale(glm::mat4(), glm::vec3(0.1,0.1,0.1));
  gun_instances.push_back(i);
  g_hud_objects.push_back(gun_instances);
  
    
  std::list <ModelInstance> world_instances;
  ModelInstance world;
  world.asset = &g_world;
  world.transform = glm::mat4();
  world_instances.push_back(world);
  g_objects.push_back(world_instances);
  /*

    ModelInstance hLeft;
    hLeft.asset = &g_wooden_crate;
    //hLeft.transform = translate(-8,0,0) * scale(1,6,1);
    hLeft.transform = glm::translate(glm::mat4(), glm::vec3(-8, 0, 0)) * glm::scale(glm::mat4(), glm::vec3(1,6,1));
    g_instances.push_back(hLeft);

    ModelInstance hRight;
    hRight.asset = &g_wooden_crate;
    //hRight.transform = translate(-4,0,0) * scale(1,6,1);
    hRight.transform = glm::translate(glm::mat4(), glm::vec3(-4, 0, 0)) * glm::scale(glm::mat4(), glm::vec3(1,6,1));
    g_instances.push_back(hRight);

    ModelInstance hMid;
    hMid.asset = &g_wooden_crate;
    //hMid.transform = translate(-6,0,0) * scale(2,1,0.8);
    hMid.transform = glm::translate(glm::mat4(), glm::vec3(-6, 0, 0)) * glm::scale(glm::mat4(), glm::vec3(2,1,0.8));
    g_instances.push_back(hMid);

  */
    
}

static void RenderInstance(const ModelInstance& inst)
{
  glm::mat4 new_transform = glm::translate(glm::mat4(), glm::vec3(1.5,-1.0,-5.0));
  //new_transform = glm::mat4(); ; //* glm::translate(glm::mat4(), g_camera.GetPosition());
  ModelAsset *asset = inst.asset;
  Program *shaders = asset->shaders;
 

  //bind the shaders
  //shaders->Use();
    
  //set the shader uniforms
   
  
  //    shaders->SetUniform("model", inst.transform);
  // TO DO FIX FLICKERING BECAUSE OF THIS DIRTY INBRED GUN SOLUTION!!!

  shaders->SetUniform("camera", g_camera.GetMatrix());
  shaders->SetUniform("model", inst.transform);

    
  shaders->SetUniform("material_tex", 0); //set to 0 because the texture will be bound to GL_TEXTURE0
  shaders->SetUniform("material_shininess", asset->shininess);
  shaders->SetUniform("material_specular_color", asset->specular_color);

  shaders->SetUniform("light.position", g_light.position);
  shaders->SetUniform("light.intensities", g_light.intensities);
  shaders->SetUniform("light.attenuation", g_light.attenuation);
  shaders->SetUniform("light.ambient_coefficient", g_light.ambient_coefficient);

  shaders->SetUniform("camera_position", g_camera.GetPosition());
    
  //bind the texture
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, asset->texture->GetTextureID());


  /*
  //bind VAO and draw
  glBindVertexArray(asset->vao);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, asset->element_vbo);
    
  //glDrawArrays(asset->draw_type, asset->draw_start, asset->draw_count);
  glDrawElements(GL_TRIANGLES, asset->draw_count, GL_UNSIGNED_INT, 0);

  //unbind everything
  glBindVertexArray(0);
  //glBindVertexArray(0);

  */
  for (size_t i = 0; i < asset->buffer_ids.size(); i++)
    {
      glBindVertexArray(asset->buffer_ids[i].vao);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, asset->buffer_ids[i].index);
      glDrawElements(GL_TRIANGLES, asset->buffer_ids[i].draw_count, GL_UNSIGNED_INT, 0);
    }
  glBindTexture(GL_TEXTURE_2D, 0);
  //shaders->StopUsing();
}

//////////////


static void RenderGunInstance(const ModelInstance& inst)
{
  //glm::mat4 new_transform = glm::translate(glm::mat4(), glm::vec3(1.5,-1.0,-5.0));
  glm::mat4 new_transform = glm::translate(glm::rotate(glm::mat4(), 0.2f, glm::vec3(0.0f,1.0f,0.0f)), glm::vec3(0.5f, 0.0f,-1.0f));
  //new_transform = glm::mat4(); ; //* glm::translate(glm::mat4(), g_camera.GetPosition());
  ModelAsset *asset = inst.asset;
  Program *shaders = asset->shaders;

  //bind the shaders
  //shaders->Use();
    
  //set the shader uniforms
    
  shaders->SetUniform("camera", g_camera.GetProjection());
  shaders->SetUniform("model", new_transform);
  
  shaders->SetUniform("material_tex", 0); //set to 0 because the texture will be bound to GL_TEXTURE0
  shaders->SetUniform("material_shininess", asset->shininess);
  shaders->SetUniform("material_specular_color", asset->specular_color);

  shaders->SetUniform("light.position", g_light.position);
  shaders->SetUniform("light.intensities", g_light.intensities);
  shaders->SetUniform("light.attenuation", g_light.attenuation);
  shaders->SetUniform("light.ambient_coefficient", g_light.ambient_coefficient);

  shaders->SetUniform("camera_position", g_camera.GetPosition());
    
  //bind the texture
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, asset->texture->GetTextureID());


  /*
  //bind VAO and draw
  glBindVertexArray(asset->vao);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, asset->element_vbo);
    
  //glDrawArrays(asset->draw_type, asset->draw_start, asset->draw_count);
  glDrawElements(GL_TRIANGLES, asset->draw_count, GL_UNSIGNED_INT, 0);

  //unbind everything
  glBindVertexArray(0);
  //glBindVertexArray(0);

  */
  for (size_t i = 0; i < asset->buffer_ids.size(); i++)
    {
      glBindVertexArray(asset->buffer_ids[i].vao);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, asset->buffer_ids[i].index);
      glDrawElements(GL_TRIANGLES, asset->buffer_ids[i].draw_count, GL_UNSIGNED_INT, 0);
    }
  glBindTexture(GL_TEXTURE_2D, 0);
  //shaders->StopUsing();
}


void Render()
{
  // clear everything
  glClearColor(0, 0, 0.1, 1); // black
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // render all the instances
  std::list<ModelInstance>::const_iterator instance_iter;
  std::list< std::list <ModelInstance> >::const_iterator object_iter;

  Program *current_shader = g_objects.front().front().asset->shaders; //List of Objects -> List of Instances 
  current_shader->Use();

  for(object_iter = g_objects.begin(); object_iter != g_objects.end(); ++object_iter)
    {
      for(instance_iter = object_iter->begin(); instance_iter != object_iter->end(); ++instance_iter)
	{
	  
	    if(instance_iter->asset->shaders != current_shader)
	    {
	      current_shader->StopUsing();
	      current_shader = instance_iter->asset->shaders;
	      current_shader->Use();
	      //printf("Shader bound and unbound");
	      }
	  
	  RenderInstance(*instance_iter);
	}
    }

  glClear(GL_DEPTH_BUFFER_BIT);
  //glDepthMask(GL_FALSE);

  for(object_iter = g_hud_objects.begin(); object_iter != g_hud_objects.end(); ++object_iter)
    {
      for(instance_iter = object_iter->begin(); instance_iter != object_iter->end(); ++instance_iter)
	{
	  
	    if(instance_iter->asset->shaders != current_shader)
	    {
	      current_shader->StopUsing();
	      current_shader = instance_iter->asset->shaders;
	      current_shader->Use();
	      //printf("Shader bound and unbound");
	      }
	  
	  RenderGunInstance(*instance_iter);
	}
    }
  
  //glDepthMask(GL_TRUE);
  
}

bool Init()
{
  bool success = true;

  //Initialize SDL
  if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
      printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
      success = false;
    }
  else
    {
      //Use OpenGL 3.1 core
      SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
      SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
      SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

      //Create window
      g_window = SDL_CreateWindow("adunaEngine", SDL_WINDOWPOS_UNDEFINED,
				  SDL_WINDOWPOS_UNDEFINED, g_screen_width,
				  g_screen_height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
      if(g_window == NULL)
	{
	  printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
	  success = false;
	}
      else
	{
	  //Create context
	  g_context = SDL_GL_CreateContext(g_window);
	  if(g_context == NULL)
	    {
	      printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
	      success = false;
	    }
	  else
	    {
	      //Initialize GLEW
	      glewExperimental = GL_TRUE; 
	      GLenum glew_error = glewInit();
	      if(glew_error != GLEW_OK)
		{
		  printf("Error initializing GLEW! %s\n", glewGetErrorString(glew_error));
		}

	      std::cout << "\nOpenGL version: " << glGetString(GL_VERSION) << std::endl;
	      std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
	      std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
	      std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;

	      // OpenGL settings
	      glEnable(GL_BLEND);
	      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	      glEnable(GL_DEPTH_TEST);
	      glDepthFunc(GL_LESS);
	      
	      //Use Vsync
	      if(SDL_GL_SetSwapInterval(1) < 0)
		{
		  printf("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
		}

	    }
	}
    }

  return success;

}


int main(int argc, char* args[])
{
  if(!Init())
    {
      printf("Failed to initialize!\n");
      return 0;
    }
  else
    {      
      //Main loop flag
      bool quit = false;

      //Event handler
      SDL_Event e;
		
      //Enable text input
      SDL_StartTextInput();

      //LoadShaders();
      //LoadTexture();
      //LoadTriangle();
      
      // initialise the gWoodenCrate asset
      g_wooden_crate = LoadWoodenCrateAsset("./assets/models/assault_rifle.obj","./assets/textures/Diffuse.png");
      g_world = LoadWoodenCrateAsset("./assets/models/lost_empire.obj","./assets/textures/lost_empire-RGBA.png");
      // create all the instances in the 3D scene based on the gWoodenCrate asset
      CreateInstances();



      g_light.position = glm::vec3(15, 15, 15);
      g_light.intensities = glm::vec3(1,1,1);
      g_light.attenuation = 0.2f;
      g_light.ambient_coefficient = 0.005f;
      
      g_camera.SetPosition(glm::vec3(0,0,4));
      g_camera.SetViewportAspectRatio(((float)g_screen_width/(float)g_screen_height));


      SDLTimer time_elapsed;

      time_elapsed.Start();
      
      const Uint8 *keyboard_state = SDL_GetKeyboardState(NULL);

      uint32_t last_time = time_elapsed.GetTicks();
      int num_frames = 0;

      
      //While application is running
      while(!quit)
	{
	  //Handle events on queue

	  uint32_t current_time = time_elapsed.GetTicks();
	  num_frames++;
	  if(current_time - last_time >= 500)
	    {
	      g_time_elapsed = 500.0/double(num_frames);
	      //std::cout<<g_time_elapsed<<std::endl;
	      std::string new_window_title = "adunaEngine - "+std::to_string(g_time_elapsed)+" ms";
	      SDL_SetWindowTitle(g_window, new_window_title.c_str());

	      num_frames = 0;
	      last_time += 500;
	    }
	  
	  while(SDL_PollEvent(&e) != 0)
	    {

	      switch(e.type)
		{
		case SDL_QUIT:
		  quit = true;
		  break;

		}
	      if(e.type == SDL_WINDOWEVENT)
		{
		  switch(e.window.event)
		    {
		    case SDL_WINDOWEVENT_ENTER:
		      std::cout<<"Window gained focus"<<std::endl;
		      has_focus = true;
		      break;
		    case SDL_WINDOWEVENT_LEAVE:
		      std::cout<<"Window lost focus"<<std::endl;
		      has_focus = false;
		      break;
		    }
		}
	      
	    }
	  if(has_focus)
	    {
	      //SDL_ShowCursor(SDL_DISABLE); 
	      //SDL_WarpMouseInWindow(g_window, 320, 240);
	    }
	  
	  float sensit = 0.01f;

	  if(keyboard_state[SDL_SCANCODE_W])
	    {
	      g_camera.OffsetPosition(g_time_elapsed * sensit * g_camera.GetForwardVector());
	    }
	  if(keyboard_state[SDL_SCANCODE_S])
	    {
	      g_camera.OffsetPosition(g_time_elapsed * sensit * -g_camera.GetForwardVector());
	    }
	  if(keyboard_state[SDL_SCANCODE_A])
	    {
	      g_camera.OffsetPosition(g_time_elapsed * sensit * -g_camera.GetRightVector());
	    }
	  if(keyboard_state[SDL_SCANCODE_D])
	    {
	      g_camera.OffsetPosition(g_time_elapsed * sensit * g_camera.GetRightVector());
	    }

	  //g_time_elapsed = (float)(time_elapsed.GetTicks());

	  //std::cout<<g_time_elapsed<<std::endl;
	  
	  Update(g_time_elapsed);
	  
	  //Render quad
	  Render();
			
	  //Update screen
	  SDL_GL_SwapWindow(g_window);
	}
		
      //Disable text input
      SDL_StopTextInput();
    }
  
  //Free Resources
  Close();
  return 0;
}


void Close()
{
  
  //Destroy window
  SDL_DestroyWindow(g_window);
  g_window = NULL;

  //Quit SDL subsystems
  SDL_Quit();
}
