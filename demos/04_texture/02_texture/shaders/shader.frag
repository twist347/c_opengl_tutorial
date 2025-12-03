#version 330 core

out vec4 frag_color;

in vec3 v_color;
in vec2 v_tex_coord;

uniform sampler2D u_tex0;

void main() {
   frag_color = texture(u_tex0, v_tex_coord) * vec4(v_color, 1.0);
}