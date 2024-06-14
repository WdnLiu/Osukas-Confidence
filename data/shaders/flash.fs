#version 330 core

in vec2 TexCoords;

out vec4 FragColor;

uniform float time;       // Elapsed time
uniform vec2 resolution;  // Screen resolution
uniform sampler2D u_texture;

void main()
{
    // Normalize texture coordinates to range from -1 to 1
    vec2 uv = TexCoords;
    uv.x *= resolution.x / resolution.y; // Adjust for aspect ratio

    // Calculate the distance from the center of the screen
    float dist = length(uv);

    // Calculate the expanding radius based on time
    float radius = mod(time, 2.0); // Radius will loop every 2 seconds

    // Create a smooth edge for the circle
    float smoothEdge = 0.02; // Adjust for softer/harder edge
    float circle = smoothstep(radius - smoothEdge, radius + smoothEdge, dist);

    // Set the color of the circle
    vec3 circleColor = vec3(1.0, 1.0, 1.0); // White circle
    vec4 bgColor = texture(u_texture, uv);

    // Mix the circle color and background color based on the circle's smooth edge
    vec3 finalColor = mix(circleColor, bgColor.xyz, circle);

    FragColor = vec4(finalColor, 1.0);
}
