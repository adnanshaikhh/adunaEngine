#include "FileUtil.h"

#include <stdexcept>
#include <string>
#include <fstream>


std::string FileUtil::read(const std::string file_name)
{
  std::ifstream file(file_name.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
  if(file)
    { 
      std::string text;
      text.resize(file.tellg());
      file.seekg(0,std::ios::beg);
      file.read(&text[0],text.size());
      file.close();

      return text;
    }
  throw std::runtime_error(std::string("Failed to open file: ") + file_name);
}
