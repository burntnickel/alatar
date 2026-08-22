#include "util.h"

namespace alatar {

int SignExtendNibble(unsigned char c) {
  c = c & 0x0f;

  if (c <= 7) {
    return static_cast<int>(c);
  }

  return -1 * (16 - static_cast<int>(c));
}

}  // namespace alatar
