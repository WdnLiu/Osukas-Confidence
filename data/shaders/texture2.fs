#version 330 core

// Input varying variables from the vertex shader
in vec3 v_position;
in vec3 v_world_position;
in vec3 v_normal;
in vec2 v_uv;
in vec4 v_color;

// Uniforms for the color, texture, and time
uniform vec4 u_color;
uniform sampler2D u_texture;
uniform float u_time;
uniform float u_percentage;
uniform float u_decrease;

// Output variable for the fragment color
out vec4 FragColor;

void main() {
    vec2 uv = v_uv;
    vec4 finalColor = u_color;
    if (uv.x > u_percentage || uv.x < 0.22)
        discard;
    else if (v_uv.x > u_percentage + u_decrease) 
        finalColor = vec4(1);  

    FragColor = finalColor * texture(u_texture, uv) * (0.2 + uv.x);
}
