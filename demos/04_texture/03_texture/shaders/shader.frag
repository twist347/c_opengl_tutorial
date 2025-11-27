#version 330 core

out vec4 frag_color;
in vec2 tex_coord;

uniform sampler2D tex0;
uniform sampler2D tex1;

void main() {
   frag_color = mix(texture(tex0, tex_coord), texture(tex1, tex_coord), 0.5);
};