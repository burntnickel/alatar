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

void TextureSetupHelper(GLenum texture_unit, GLuint* opengl_buffer, std::span<const unsigned char> buffer,
                        GLenum usage, GLuint* opengl_texture, GLenum internalformat) {
  glActiveTexture(texture_unit);
  PrintGLError("TextureSetupHelper:glActiveTexture");

  glGenBuffers(1, opengl_buffer);
  PrintGLError("TextureSetupHelper:glGenBuffers");

  glBindBuffer(GL_TEXTURE_BUFFER, *opengl_buffer);
  PrintGLError("TextureSetupHelper:glBindBuffer");

  glBufferData(GL_TEXTURE_BUFFER, static_cast<GLsizeiptr>(buffer.size()), buffer.data(), usage);
  PrintGLError("TextureSetupHelper:glBufferData");

  glGenTextures(1, opengl_texture);
  PrintGLError("TextureSetupHelper:glGenTextures");

  glBindTexture(GL_TEXTURE_BUFFER, *opengl_texture);
  PrintGLError("TextureSetupHelper:glBindTexture");

  glTexBuffer(GL_TEXTURE_BUFFER, internalformat, *opengl_buffer);
  PrintGLError("main:glTexBuffer");
}

ShaderVars BuildShaderProgram(const std::string& vertex_shader_source,
                              const std::string& fragment_shader_source) {
  ShaderVars shader_vars;

  // Create VBO, IBO & IBO
  glGenBuffers(1, &(shader_vars.vbo));
  glGenBuffers(1, &(shader_vars.ibo));
  glGenVertexArrays(1, &(shader_vars.vao));

  // Generate program
  const GLuint shader_program = glCreateProgram();

  // Create vertex shader
  const GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);

  // Set vertex source & compile
  const char* vertex_shader_source_c_str = vertex_shader_source.c_str();
  glShaderSource(vertex_shader, 1, &vertex_shader_source_c_str, NULL);
  glCompileShader(vertex_shader);

  // Check vertex shader for errors
  GLint v_shader_complied = GL_FALSE;
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &v_shader_complied);

  if (v_shader_complied != GL_TRUE) {
    std::cerr << "Unable to compile vertex shader " << vertex_shader << "\n";
    BurningLogic::PrintShaderLog(vertex_shader);
    throw std::runtime_error("Unable to compile vertex shader");
  }

  // Attach vertex shader to program
  glAttachShader(shader_program, vertex_shader);

  // Create fragment shader
  const GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

  // Set fragment source & complie
  const char* fragment_shader_source_c_str = fragment_shader_source.c_str();
  glShaderSource(fragment_shader, 1, &fragment_shader_source_c_str, NULL);
  glCompileShader(fragment_shader);

  // Check fragment shader for errors
  GLint f_shader_compiled = GL_FALSE;
  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &f_shader_compiled);

  if (f_shader_compiled != GL_TRUE) {
    std::cout << "Unable to compile fragment shader " << fragment_shader << "\n";
    BurningLogic::PrintShaderLog(fragment_shader);
    throw std::runtime_error("Unable to compile fragmant shader");
  }

  // Attach fragment shader to program
  glAttachShader(shader_program, fragment_shader);

  // Link program
  glLinkProgram(shader_program);

  // Check for errors
  GLint program_success = GL_TRUE;
  glGetProgramiv(shader_program, GL_LINK_STATUS, &program_success);

  if (program_success != GL_TRUE) {
    std::cout << "Error linking program " << shader_program << "\n";
    BurningLogic::PrintProgramLog(shader_program);
    throw std::runtime_error("Unable to link shader program");
  }

  // Detatch and delete shaders
  glDetachShader(shader_program, vertex_shader);
  glDeleteShader(vertex_shader);
  glDetachShader(shader_program, fragment_shader);
  glDeleteShader(fragment_shader);

  shader_vars.program = shader_program;

  return shader_vars;
}

void Identity4(mat4& mat) {
  mat[0] = 1.0;
  mat[1] = 0.0;
  mat[2] = 0.0;
  mat[3] = 0.0;

  mat[4] = 0.0;
  mat[5] = 1.0;
  mat[6] = 0.0;
  mat[7] = 0.0;

  mat[8] = 0.0;
  mat[9] = 0.0;
  mat[10] = 1.0;
  mat[11] = 0.0;

  mat[12] = 0.0;
  mat[13] = 0.0;
  mat[14] = 0.0;
  mat[15] = 1.0;
}

mat4 GetIdentity4(void) {
  mat4 mat;

  Identity4(mat);

  return mat;
}

void ScaleMatHelper2D(mat4& viewMatrix, float x_scale, float y_scale) {
  viewMatrix[0] = x_scale;
  viewMatrix[1] = 0.0;
  viewMatrix[2] = 0.0;
  viewMatrix[3] = 0.0;

  viewMatrix[4] = 0.0;
  viewMatrix[5] = y_scale;
  viewMatrix[6] = 0.0;
  viewMatrix[7] = 0.0;

  viewMatrix[8] = 0.0;
  viewMatrix[9] = 0.0;
  viewMatrix[10] = 1.0;
  viewMatrix[11] = 0.0;

  viewMatrix[12] = 0.0;
  viewMatrix[13] = 0.0;
  viewMatrix[14] = 0.0;
  viewMatrix[15] = 1.0;
}

}  // namespace BurningLogic
