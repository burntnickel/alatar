#ifndef H_ALATAR_CLASSIC
#define H_ALATAR_CLASSIC

// Miscelanaeous data / functions that only apply to the classic mode display

#include <array>

namespace alatar_classic {

// Color sequences for animating fire and treasures
const std::array<const unsigned char, 3> kFireCycleColors{{8, 7, 10}}; // orange, yellow, pink
const std::array<const unsigned char, 3> kTreasureCycleColors{{7, 1, 3}}; // yellow, white, cyan

// Bounds of the fire animtation charaters to cycle through
constexpr unsigned char kFireTileFirst = 114;
constexpr unsigned char kFireTileLast = 117;

// Rather then specify the number of frame between animation updates here we specify the time between frames
// and do the computation dynamically in the game loop
constexpr double kTreasureColorCycleFrameTimeMs = 1000.0 / 15.0;
constexpr double kFireColorCycleFrameTimeMs = 1000.0 / 20.0;
constexpr double kFireAnimationFrameTimeMs = 1000.0 / 10.0;

}  // namespace alatar_classic

#endif
