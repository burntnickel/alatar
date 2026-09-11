// Copyright 2026 Jude Giampaolo
//
// This file is part of Alatar.
//
// Alatar is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
//
// Alatar is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
// PARTICULAR PURPOSE. See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with Alatar.
// If not, see <https://www.gnu.org/licenses/>. 

#ifndef H_BURNINGLOGIC_OPENGL_HELPER
#define H_BURNINGLOGIC_OPENGL_HELPER

#include <GL/glew.h>

#include <array>
#include <filesystem>
#include <source_location>
#include <span>
#include <string>

#ifdef __linux__
#include <GL/glu.h>
#include <SDL_opengl.h>

#elif defined _WIN32
#include <GL/glu.h>
#include <SDL_opengl.h>

#elif defined __APPLE__
#include <OpenGL/glu.h>
#include <SDL2/SDL_opengl.h>
#endif

namespace BurningLogic {

struct ShaderVars {
  GLuint vbo;
  GLuint ibo;
  GLuint vao;
  GLuint program;
};

using mat4 = std::array<float, 16>;

void PrintShaderLog(GLuint shader);
void PrintProgramLog(GLuint program);
GLuint GetShaderAttributeLocation(GLuint program, const std::string& attribute_name);
GLint GetShaderUniformLocation(GLuint program, const std::string& uniform_name);
std::string LoadShaderSource(std::filesystem::path shader_filename);
void PrintGLError(const std::string& s, const std::source_location loc = std::source_location::current());
void TextureSetupHelper(GLenum texture_unit, GLuint* opengl_buffer, std::span<const unsigned char> buffer,
                        GLenum usage, GLuint* opengl_texture, GLenum internalformat);
ShaderVars BuildShaderProgram(const std::string& vertex_shader_source,
                              const std::string& fragment_shader_source);
void Identity4(mat4& mat);
mat4 GetIdentity4(void);
void ScaleMatHelper2D(mat4& viewMatrix, float x_scale, float y_scale);

}  // namespace BurningLogic

#endif
