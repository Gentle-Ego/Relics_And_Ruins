// Dichiarazioni di input
in vec4 vertexPosition;  // Posizione del vertice
in vec2 texCoords;       // Coordinate della texture

// Dichiarazioni di output
out vec2 fragTexCoords;  // Coordinate della texture passate al fragment shader

void main() {
    // Impostiamo la posizione finale del vertice
    gl_Position = vertexPosition;

    // Passiamo le coordinate della texture al fragment shader
    fragTexCoords = texCoords;
}
