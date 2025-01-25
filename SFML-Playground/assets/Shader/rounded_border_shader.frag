uniform sampler2D texture;   // Texture associata
uniform vec2 resolution;     // Dimensioni dello sprite
uniform float radius;        // Raggio dei bordi arrotondati

in vec2 fragTexCoords;  // Coordinate della texture passate dal vertex shader
out vec4 fragColor;     // Colore del frammento

void main() {
    vec2 uv = fragTexCoords; // Coordinate normalizzate della texture
    vec2 distToEdge = min(uv * resolution, resolution - uv); // Distanza ai bordi
    float alpha = smoothstep(0.0, radius, min(distToEdge.x, distToEdge.y));
    vec4 color = texture2D(texture, uv);
    fragColor = vec4(color.rgb, color.a * alpha); // Applica trasparenza
}
