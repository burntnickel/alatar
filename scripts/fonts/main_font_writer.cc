

#include <algorithm>
#include <format>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <ft2build.h>
#include FT_FREETYPE_H

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image_write.h"

// const std::string kFontFileName = "C64_Pro_Mono-STYLE.ttf";
// constexpr kCharHeight = 1711; // char_height in 1/64 of points

const std::string kFontFileName = "Alatar.ttf";
constexpr int kCharHeight = 1904;  // char_height in 1/64 of points

using CharMap = std::map<unsigned char, FT_ULong>;

static void PopulateCharMap(CharMap& char_map) {
  // Lower case letters
  for (int idx = 0x01; idx <= 0x1a; ++idx) {
    unsigned char c64_idx = static_cast<unsigned char>(idx);
    FT_ULong ascii_idx = static_cast<FT_ULong>(0x61 - 0x01 + idx);
    char_map[c64_idx] = ascii_idx;
  }

  char_map[0x21] = '!';
  char_map[0x22] = '"';
  char_map[0x23] = '#';
  char_map[0x24] = '$';
  char_map[0x25] = '%';

  char_map[0x26] = '&';
  char_map[0x27] = '\'';
  char_map[0x28] = '(';
  char_map[0x29] = ')';
  char_map[0x2a] = '*';

  char_map[0x2b] = '+';
  char_map[0x2c] = ',';
  char_map[0x2d] = '-';
  char_map[0x2e] = '.';
  char_map[0x2f] = '/';

  // Numbers and following punctuation
  for (int idx = 0x30; idx <= 0x3f; ++idx) {
    unsigned char c64_idx = static_cast<unsigned char>(idx);
    FT_ULong ascii_idx = static_cast<FT_ULong>(idx);
    char_map[c64_idx] = ascii_idx;
  }

  // Upper case letters
  for (int idx = 0x41; idx <= 0x5a; ++idx) {
    unsigned char c64_idx = static_cast<unsigned char>(idx);
    FT_ULong ascii_idx = static_cast<FT_ULong>(0x41 - 0x41 + idx);
    char_map[c64_idx] = ascii_idx;
  }
}

int main(void) {
  std::vector<unsigned char> pixmap;
  const std::string base_filename = "main_font_raw_";
  CharMap char_map;
  FT_Library ft_library;
  ;

  auto error = FT_Init_FreeType(&ft_library);

  if (error) {
    std::cerr << "Error initalizing FT Library\n";
    return EXIT_FAILURE;
  }

  FT_Face face;

  error = FT_New_Face(ft_library, kFontFileName.c_str(), 0, &face);

  if (error == FT_Err_Unknown_File_Format) {
    std::cerr << "Font format unsupported\n";
    return EXIT_FAILURE;
  } else if (error) {
    std::cerr << "Error reading font file\n";
    return EXIT_FAILURE;
  }

  error = FT_Set_Char_Size(face,        /* handle to face object         */
                           0,           /* char_width in 1/64 of points  */
                           kCharHeight, /* char_height in 1/64 of points */
                           300,         /* horizontal device resolution  */
                           300);        /* vertical device resolution    */

  if (error) {
    std::cerr << "Error calling FT_Set_Char_Size\n";
    return EXIT_FAILURE;
  }

  PopulateCharMap(char_map);

  for (const auto [c64_idx, ascii_idx] : char_map) {
  }

  // We'll loop twice here
  // The first pass is assess the overall bound box of the glyphs processed
  // The second pass will save them off as PNG files
  unsigned int bounding_width = 0;
  int height_above = 0;
  int height_below = 0;

  std::cout << "Processing first pass...\n";

  for (const auto [c64_idx, ascii_idx] : char_map) {
    auto glyph_index = FT_Get_Char_Index(face, ascii_idx);

    error = FT_Load_Glyph(face,             /* handle to face object */
                          glyph_index,      /* glyph index           */
                          FT_LOAD_DEFAULT); /* load flags*/

    if (error) {
      std::cerr << "Error calling FT_Load_Glyph\n";
      return EXIT_FAILURE;
    }

    error = FT_Render_Glyph(face->glyph,            /* glyph slot  */
                            FT_RENDER_MODE_NORMAL); /* render mode */

    if (error) {
      std::cerr << "Error calling FT_Render_Glyph\n";
      return EXIT_FAILURE;
    }

    auto the_glyph = face->glyph;

    height_above = std::max(height_above, the_glyph->bitmap_top);
    height_below = std::max(
        height_below, static_cast<int>(the_glyph->bitmap.rows) - static_cast<int>(the_glyph->bitmap_top));
    bounding_width = std::max(bounding_width, the_glyph->bitmap.width);
  }

  unsigned int bounding_height = height_above + height_below;

  std::cout << "Bounding Box Width  : " << bounding_width << "\n";
  std::cout << "Bounding Box Height : " << bounding_height << "\n";
  std::cout << "Max. Above Baseline : " << height_above << "\n";
  std::cout << "Max. Below Baseline : " << height_below << "\n";

  std::cout << "Processing second pass...\n";

  for (const auto [c64_idx, ascii_idx] : char_map) {
    auto glyph_index = FT_Get_Char_Index(face, ascii_idx);

    error = FT_Load_Glyph(face,             /* handle to face object */
                          glyph_index,      /* glyph index           */
                          FT_LOAD_DEFAULT); /* load flags*/

    if (error) {
      std::cerr << "Error calling FT_Load_Glyph\n";
      return EXIT_FAILURE;
    }

    error = FT_Render_Glyph(face->glyph,            /* glyph slot  */
                            FT_RENDER_MODE_NORMAL); /* render mode */

    if (error) {
      std::cerr << "Error calling FT_Render_Glyph\n";
      return EXIT_FAILURE;
    }

    auto the_glyph = face->glyph;

    int width = the_glyph->bitmap.width;
    int stride = width;

    // Resize the pixmap vector and the copy the glyphs into it in the proper spots
    pixmap.resize(width * bounding_height);
    std::fill(pixmap.begin(), pixmap.end(), 0);

    int src = 0;
    int dst = 0;

    for (int rr = 0; rr < the_glyph->bitmap.rows; ++rr) {
      for (int cc = 0; cc < width; ++cc) {
        src = rr * the_glyph->bitmap.pitch + cc;
        dst = (height_above - the_glyph->bitmap_top + rr) * stride + cc;
        // std::cout << width * bounding_height << "   " << dst << "\n";
        pixmap[dst] = std::max(static_cast<int>(the_glyph->bitmap.buffer[src]), 0);
      }
    }

    std::string filename = base_filename + std::format("{:03}", c64_idx) + ".png";

    int status = stbi_write_png(filename.c_str(), width, bounding_height, 1, pixmap.data(), stride);

    if (status == 0) {
      std::cerr << "Error writing PNG file (" << filename << "\n";
      return EXIT_FAILURE;
    }
  }

  return EXIT_SUCCESS;
}
