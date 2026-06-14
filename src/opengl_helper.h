#ifndef H_BURNINGLOGIC_OPENGL_HELPER
#define H_BURNINGLOGIC_OPENGL_HELPER

#include <GL/glew.h>

#include <filesystem>
#include <source_location>
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

void PrintShaderLog(GLuint shader);
void PrintProgramLog(GLuint program);
GLuint GetShaderAttributeLocation(GLuint program, const std::string& attribute_name);
GLint GetShaderUniformLocation(GLuint program, const std::string& uniform_name);
std::string LoadShaderSource(std::filesystem::path shader_filename);
void PrintGLError(const std::string& s, const std::source_location loc = std::source_location::current());

}  // namespace BurningLogic

#endif
