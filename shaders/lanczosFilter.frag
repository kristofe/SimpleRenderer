#version 410
uniform sampler2D inputImageTexture;

in vec2 centerTextureCoordinate;
in vec2 oneStepLeftTextureCoordinate;
in vec2 twoStepsLeftTextureCoordinate;
in vec2 threeStepsLeftTextureCoordinate;
in vec2 fourStepsLeftTextureCoordinate;
in vec2 oneStepRightTextureCoordinate;
in vec2 twoStepsRightTextureCoordinate;
in vec2 threeStepsRightTextureCoordinate;
in vec2 fourStepsRightTextureCoordinate;

 // sinc(x) * sinc(x/a) = (a * sin(pi * x) * sin(pi * x / a)) / (pi^2 * x^2)
 // Assuming a Lanczos constant of 2.0, and scaling values to max out at x = +/- 1.5

out vec4 color;

void main(void )
{
   vec4 fragmentColor = texture(inputImageTexture, centerTextureCoordinate) * 0.38026;

   fragmentColor += texture(inputImageTexture, oneStepLeftTextureCoordinate) * 0.27667;
   fragmentColor += texture(inputImageTexture, oneStepRightTextureCoordinate) * 0.27667;

   fragmentColor += texture(inputImageTexture, twoStepsLeftTextureCoordinate) * 0.08074;
   fragmentColor += texture(inputImageTexture, twoStepsRightTextureCoordinate) * 0.08074;

   fragmentColor += texture(inputImageTexture, threeStepsLeftTextureCoordinate) * -0.02612;
   fragmentColor += texture(inputImageTexture, threeStepsRightTextureCoordinate) * -0.02612;

   fragmentColor += texture(inputImageTexture, fourStepsLeftTextureCoordinate) * -0.02143;
   fragmentColor += texture(inputImageTexture, fourStepsRightTextureCoordinate) * -0.02143;

   color = fragmentColor;
}
