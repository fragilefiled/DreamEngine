#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 uv;
layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 1) uniform sampler2D texSampler;
layout(set = 0, binding = 2) uniform sampler2D texSampler1;
layout(set = 0, binding = 3) uniform sampler2D texSampler2;
layout(set = 0, binding = 4) uniform sampler2D texSampler3;

void main() {
    outColor = texture(texSampler2, uv) ;;
}