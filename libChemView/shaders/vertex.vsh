attribute highp vec3 modelSpaceVertexPos;
attribute highp vec3 modelSpaceVertexNormal;

varying highp vec3 worldSpacePos;
varying highp vec3 cameraSpaceNormal;
varying highp vec3 cameraSpaceEyeDirection;
varying highp vec3 cameraSpaceLightDirection;
varying lowp vec4 col;

uniform highp mat4 model;
uniform highp mat4 view;
uniform highp mat4 projection;
uniform highp vec3 worldSpaceLightPosition;

void main(void)
{
    gl_Position = projection * view * model * vec4(modelSpaceVertexPos, 1);
    worldSpacePos = (model * vec4(modelSpaceVertexPos ,1));

    vec3 cameraSpaceVertexPos = (view * model * vec4(modelSpaceVertexPos ,1));
    cameraSpaceEyeDirection = vec3(0,0,0) - cameraSpaceVertexPos;

    vec3 cameraSpaceLightPos = (view * vec4(worldSpaceLightPosition ,1));
    cameraSpaceLightDirection = cameraSpaceLightPos + cameraSpaceEyeDirection;

    cameraSpaceNormal = (view * model * vec4(modelSpaceVertexNormal, 0));
};
