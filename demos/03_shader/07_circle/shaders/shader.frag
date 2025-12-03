#version 330 core

in vec3 v_color;
in vec3 v_pos;

out vec4 frag_color;

uniform vec2 u_center;
uniform float u_radius;

void main() {
   float dist = distance(v_pos.xy, u_center);

   if (dist <= u_radius) {
      frag_color = vec4(v_color, 1.0);
   } else {
      discard;
   }
}
