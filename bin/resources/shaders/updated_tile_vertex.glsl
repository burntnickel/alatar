#version 400 core

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

// OpenGL updated vertex shader for Alatar

// Uniforms
// ----------------------------------
uniform mat4 u_view_matrix;

// Per vertex inputs
// ----------------------------------
in vec2 in_pos;
in vec2 in_uv;

// Outputs
// ----------------------------------
out vec4 shader_coord;
out vec2 UV;

void main() {
    vec4 position4 = vec4(in_pos.x, in_pos.y, 0.0, 1.0);
    gl_Position = u_view_matrix * position4;
    //gl_Position = 2.0 * u_view_matrix * vec4(in_pos.x, in_pos.y, 0.0, 1.0) - vec4(1.0, 1.0, 0.0, 0.0);
    //gl_Position.w = 1.0;
    //shader_coord = gl_Position;
    shader_coord = position4;
    //UV = vec2(in_uv.x + 1.0, -1.0 * in_uv.y + 1.0);
    UV = vec2(in_uv.x, -1.0 * in_uv.y + 1.0);
    //UV = in_uv;
}
