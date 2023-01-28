#version 450

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;


layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 biotangent;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 outuv;
void main() {
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(pos, 1.0);
    fragColor = vec3(1.0, 1.0, 1.0);
    outuv = uv;
}