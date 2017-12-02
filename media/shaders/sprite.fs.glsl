#version 330

uniform sampler2D sprite_texture;
in vec2 vs_uv;
out vec4 color;

void main() {
    color = texture(sprite_texture, vs_uv);
}
