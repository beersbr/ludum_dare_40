#version 330

uniform sampler2D frame_texture;
in vec2 vs_uv;
out vec4 color;

void main() {
    color = vec4(texture(frame_texture, vs_uv).rgb, 1.0);
}
