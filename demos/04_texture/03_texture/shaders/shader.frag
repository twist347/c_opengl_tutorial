#version 330 core

out vec4 frag_color;

in vec2 v_tex_coord;

uniform sampler2D u_tex0;
uniform sampler2D u_tex1;

void main() {
   frag_color = mix(texture(u_tex0, v_tex_coord), texture(u_tex1, v_tex_coord), 0.5);
}