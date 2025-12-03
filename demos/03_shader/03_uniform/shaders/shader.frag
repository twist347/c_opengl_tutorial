#version 330 core

out vec4 frag_color;

uniform float u_time;

void main() {
   float val = abs(sin(u_time));
   frag_color = vec4(val, 0.0, 0.0, 0.0);
}