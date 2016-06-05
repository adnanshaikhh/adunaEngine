#include "ObjLoader.h"
#include "FileUtil.h"

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

bool ObjLoader::LoadObj (const std::string file_name,
		       std::vector <glm::vec3> &out_vertices,
		       std::vector <glm::vec2> &out_uvs,
		       std::vector <glm::vec3> &out_normals)
{

  std::ifstream obj_file(file_name.c_str(), std::ios::in | std::ios::binary);

  std::vector<unsigned int> vertex_indices, uv_indices, normal_indices;
  std::vector<glm::vec3> temp_vertices; 
  std::vector<glm::vec2> temp_uvs;
  std::vector<glm::vec3> temp_normals;

  printf("Trying to parse %s\n", file_name.c_str());
	
  if(obj_file)
    {
      std::string obj_file_line;
      while(std::getline(obj_file, obj_file_line))
	{
	  
	  if (obj_file_line.substr(0, 2) == "v ")
	    {
	      std::cout << "Vector: " << obj_file_line << "\n";
	      glm::vec3 vertex;
	      std::istringstream s(obj_file_line.substr(2));
	      s >> vertex.x;
	      s >> vertex.y;
	      s >> vertex.z;

	      std::cout << vertex.x << " " << vertex.y << " " << vertex.z << "\n";
	      
	      temp_vertices.push_back(vertex);
	      
	    }
	  else if(obj_file_line.substr(0, 3) == "vn ")
	    {
	      std::cout << "Vertex Normal: " << obj_file_line << "\n";
	      glm::vec3 vertex_normal;
	      std::istringstream s(obj_file_line.substr(2));
	      s >> vertex_normal.x;
	      s >> vertex_normal.y;
	      s >> vertex_normal.z;
	      
	      std::cout << vertex_normal.x << " " << vertex_normal.y << " " << vertex_normal.z << "\n";
	      
	      temp_normals.push_back(vertex_normal);
	    }
	  else if(obj_file_line.substr(0, 3) == "vt ")
	    {
	      std::cout << "Vertex UV: " << obj_file_line << "\n";
	      glm::vec2 uv;
	      std::istringstream s(obj_file_line.substr(2));
	      s >> uv.x;
	      s >> uv.y;
	      
	      std::cout << uv.x << " " << uv.y << " " << "\n";
	      
	      temp_uvs.push_back(uv);
	    }
	  else if(obj_file_line.substr(0, 2) == "f ")
	    {
	      std::cout << "Face: " << obj_file_line << "\n";
	      
	      unsigned int vertex_index[3], uv_index[3], normal_index[3];


	      int matches = sscanf((obj_file_line.substr(2, obj_file_line.size())).c_str(), "%d/%d/%d %d/%d/%d %d/%d/%d\n",
				   &vertex_index[0],
				   &uv_index[0],
				   &normal_index[0],
				   &vertex_index[1],
				   &uv_index[1],
				   &normal_index[1],
				   &vertex_index[2],
				   &uv_index[2],
				   &normal_index[2]
				   );
	      printf("Match is %d\n", matches);
	      if (matches != 9)
		{
		  printf("File can't be read by our simple parser :-( Try exporting with other options\n");
		  //return false;
		}
	      printf("%d %d %d | %d %d %d | %d %d %d\n",
		     vertex_index[0],
		     uv_index[0],
		     normal_index[0],
		     vertex_index[1],
		     uv_index[1],
		     normal_index[1],
		     vertex_index[2],
		     uv_index[2],
		     normal_index[2]);


	      vertex_indices.push_back(vertex_index[0]);
	      vertex_indices.push_back(vertex_index[1]);
	      vertex_indices.push_back(vertex_index[2]);
	      uv_indices    .push_back(uv_index[0]);
	      uv_indices    .push_back(uv_index[1]);
	      uv_indices    .push_back(uv_index[2]);
	      normal_indices.push_back(normal_index[0]);
	      normal_indices.push_back(normal_index[1]);
	      normal_indices.push_back(normal_index[2]);
	    }

	}

      	// For each vertex of each triangle
	for( unsigned int i=0; i<vertex_indices.size(); i++ )
	  {

		// Get the indices of its attributes
		unsigned int vertexIndex = vertex_indices[i];
		unsigned int uvIndex = uv_indices[i];
		unsigned int normalIndex = normal_indices[i];
		
		// Get the attributes thanks to the index
		glm::vec3 vertex = temp_vertices[ vertexIndex-1 ];
		glm::vec2 uv = temp_uvs[ uvIndex-1 ];
		glm::vec3 normal = temp_normals[ normalIndex-1 ];
		
		// Put the attributes in buffers
		out_vertices.push_back(vertex);
		out_uvs     .push_back(uv);
		out_normals .push_back(normal);
	
	}

	return true;

    }
  return false;
}

GLfloat *  ObjLoader::LoadObjData(const std::string file_name)
{
  std::vector <glm::vec3> vertices;
  std::vector <glm::vec2> uvs;
  std::vector <glm::vec3> normals;
      
  if(LoadObj(file_name, vertices, uvs, normals))
    {
      printf("Obj Parsing Succeded!! \n");
      int num_vertices = vertices.size();
      int num_uvs = uvs.size();
      int num_normals = normals.size();
      int num_bytes = num_vertices * 3 + num_uvs * 2 + num_normals * 3;

      printf("num_vertices are %d\n", num_vertices);
      printf("num_uvs are %d\n", num_uvs);
      printf("num_normals are %d\n", num_normals);

      static GLfloat * obj_data = (GLfloat *) malloc(num_vertices * 8);
      
      for(int i = 0, j = 0; i < num_vertices * 8; i += 8, j += 1)
	{
	  obj_data[i + 0] = vertices[j].x;
	  printf("obj_data[%d + 0] = %f\n", i , vertices[j].x);
	  
	  obj_data[i + 1] = vertices[j].y;
	  printf("obj_data[%d + 1] = %f\n", i , vertices[j].y);
	  
	  obj_data[i + 2] = vertices[j].z;
	  printf("obj_data[%d + 2] = %f\n", i , vertices[j].z);

	  obj_data[i + 3] = uvs[j].x;
	  printf("obj_data[%d + 3] = %f\n", i , uvs[j].x);
	  
	  obj_data[i + 4] = uvs[j].y;
	  printf("obj_data[%d + 4] = %f\n", i , uvs[j].y);

	  obj_data[i + 5] = normals[j].x;
	  printf("obj_data[%d + 5] = %f\n", i , normals[j].x);
	  
	  obj_data[i + 6] = normals[j].y;
	  printf("obj_data[%d + 6] = %f\n", i , normals[j].y);
	  
	  obj_data[i + 7] = normals[j].z;
	  printf("obj_data[%d + 7] = %f\n", i , normals[j].z);
      
	}
      
      printf("Obj Loading Done!");
      //return obj_data;

      return obj_data;
    }
  
  else
    {
      printf("Obj Loading Failed!! \n");
      return NULL;
    }
  
}

