#version 330 core

// Input vertex attribute from the vertex buffer
in vec3 a_vertex;
in vec3 a_normal;
in vec2 a_uv;
in vec4 a_color;

// Uniforms for camera position and matrices
uniform vec3 u_camera_pos;
uniform mat4 u_model;
uniform mat4 u_viewprojection;

// Output varying variables to the fragment shader
out vec3 v_position;
out vec3 v_world_position;
out vec3 v_normal;
out vec2 v_uv;
out vec4 v_color;

void main() {    
    // Calculate the normal in camera space
    v_normal = (u_model * vec4(a_normal, 0.0)).xyz;
    
    // Calculate the vertex in object space
    v_position = a_vertex;
    v_world_position = (u_model * vec4(v_position, 1.0)).xyz;
    
    // Pass the color to the fragment shader
    v_color = a_color;

    // Pass the texture coordinates to the fragment shader
    v_uv = a_uv;

    // Calculate the position of the vertex using the matrices
    gl_Position = u_viewprojection * vec4(v_world_position, 1.0);
}
