// Tessellation Evaluation Shader (TES)
#version 450

layout(quads, equal_spacing, ccw) in;

uniform mat4 view;
uniform mat4 projection;
uniform int face;

// Output position and normal
out vec3 FragPos;
out vec3 FragNormal;

// Function to compute the Bézier basis function
float bernstein(int i, float t) {
    if (i == 0) return (1.0 - t) * (1.0 - t) * (1.0 - t);
    if (i == 1) return 3.0 * t * (1.0 - t) * (1.0 - t);
    if (i == 2) return 3.0 * t * t * (1.0 - t);
    if (i == 3) return t * t * t;
    return 0.0;
}

// Function to compute the derivative of the Bézier basis function
float bernsteinDerivative(int i, float t) {
    if (i == 0) return -3.0 * (1.0 - t) * (1.0 - t);
    if (i == 1) return 3.0 * (1.0 - t) * (1.0 - 3.0 * t);
    if (i == 2) return 3.0 * t * (2.0 - 3.0 * t);
    if (i == 3) return 3.0 * t * t;
    return 0.0;
}

void main() {
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    // Compute Bézier surface position
    vec3 position = vec3(0.0);
    vec3 dPdu = vec3(0.0);
    vec3 dPdv = vec3(0.0);

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            float Bu = bernstein(i, u);
            float Bv = bernstein(j, v);
            float dBu = bernsteinDerivative(i, u);
            float dBv = bernsteinDerivative(j, v);

            vec3 controlPoint = gl_in[i * 4 + j].gl_Position.xyz;
            position += controlPoint * Bu * Bv;
            dPdu += controlPoint * dBu * Bv;
            dPdv += controlPoint * Bu * dBv;
        }
    }

    FragPos = position;
    FragNormal = normalize(cross(dPdu, dPdv));
    if (face == 0 || face == 3 || face == 4)
        FragNormal *= -1;

    gl_Position = projection * view * vec4(position, 1.0);
}