#version 330 core

in vec3 color;
in vec3 pos;

out vec4 frag_color;

uniform vec2 center;
uniform float radius;

void main() {
   float dist = distance(pos.xy, center);

   if (dist <= radius) {
      frag_color = vec4(color, 1.0);
   } else {
      discard;
   }
}
