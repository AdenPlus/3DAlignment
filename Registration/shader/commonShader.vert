#version 400

in vec3 vertTexCoord;
out vec2 fragTexCoord;
uniform mat4 projectionMatrix;

void main() {
    gl_Position = projectionMatrix * vec4(vertTexCoord, 1.0);
    fragTexCoord = vec2(vertTexCoord.x, vertTexCoord.y);
}
