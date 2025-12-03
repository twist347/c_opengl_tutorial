#version 330 core

in vec2 v_tex_coord;
in vec3 v_color;

uniform sampler2D u_tex0;

uniform int u_mode;        // 0..7
uniform float u_time;      // glfwGetTime()

out vec4 frag_color;

vec4 effect_original(vec2 uv) {
    return texture(u_tex0, uv);
}

vec4 effect_grayscale(vec2 uv) {
    vec4 c = texture(u_tex0, uv);
    float gray = dot(c.rgb, vec3(0.299, 0.587, 0.114));
    return vec4(vec3(gray), c.a);
}

vec4 effect_invert(vec2 uv) {
    vec4 c = texture(u_tex0, uv);
    c.rgb = 1.0 - c.rgb;
    return c;
}

vec4 effect_sepia(vec2 uv) {
    vec4 c = texture(u_tex0, uv);
    vec3 sepia;
    sepia.r = dot(c.rgb, vec3(0.393, 0.769, 0.189));
    sepia.g = dot(c.rgb, vec3(0.349, 0.686, 0.168));
    sepia.b = dot(c.rgb, vec3(0.272, 0.534, 0.131));
    return vec4(sepia, c.a);
}

vec4 effect_color_adjust(vec2 uv) {
    vec4 c = texture(u_tex0, uv);
    vec3 col = c.rgb;

    return vec4(col, c.a);
}

vec4 effect_vignette(vec2 uv) {
    vec4 c = texture(u_tex0, uv);

    vec2 centered = uv * 2.0 - 1.0;
    float dist = length(centered);

    float radius = 0.7;
    float softness = 0.5;

    float vignette = smoothstep(radius, radius - softness, dist);

    c.rgb *= vignette;

    return c;
}

vec4 effect_waves(vec2 uv) {
    const float freq = 10.0;
    const float amp = 0.03;
    const float speed = 3.0;

    vec2 uv2 = uv;
    uv2.y += sin(uv.x * freq + u_time * speed) * amp;

    return texture(u_tex0, uv2);
}

vec4 effect_scroll(vec2 uv) {
    const float speed = 0.2;
    vec2 uv2 = uv + vec2(u_time * speed, 0.0);
    uv2 = fract(uv2);

    return texture(u_tex0, uv2);
}

vec4 effect_posterize(vec2 uv) {
    vec4 c = texture(u_tex0, uv);
    float levels = 4.0;
    c.rgb = floor(c.rgb * levels) / levels;
    return c;
}

vec4 effect_rgb_shift(vec2 uv) {
    float amount = 0.01;
    vec2 r_uv = uv + vec2(amount, 0.0);
    vec2 g_uv = uv;
    vec2 b_uv = uv - vec2(amount, 0.0);

    return vec4(
        texture(u_tex0, r_uv).r,
        texture(u_tex0, g_uv).g,
        texture(u_tex0, b_uv).b,
        1.0
    );
}

vec4 effect_contrast(vec2 uv) {
    vec4 c = texture(u_tex0, uv);
    float contrast = 2.0;
    c.rgb = (c.rgb - 0.5) * contrast + 0.5;
    return c;
}

vec4 effect_kaleidoscope(vec2 uv) {
    vec2 centered = uv - 0.5;
    float angle = atan(centered.y, centered.x);
    float radius = length(centered);

    // 6-сторонний калейдоскоп
    angle = mod(angle, 3.14159 / 3.0);
    vec2 newUV = vec2(cos(angle), sin(angle)) * radius + 0.5;

    return texture(u_tex0, newUV);
}

vec4 effect_swirl(vec2 uv) {
    vec2 centered = uv - 0.5;
    float radius = length(centered);
    float angle = atan(centered.y, centered.x);

    float swirl = 2.0;
    angle += swirl * (1.0 - radius * 2.0);

    vec2 newUV = vec2(cos(angle), sin(angle)) * radius * 2.0 + 0.5;
    return texture(u_tex0, newUV);
}

vec4 effect_pixelate(vec2 uv) {
    float pixelSize = 0.02;
    vec2 pixelUV = floor(uv / pixelSize) * pixelSize;
    return texture(u_tex0, pixelUV);
}

vec4 effect_glow(vec2 uv) {
    vec4 c = texture(u_tex0, uv);

    // Простой bloom
    float brightness = dot(c.rgb, vec3(0.299, 0.587, 0.114));
    if (brightness > 0.7) {
        c.rgb += vec3(0.3, 0.3, 0.2);
    }

    return c;
}

vec4 effect_scanlines(vec2 uv) {
    vec4 c = texture(u_tex0, uv);
    float scanline = sin(uv.y * 800.0) * 0.1 + 0.9;
    c.rgb *= scanline;
    return c;
}

vec4 effect_pulse(vec2 uv) {
    vec4 c = texture(u_tex0, uv);
    float pulse = sin(u_time * 3.0) * 0.3 + 0.7;
    c.rgb *= pulse;
    return c;
}

vec4 effect_zoom(vec2 uv) {
    float zoom = 1.0 + sin(u_time) * 0.3;
    vec2 centered = (uv - 0.5) / zoom + 0.5;
    return texture(u_tex0, centered);
}

void main() {
    vec4 result;

    switch (u_mode) {
        case 0: result = effect_zoom(v_tex_coord); break;
        case 1: result = effect_grayscale(v_tex_coord); break;
        case 2: result = effect_invert(v_tex_coord); break;
        case 3: result = effect_sepia(v_tex_coord); break;
        case 4: result = effect_color_adjust(v_tex_coord); break;
        case 5: result = effect_vignette(v_tex_coord); break;
        case 6: result = effect_waves(v_tex_coord); break;
        case 7: result = effect_scroll(v_tex_coord); break;
        default: result = effect_original(v_tex_coord); break;
    }

    frag_color = result;
}
