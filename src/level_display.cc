#include <GL/glew.h>
#include <SDL.h>

// #include <array>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <span>
#include <string>

#include "c64_clut.h"
#include "getrespath.h"
#include "opengl_helper.h"
#include "wiz_level.h"

constexpr double kMsPerFrame = 1000.0 / 60.0;

constexpr unsigned int kRowTiles = 25;
constexpr unsigned int kColTiles = 40;

const std::string kVertexShaderName{"vertex.glsl"};
const std::string kFragmentShaderName{"classic_fragment.glsl"};

SDL_Window* g_window = NULL;
SDL_GLContext g_sdl_glcontext;

uint_least32_t g_row_bytes;

GLuint g_tbo_tile_buffer;
GLuint g_tbo_tex_tile_buffer;
GLuint g_tbo_color_buffer;
GLuint g_tbo_tex_color_buffer;
GLuint g_tbo_tileset_buffer;
GLuint g_tbo_tex_tileset_buffer;
GLuint g_tbo_clut_buffer;
GLuint g_tbo_tex_clut_buffer;

struct ShaderVars {
  GLuint vbo;
  GLuint ibo;
  GLuint vao;
  GLuint program;
};

struct Rect {
  double top;
  double left;
  double bottom;
  double right;
};

// For now we'll just scale off the C64 and later we'll adjust to fix theaspect ratio
constexpr uint_least32_t kScreenWidth = 8 * kColTiles * 5 * 0.75;
constexpr uint_least32_t kScreenHeight = 8 * kRowTiles * 5;

constexpr unsigned int kCharSetSize = 8 * 256;
constexpr unsigned int kTileBufferSize = 8 * 8 * 256;

static bool Initialize(void) {
  bool success = true;

  // Initalize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "SDL could not be initialized: " << SDL_GetError() << "\n";
    success = false;
  }

  if (success) {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Create window
    g_window = SDL_CreateWindow("SDL Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, kScreenWidth,
                                kScreenHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    if (g_window == NULL) {
      std::cerr << "Window could not be created: " << SDL_GetError() << "\n";
      success = false;
    }
  }

  if (success) {
    // Create context
    g_sdl_glcontext = SDL_GL_CreateContext(g_window);

    if (g_sdl_glcontext == NULL) {
      std::cerr << "OpenGL context could not be created: " << SDL_GetError() << "\n";
      success = false;
    }
  }

  if (success) {
    // Initialize GLEW
    glewExperimental = GL_TRUE;  // Is this a variable defined in the header?
    GLenum glew_error = glewInit();

    if (glew_error != GLEW_OK) {
      std::cerr << "Error initializing GLEW: " << glewGetErrorString(glew_error) << "\n";
      success = false;
    }
  }

  if (success) {
    // Use Vsync
    if (SDL_GL_SetSwapInterval(1) < 0) {
      std::cerr << "Warning: Unable to set VSync: " << SDL_GetError() << "\n";
    }
  }

  glDisable(GL_DEPTH_TEST);

  return success;
}

static bool LoadCharSet(std::filesystem::path path_and_name,
                        std::span<unsigned char, kTileBufferSize> char_set) {
  constexpr int kDataOffset = 2;

  std::error_code ec;
  std::uintmax_t size = std::filesystem::file_size(path_and_name, ec);

  if (ec.value() != 0) {
    std::cerr << "Error: " << ec.message() << "(" << path_and_name << ")" << std::endl;
    return false;
  }

  if (size != (kCharSetSize + kDataOffset)) {
    std::cerr << "Error: File is of the incorrect size" << std::endl;
    return false;
  }

  std::ifstream in(path_and_name, std::ios::binary);

  if (!in.is_open()) {
    std::cerr << "Error: Unable to open file" << std::endl;
    return false;
  }

  in.seekg(kDataOffset, std::ios::beg);

  std::array<char, kCharSetSize> buffer;

  in.read(buffer.data(), static_cast<std::streamsize>(kCharSetSize));

  if (in.gcount() != static_cast<std::streamsize>(kCharSetSize)) {
    std::cerr << "Error: Unexpected end of file" << std::endl;
    return false;
  }

  in.close();

  // This includes all of the funny decoding of the byte/bit ordering
  for (unsigned int chr = 0; chr < 256; ++chr) {
    for (unsigned int in_rr = 0; in_rr < 8; ++in_rr) {
      unsigned int out_rr = 8 * chr + in_rr;
      unsigned int in_idx = 8 * chr + in_rr;

      unsigned char c = static_cast<unsigned char>(buffer[in_idx]);

      for (unsigned int bb = 0; bb < 8; ++bb) {
        unsigned int out_idx = 8 * out_rr + bb;

        if (c & 128) {
          char_set[out_idx] = 255;
        } else {
          char_set[out_idx] = 0;
        }

        c = c << 1;
      }
    }
  }

  return true;
}

static bool LoadLevel(std::string file_name, std::span<unsigned char, wizard_level::kFileLength> level_data) {
  std::error_code ec;
  std::uintmax_t size = std::filesystem::file_size(file_name, ec);

  if (ec.value() != 0) {
    std::cerr << "Error: " << ec.message() << std::endl;
    return false;
  }

  if (size != wizard_level::kFileLength) {
    std::cerr << "Error: File is of the incorrect size" << std::endl;
    return false;
  }

  std::ifstream in(file_name, std::ios::binary);

  if (!in.is_open()) {
    std::cerr << "Error: Unable to open file" << std::endl;
    return false;
  }

  in.read(reinterpret_cast<char*>(level_data.data()), wizard_level::kFileLength);

  if (in.gcount() != wizard_level::kFileLength) {
    std::cerr << "Error: Unexpected end of file" << std::endl;
    return false;
  }

  in.close();

  return true;
}

static ShaderVars BuildShaderProgram(const std::string& vertex_shader_source,
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

// TODO: A lot of this should be moved into a new data structure as it doesn't change with use
static void PaintRect(ShaderVars shader_vars, Rect r) {
  SDL_FRect sdl_rect;

  sdl_rect.x = static_cast<float>(r.left);
  sdl_rect.y = static_cast<float>(r.top);
  sdl_rect.w = static_cast<float>((r.right - r.left));
  sdl_rect.h = static_cast<float>((r.bottom - r.top));

  std::array<GLfloat, 8> vertex_buffer = {{sdl_rect.x, sdl_rect.y, sdl_rect.x + sdl_rect.w, sdl_rect.y,
                                           sdl_rect.x, sdl_rect.y + sdl_rect.h, sdl_rect.x + sdl_rect.w,
                                           sdl_rect.y + sdl_rect.h}};
  std::array<GLuint, 8> index_buffer = {{0, 1, 2, 3}};

  // Bind program
  glUseProgram(shader_vars.program);
  BurningLogic::PrintGLError("PaintRect:glUseProgram");

  glBindVertexArray(shader_vars.vao);
  BurningLogic::PrintGLError("PaintRect:glBindVertexArray");

  glBindBuffer(GL_ARRAY_BUFFER, shader_vars.vbo);
  BurningLogic::PrintGLError("PaintRect:glBindBuffer");

  glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertex_buffer.size() * sizeof(GLfloat)),
               vertex_buffer.data(), GL_STREAM_DRAW);
  BurningLogic::PrintGLError("PaintRect:glBufferData");

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shader_vars.ibo);
  BurningLogic::PrintGLError("PaintRect:glBindBuffer");

  glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(index_buffer.size() * sizeof(GLuint)),
               index_buffer.data(), GL_STREAM_DRAW);
  BurningLogic::PrintGLError("PaintRect:glBufferData");

  // Enable vertex position
  GLuint vpos_location = BurningLogic::GetShaderAttributeLocation(shader_vars.program, "v_pos");
  glEnableVertexAttribArray(vpos_location);
  BurningLogic::PrintGLError("PaintRect:glEnableVertexAttribArray");

  // Set vertex data
  glBindBuffer(GL_ARRAY_BUFFER, shader_vars.vbo);
  BurningLogic::PrintGLError("PaintRect:glBindBuffer");

  glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), NULL);
  BurningLogic::PrintGLError("PaintRect:glVertexAttribPointer");

  // Set scaling and translation uniforms
  // TODO save this stuff off and only update if the window size changes
  int screen_width;
  int screen_height;

  SDL_GL_GetDrawableSize(g_window, &screen_width, &screen_height);
  // glViewport(0, 0, screen_width, screen_height);

  // Pass window dimensions to the shader
  /*GLint viewport_dims_location =
      BurningLogic::GetShaderUniformLocation(shader_vars.program, "u_viewport_dims");
  glUniform2f(viewport_dims_location, static_cast<GLfloat>(kScreenWidth),
              static_cast<GLfloat>(kScreenHeight));
  BurningLogic::PrintGLError("PaintRect:glUniform2f");*/

  // Texture stuff (tile buffer)
  GLint tilebuffer_location = BurningLogic::GetShaderUniformLocation(shader_vars.program, "u_tile_buffer");
  glUniform1i(tilebuffer_location, 0);
  BurningLogic::PrintGLError("PaintRect:glUniform1i");

  // Texture stuff (color buffer)
  GLint colorbuffer_location = BurningLogic::GetShaderUniformLocation(shader_vars.program, "u_color_buffer");
  glUniform1i(colorbuffer_location, 1);
  BurningLogic::PrintGLError("PaintRect:glUniform1i");

  // Texture stuff (tile set buffer)
  GLint tileset_location = BurningLogic::GetShaderUniformLocation(shader_vars.program, "u_tileset_buffer");
  glUniform1i(tileset_location, 2);
  BurningLogic::PrintGLError("PaintRect:glUniform1i");

  // Texture stuff (clut)
  GLint clut_location = BurningLogic::GetShaderUniformLocation(shader_vars.program, "u_clut_buffer");
  glUniform1i(clut_location, 3);
  BurningLogic::PrintGLError("PaintRect:glUniform1i");

  // ------
  glActiveTexture(GL_TEXTURE0);
  BurningLogic::PrintGLError("PaintRect:glActiveTexture");

  glBindTexture(GL_TEXTURE_BUFFER, g_tbo_tile_buffer);
  BurningLogic::PrintGLError("PaintRect:glBindTexture");

  // ------
  glActiveTexture(GL_TEXTURE1);
  BurningLogic::PrintGLError("PaintRect:glActiveTexture");

  glBindTexture(GL_TEXTURE_BUFFER, g_tbo_color_buffer);
  BurningLogic::PrintGLError("PaintRect:glBindTexture");

  // Set index data and render
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shader_vars.ibo);
  BurningLogic::PrintGLError("PaintRect:glBindBuffer");

  glDrawElements(GL_TRIANGLE_STRIP, 2 * 2, GL_UNSIGNED_INT, NULL);
  BurningLogic::PrintGLError("PaintRect:glDrawElements");

  // Disable vertex position
  glDisableVertexAttribArray(vpos_location);

  // Unbind program
  glUseProgram(0);
}

static void TextureSetupHelper(GLenum texture, GLuint* opengl_buffer, std::span<const unsigned char> buffer,
                               GLenum usage, GLuint* opengl_texture, GLenum internalformat) {
  glActiveTexture(texture);
  BurningLogic::PrintGLError("TextureSetupHelper:glActiveTexture");

  glGenBuffers(1, opengl_buffer);
  BurningLogic::PrintGLError("TextureSetupHelper:glGenBuffers");

  glBindBuffer(GL_TEXTURE_BUFFER, *opengl_buffer);
  BurningLogic::PrintGLError("TextureSetupHelper:glBindBuffer");

  glBufferData(GL_TEXTURE_BUFFER, static_cast<GLsizeiptr>(buffer.size()), buffer.data(), usage);
  BurningLogic::PrintGLError("TextureSetupHelper:glBufferData");

  glGenTextures(1, opengl_texture);
  BurningLogic::PrintGLError("TextureSetupHelper:glGenTextures");

  glBindTexture(GL_TEXTURE_BUFFER, *opengl_texture);
  BurningLogic::PrintGLError("TextureSetupHelper:glBindTexture");

  glTexBuffer(GL_TEXTURE_BUFFER, internalformat, *opengl_buffer);
  BurningLogic::PrintGLError("main:glTexBuffer");
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
  bool success;
  std::array<unsigned char, kRowTiles * kColTiles> tile_buffer{};
  std::array<unsigned char, kRowTiles * kColTiles> color_buffer{};
  std::array<unsigned char, kTileBufferSize> tile_set{};
  std::array<unsigned char, wizard_level::kFileLength> level_data{};

  const std::filesystem::path kResourcePath = BurningLogic::GetResPath();

  const std::string kShadersDirName{"shaders"};
  const std::string kCharSetsDirName{"classic"};
  const std::string kCharSetName{"chrw"};

  if (argc != 2) {
    std::cerr << "Exactly one arguement must be supplied, the level file name" << std::endl;
    return EXIT_FAILURE;
  }

  success = LoadLevel(argv[1], level_data);

  if (!success) {
    std::cerr << "Failed to load level file, exiting\n";
    return EXIT_FAILURE;
  }

  success = Initialize();

  if (!success) {
    std::cerr << "Failed to initalize, exiting\n";
    return EXIT_FAILURE;
  }

  // Set to 32 (space) as blank character & black
  for (unsigned int ii = 0; ii < (kRowTiles * kColTiles); ++ii) {
    tile_buffer[ii] = 32;
    color_buffer[ii] = 0;
  }

  wizard_level::LevelClass level(level_data);

  for (int row = 0; row < wizard_level::kTileDataRows; ++row) {
    for (int col = 0; col < wizard_level::kTileDataCols; ++col) {
      unsigned int screen_row = static_cast<unsigned int>(row) + 1;
      unsigned int screen_col = static_cast<unsigned int>(col);
      unsigned int screen_index = kColTiles * screen_row + screen_col;

      unsigned char tile = level.GetTileAt(row, col);

      tile_buffer[screen_index] = tile;
      color_buffer[screen_index] = level.GetTileColor(tile);
    }
  }

  // Move these strings to constants above?
  auto tile_set_path_and_name = kResourcePath / kCharSetsDirName / kCharSetName;
  success = LoadCharSet(tile_set_path_and_name, tile_set);

  if (!success) {
    std::cerr << "Failed to load charater set, exiting\n";
    return EXIT_FAILURE;
  }

  // TODO: get rid of all of these globals

  // Tile buffer
  TextureSetupHelper(GL_TEXTURE0, &g_tbo_tile_buffer, tile_buffer, GL_DYNAMIC_DRAW, &g_tbo_tex_tile_buffer,
                     GL_R8UI);

  // Color buffer
  TextureSetupHelper(GL_TEXTURE1, &g_tbo_color_buffer, color_buffer, GL_DYNAMIC_DRAW, &g_tbo_tex_color_buffer,
                     GL_R8UI);

  // Tile set buffer
  TextureSetupHelper(GL_TEXTURE2, &g_tbo_tileset_buffer, tile_set, GL_STATIC_DRAW, &g_tbo_tex_tileset_buffer,
                     GL_R8UI);

  // CLUT buffer calls
  TextureSetupHelper(GL_TEXTURE3, &g_tbo_clut_buffer, kDefaultC64Clut, GL_STATIC_DRAW, &g_tbo_tex_clut_buffer,
                     GL_RGBA8UI);

  // Set up shaders
  const std::filesystem::path kShaderPath = kResourcePath / kShadersDirName;
  const std::filesystem::path kVertexShaderFilename = kShaderPath / kVertexShaderName;
  const std::filesystem::path kFragmentShaderFilename = kShaderPath / kFragmentShaderName;

  const std::string line_vertex_shader_string = BurningLogic::LoadShaderSource(kVertexShaderFilename);
  const std::string line_fragment_shader_source = BurningLogic::LoadShaderSource(kFragmentShaderFilename);

  auto my_shader_vars = BuildShaderProgram(line_vertex_shader_string, line_fragment_shader_source);

  bool gDone = false;
  SDL_Event sdl_event;

  while (!gDone) {
    Uint64 start_counter = SDL_GetPerformanceCounter();

    while (SDL_PollEvent(&sdl_event) != 0) {
      switch (sdl_event.type) {
        case SDL_QUIT:
          gDone = true;
          break;
        default:
          break;
      }
    }

    // Clear the screen
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw
    // TODO: Should probably rename this as well
    PaintRect(my_shader_vars, Rect({-1.0, -1.0, 2.0, 2.0}));

    // Present
    SDL_GL_SwapWindow(g_window);

    Uint64 end_counter = SDL_GetPerformanceCounter();

    double elapsed_ms = static_cast<double>(end_counter - start_counter) /
                        static_cast<double>(SDL_GetPerformanceFrequency()) * 1000.0;

    auto delay_time_ms = std::floor(kMsPerFrame - elapsed_ms);

    if (delay_time_ms > 0.0) {
      SDL_Delay(static_cast<Uint32>(delay_time_ms));
    }
  }

  return EXIT_SUCCESS;
}
