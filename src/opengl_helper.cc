#include <fstream>
#include <iostream>

#include "opengl_helper.h"

namespace BurningLogic {

void PrintShaderLog(GLuint shader) {
  // Make sure it actually is a shader
  if (glIsShader(shader)) {
    // Shader log length
    int info_log_length = 0;
    int max_length = 0;

    // Get log length
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &max_length);

    // Allocate string
    char* info_log = new char[max_length];

    // Get info log
    glGetShaderInfoLog(shader, max_length, &info_log_length, info_log);

    if (info_log_length > 0) {
      std::cerr << info_log << "\n";
    }

    // Deallocate string
    delete[] info_log;
  } else {
    std::cerr << "Name " << shader << "is not a shader\n";
  }
}

void PrintProgramLog(GLuint program) {
  // Make sure is actually is a shader program
  if (glIsProgram(program)) {
    // Program log length
    int info_log_length = 0;
    int max_length = 0;

    // Get log length
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &max_length);

    // Allocate string
    char* info_log = new char[max_length];

    // Get info log
    glGetProgramInfoLog(program, max_length, &info_log_length, info_log);

    if (info_log_length > 0) {
      std::cerr << info_log << "\n";
    }

    // Deallocate string
    delete[] info_log;
  } else {
    std::cerr << "Name " << program << "is not a program\n";
  }
}

GLuint GetShaderAttributeLocation(GLuint program, const std::string& attribute_name) {
  GLint tmp_location = glGetAttribLocation(program, attribute_name.c_str());

  if (tmp_location == -1) {
    throw std::runtime_error(attribute_name + " is not a valid glsl program variable!");
  }

  return static_cast<GLuint>(tmp_location);
}

GLint GetShaderUniformLocation(GLuint program, const std::string& uniform_name) {
  GLint tmp_location = glGetUniformLocation(program, uniform_name.c_str());

  if (tmp_location == -1) {
    throw std::runtime_error(uniform_name + " is not a valid glsl program uniform!");
  }

  return tmp_location;
}

std::string LoadShaderSource(std::filesystem::path shader_filename) {
  constexpr std::size_t buffer_length = static_cast<std::size_t>(8192);
  std::string return_string;

  auto input_stream = std::ifstream(shader_filename.string());
  input_stream.exceptions(std::ios_base::badbit);

  if (!input_stream) {
    throw std::ios_base::failure("File not found: " + shader_filename.string());
  }

  std::string buffer = std::string(buffer_length, '\0');

  while (input_stream.read(&(buffer[0]), buffer_length)) {
    return_string.append(buffer, 0, static_cast<std::size_t>(input_stream.gcount()));
  }

  return_string.append(buffer, 0, static_cast<std::size_t>(input_stream.gcount()));

  return return_string;
}

void PrintGLError(const std::string& s, const std::source_location loc) {
  GLenum gl_err = glGetError();

  if (gl_err != GL_NO_ERROR) {
    while (gl_err != GL_NO_ERROR) {
      std::cerr << loc.file_name() << "(" << loc.line() << ") " << s << ": 0x" << std::hex << gl_err
                << std::dec << "\n";
      gl_err = glGetError();
    }

    exit(EXIT_FAILURE);
  }
}

}  // namespace BurningLogic
