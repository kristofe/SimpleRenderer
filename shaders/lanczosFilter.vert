#version 410
in vec3 position;
in vec3 normal;
in vec2 uv;
in vec4 tangent;
in vec4 color;


 uniform float textureWidth;
 uniform float textureHeight;

 out vec2 centerTextureCoordinate;
 out vec2 oneStepLeftTextureCoordinate;
 out vec2 twoStepsLeftTextureCoordinate;
 out vec2 threeStepsLeftTextureCoordinate;
 out vec2 fourStepsLeftTextureCoordinate;
 out vec2 oneStepRightTextureCoordinate;
 out vec2 twoStepsRightTextureCoordinate;
 out vec2 threeStepsRightTextureCoordinate;
 out vec2 fourStepsRightTextureCoordinate;

 void main()
 {
     gl_Position = vec4(position, 1.0);
     float texelWidthOffset = 0.5*(1.0f/textureWidth);
     float texelHeightOffset = 0.5*(1.0f/textureHeight);

     vec2 firstOffset = vec2(texelWidthOffset, texelHeightOffset);
     vec2 secondOffset = vec2(2.0 * texelWidthOffset, 2.0 * texelHeightOffset);
     vec2 thirdOffset = vec2(3.0 * texelWidthOffset, 3.0 * texelHeightOffset);
     vec2 fourthOffset = vec2(4.0 * texelWidthOffset, 4.0 * texelHeightOffset);

     centerTextureCoordinate = uv;
     oneStepLeftTextureCoordinate = uv - firstOffset;
     twoStepsLeftTextureCoordinate = uv - secondOffset;
     threeStepsLeftTextureCoordinate = uv - thirdOffset;
     fourStepsLeftTextureCoordinate = uv - fourthOffset;
     oneStepRightTextureCoordinate = uv + firstOffset;
     twoStepsRightTextureCoordinate = uv + secondOffset;
     threeStepsRightTextureCoordinate = uv + thirdOffset;
     fourStepsRightTextureCoordinate = uv + fourthOffset;
 }
