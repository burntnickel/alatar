#ifndef H_BURNINGLOGIC_SDL_HELPER
#define H_BURNINGLOGIC_SDL_HELPER

#include <filesystem>
#include <string_view>

#include <SDL_image.h>

bool ErrorEvalPrintSDL(bool condition, std::string_view message);

bool LoadToSurfaceRGBA(std::filesystem::path path_and_name, SDL_Surface*& target_surface);
bool LoadToSurface1Chan(std::filesystem::path path_and_name, SDL_Surface*& target_surface);

#endif
