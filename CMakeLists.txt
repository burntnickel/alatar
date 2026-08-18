
cmake_minimum_required( VERSION 3.7...3.26 ) 

if( ${CMAKE_VERSION} VERSION_LESS 3.12 )
    cmake_policy( VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} )
endif()

project( MyProject VERSION 0.0
                   DESCRIPTION "Alatar"
                   LANGUAGES CXX )

# Why do I need this?????
set ( CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} /usr/lib/x86_64-linux-gnu/cmake )

find_package( SDL2 REQUIRED )

find_package( SDL2_image REQUIRED )

set( CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin )

set( EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR} )

message( CMAKE_BUILD_TYPE= "${CMAKE_BUILD_TYPE}" )

add_compile_options( -Wall -Wextra -Wwrite-strings -Wno-parentheses -Wpedantic -Wdeprecated )
add_compile_options( -Wsign-conversion -Wimplicit-fallthrough -Wshadow -Warray-bounds )
add_compile_options( -Wmissing-braces -Wconversion -Wuseless-cast )

if (CMAKE_BUILD_TYPE MATCHES "^[Rr]elease")
    #add_compile_options( -O3 )
else()
    #add_compile_options( -g )
endif()

FILE( GLOB MY_HEADERS ${CMAKE_SOURCE_DIR}/src/*.h )

# Build character set extractor
set( CHAREXTRACT_SRCS ${CMAKE_SOURCE_DIR}/src/cbmchr2pixmap.cc )

add_executable( cbmchr2pixmap ${CHAREXTRACT_SRCS} ${MY_HEADERS} )

target_compile_features( cbmchr2pixmap PRIVATE cxx_std_20 )

# Build sprite extractor
set( SPRITEEXTRACT_SRCS ${CMAKE_SOURCE_DIR}/src/cbmspr2pixmap.cc )

add_executable( cbmspr2pixmap ${SPRITEEXTRACT_SRCS} ${MY_HEADERS} )

target_compile_features( cbmspr2pixmap PRIVATE cxx_std_20 )

# Build level explorer
set( LEVELEXPLORER_SRCS ${CMAKE_SOURCE_DIR}/src/level_explorer.cc )
set( LEVELEXPLORER_SRCS ${LEVELEXPLORER_SRCS} ${CMAKE_SOURCE_DIR}/src/globals.cc )
set( LEVELEXPLORER_SRCS ${LEVELEXPLORER_SRCS} ${CMAKE_SOURCE_DIR}/src/monsters.cc )
set( LEVELEXPLORER_SRCS ${LEVELEXPLORER_SRCS} ${CMAKE_SOURCE_DIR}/src/wiz_level.cc )

add_executable( level_explorer ${LEVELEXPLORER_SRCS} ${MY_HEADERS} )

target_include_directories( level_explorer PUBLIC ${SDL2_INCLUDE_DIRS} )
target_compile_features( level_explorer PRIVATE cxx_std_20 )

# Build level viewer
set( LEVELVIEWER_SRCS ${CMAKE_SOURCE_DIR}/src/level_display.cc )
set( LEVELVIEWER_SRCS ${LEVELVIEWER_SRCS} ${CMAKE_SOURCE_DIR}/src/c64_clut.cc )
set( LEVELVIEWER_SRCS ${LEVELVIEWER_SRCS} ${CMAKE_SOURCE_DIR}/src/classic.cc )
set( LEVELVIEWER_SRCS ${LEVELVIEWER_SRCS} ${CMAKE_SOURCE_DIR}/src/getexepath.cc )
set( LEVELVIEWER_SRCS ${LEVELVIEWER_SRCS} ${CMAKE_SOURCE_DIR}/src/getrespath.cc )
set( LEVELVIEWER_SRCS ${LEVELVIEWER_SRCS} ${CMAKE_SOURCE_DIR}/src/globals.cc )
set( LEVELVIEWER_SRCS ${LEVELVIEWER_SRCS} ${CMAKE_SOURCE_DIR}/src/graphics_common.cc )
set( LEVELVIEWER_SRCS ${LEVELVIEWER_SRCS} ${CMAKE_SOURCE_DIR}/src/load_data.cc )
set( LEVELVIEWER_SRCS ${LEVELVIEWER_SRCS} ${CMAKE_SOURCE_DIR}/src/monsters.cc )
set( LEVELVIEWER_SRCS ${LEVELVIEWER_SRCS} ${CMAKE_SOURCE_DIR}/src/opengl_helper.cc )
set( LEVELVIEWER_SRCS ${LEVELVIEWER_SRCS} ${CMAKE_SOURCE_DIR}/src/sdl_helper.cc )
set( LEVELVIEWER_SRCS ${LEVELVIEWER_SRCS} ${CMAKE_SOURCE_DIR}/src/sprites_classic.cc )
set( LEVELVIEWER_SRCS ${LEVELVIEWER_SRCS} ${CMAKE_SOURCE_DIR}/src/tiles_classic.cc )
set( LEVELVIEWER_SRCS ${LEVELVIEWER_SRCS} ${CMAKE_SOURCE_DIR}/src/tiles_updated.cc )
set( LEVELVIEWER_SRCS ${LEVELVIEWER_SRCS} ${CMAKE_SOURCE_DIR}/src/updated.cc )
set( LEVELVIEWER_SRCS ${LEVELVIEWER_SRCS} ${CMAKE_SOURCE_DIR}/src/wiz_level.cc )

add_executable( level_display ${LEVELVIEWER_SRCS} ${MY_HEADERS} )

# OpenGL stuff
target_link_libraries( level_display "-lGLEW -lGL" )

target_include_directories( level_display PUBLIC ${SDL2_INCLUDE_DIRS} ${SDL_IMAGE_INCLUDE_DIRS} )
target_link_libraries( level_display ${SDL2_LIBRARIES} SDL2_image )
target_compile_features( level_display PRIVATE cxx_std_20 )

message( SDL2_LIBRARIES= "${SDL2_LIBRARIES}" )
message( SDL_IMAGE_LIBRARIES= "${SDL_IMAGE_LIBRARIES}" )




# Build Level Editor Utility
#set( AGEDIT_SRCS ${CMAKE_SOURCE_DIR}/src/agedit.cc )
#set( AGEDIT_SRCS ${AGEDIT_SRCS} ${CMAKE_SOURCE_DIR}/src/bfn.cc )
#set( AGEDIT_SRCS ${AGEDIT_SRCS} ${CMAKE_SOURCE_DIR}/src/bl_da_linelist.cc )
#set( AGEDIT_SRCS ${AGEDIT_SRCS} ${CMAKE_SOURCE_DIR}/src/bl_da_opengl_helper.cc )
#set( AGEDIT_SRCS ${AGEDIT_SRCS} ${CMAKE_SOURCE_DIR}/src/bl_draw_abstract.cc )
#set( AGEDIT_SRCS ${AGEDIT_SRCS} ${CMAKE_SOURCE_DIR}/src/boost_json.cc )
#set( AGEDIT_SRCS ${AGEDIT_SRCS} ${CMAKE_SOURCE_DIR}/src/getexepath.cc )
#set( AGEDIT_SRCS ${AGEDIT_SRCS} ${CMAKE_SOURCE_DIR}/src/getrespath.cc )
#set( AGEDIT_SRCS ${AGEDIT_SRCS} ${CMAKE_SOURCE_DIR}/src/level.cc )
#set( AGEDIT_SRCS ${AGEDIT_SRCS} ${CMAKE_SOURCE_DIR}/src/utility.cc )

#add_executable( agedit ${AGEDIT_SRCS} ${MY_HEADERS} )

# OpenGL stuff
#target_link_libraries( agedit "-lGLEW -lGL" )
#target_compile_definitions ( agedit PRIVATE BL_USE_OPENGL=1 )

#target_include_directories( agedit PUBLIC ${SDL2_INCLUDE_DIRS} )
#target_link_libraries( agedit ${SDL2_LIBRARIES} )
#target_compile_features( agedit PRIVATE cxx_std_20 )


