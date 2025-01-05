// Tessellation Control Shader (TCS)
#version 450

layout(vertices = 16) out;

// Uniform input for tessellation levels
uniform float tessLevelOuter;
uniform float tessLevelInner;

void main() {
    // Pass through the control points to the TES
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    // Set tessellation levels
    gl_TessLevelOuter[0] = tessLevelOuter;
    gl_TessLevelOuter[1] = tessLevelOuter;
    gl_TessLevelOuter[2] = tessLevelOuter;
    gl_TessLevelOuter[3] = tessLevelOuter;
    gl_TessLevelInner[0] = tessLevelInner;
    gl_TessLevelInner[1] = tessLevelInner;
}