varying highp vec3 worldSpacePos;
varying highp vec3 cameraSpaceNormal;
varying highp vec3 cameraSpaceEyeDirection;
varying highp vec3 cameraSpaceLightDirection;

uniform highp mat4 model;
uniform highp mat4 view;
uniform highp vec3 worldSpaceLightPosition;
uniform lowp vec3 color;

void main(void)
{
    vec3 LightColor = vec3(1,1,1);
    float LightPower = 200.0f;

    vec3 MaterialDiffuseColor = color.xyz;
    vec3 MaterialAmbientColor = vec3(0.2,0.2,0.2) * MaterialDiffuseColor;
    vec3 MaterialSpecularColor = vec3(0.3,0.3,0.3);

    float distance = length(worldSpaceLightPosition - worldSpacePos);

    vec3 n = normalize(cameraSpaceNormal);
    vec3 l = normalize(cameraSpaceLightDirection);
    float cosTheta = clamp(dot(n, l), 0,1);

    vec3 E = normalize(cameraSpaceEyeDirection);
    vec3 R = reflect(-l, n);
    float cosAlpha = clamp(dot(E, R), 0, 1);

    gl_FragColor = vec4(
                MaterialAmbientColor +
                MaterialDiffuseColor * LightColor * LightPower * cosTheta / (distance * distance) +
                MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha,5) / (distance * distance), 1);
}
