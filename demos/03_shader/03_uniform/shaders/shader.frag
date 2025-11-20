#version 330 core

out vec4 frag_color;

uniform float time;

void main() {
   float val = abs(sin(time));
   frag_color = vec4(val, 0.0, 0.0, 0.0);
};