#version 150
#extension GL_ARB_explicit_attrib_location : enable

uniform vec2 uViewportSize;
uniform vec2 uFBOSize;
uniform vec2 uMouseLocation;
uniform vec4 uInkColor;
uniform sampler2D uTexture;
uniform sampler2D uSensorData;
uniform sampler2D uSensorDataHighRez;
uniform sampler2D uPreviousSensorInput;
uniform float uMouseRadius;
uniform float uMouseDown;
uniform float uMousePressure;
uniform float uDecayRate;
uniform float uDT;
uniform float uMinPressure;
uniform float uSmoothing;
in vec2 vUV;
//out vec4[4] color;

layout ( location = 0 ) out vec4 color;
layout ( location = 1 ) out vec4 color1;
layout ( location = 2 ) out vec4 color2;
layout ( location = 3 ) out vec4 color3;

layout(origin_upper_left) in vec4 gl_FragCoord;

//FIXME: Make the dist robust to resized viewports.
void main()
{
  const float colorBias = 200.0;
  //Fixme: Don't use a fudgeFactor for alpha
  const float alphaBoostFudgeFactor = 100.0;

  vec4 orig_color = vec4(uInkColor.rgb, 0.0);
  vec4 mouse_color = vec4(uInkColor.rgb,1.0);

  float dist = length(
                      vec2(gl_FragCoord) - 
                      vec2(
                           uMouseLocation.x - uMouseRadius,
                           uMouseLocation.y + uMouseRadius
                          )
                     );

  const ivec2 off = ivec2(1,0);
  vec2 uv2 = vec2(vUV.x, 1.0 - vUV.y);
  vec4 previousColor = texture(uTexture, vUV);
  vec4 blurredPreviousColor = previousColor;
  blurredPreviousColor += textureOffset(uTexture, vUV, off.xy);
  blurredPreviousColor += textureOffset(uTexture, vUV, off.yx);
  blurredPreviousColor += textureOffset(uTexture, vUV, -off.yx);
  blurredPreviousColor += textureOffset(uTexture, vUV, -off.xy);
  //blurredPreviousColor *= 0.2;
//the corners
  const ivec2 offCorner = ivec2(1,-1);
  blurredPreviousColor += textureOffset(uTexture, vUV, offCorner.xx);
  blurredPreviousColor += textureOffset(uTexture, vUV, offCorner.yx);
  blurredPreviousColor += textureOffset(uTexture, vUV, offCorner.yy);
  blurredPreviousColor += textureOffset(uTexture, vUV, offCorner.xy);
  blurredPreviousColor *= 0.1111111;// = 1.0/9.0

  vec4 previousSensorValue = texture(uPreviousSensorInput, vUV);

  vec4 blurredPreviousValue = previousSensorValue;
  blurredPreviousValue += textureOffset(uPreviousSensorInput, vUV, off.xy);
  blurredPreviousValue += textureOffset(uPreviousSensorInput, vUV, off.yx);
  blurredPreviousValue += textureOffset(uPreviousSensorInput, vUV, -off.yx);
  blurredPreviousValue += textureOffset(uPreviousSensorInput, vUV, -off.xy);
  blurredPreviousValue *= 0.2;

  float s =  texture(uSensorDataHighRez,uv2).r;

  float sensorOn = step(uMinPressure, s);
  s = step(uMinPressure,s) * s * 0.1;

  float t =  1.0 - step(uMouseRadius, dist);

  float mouseWriting = step(uMinPressure, t*uMouseDown);

  t = t * uMouseDown + s * colorBias;
  t = clamp(t, 0,1);

  const float mouseHeightScaling = 0.33;
  float thisFrameMouseAlpha = uMousePressure * uMouseDown *  mouseHeightScaling *
                                      (1.0 - smoothstep(0,uMouseRadius, dist));
  //By using a step function there is no fade in.  THe paint has a hard edge
  float tStepped = step(uMinPressure,t);
  vec3 m = mix(previousColor.rgb, mouse_color.rgb, tStepped);


  float writingToLoc = step(uMinPressure, sensorOn + mouseWriting);
  //Experiment with only adding pressure that is greater than previous
  float pressure = s + thisFrameMouseAlpha;
  float a = (pressure - previousColor.a);
  a = clamp(a, 0, 1);


  /*
  const float pressureReverseThreshold = 0.75;
  pressure *= alphaBoostFudgeFactor;
  float sgn = sign(pressure - pressureReverseThreshold);
  float debugPressure = pressure - pressureReverseThreshold;
  debugPressure = clamp(debugPressure, 0,1);
  */

  float incrementalInput = a*alphaBoostFudgeFactor;
  a += previousColor.a;
  color = vec4(m,a); //Lower left mrt
  color1 = vec4(a*alphaBoostFudgeFactor); //Lower right mrt

  vec4 prevSensorValueX = textureOffset(uPreviousSensorInput, vUV, off.xy);
  vec4 prevSensorValueY = textureOffset(uPreviousSensorInput, vUV, off.yx);
  vec2 gradient = vec2(prevSensorValueX.x - previousSensorValue.x,
                prevSensorValueY.x - previousSensorValue.x);
  gradient = normalize(gradient);// + vec2(1.0) * 0.5;
  color2 = vec4(gradient,0,1); //upper left mrt
  //color2 = vec4(debugPressure,debugPressure,debugPressure,1); //upper left mrt

  //This line decays the sensor input over time.  
  //by using incrementalInput it only accumulates pressures above previously 
  //captured for the current pixel
  //This one uses a blurred version of the previous sensor input
  //vec3 cumulative = incrementalInput + uDecayRate * (1.0-uDT) * 
  //                    blurredPreviousValue.xyz;
  vec3 cumulative = writingToLoc + uDecayRate * (1.0-uDT) 
                        * previousSensorValue.xyz;

  color3 = vec4(cumulative,1); //upper right mrt

  //Smooth color based upon sensor input happening recently in the area
  //We should only care about accumulated sensor input.
  //if there was recent sensor input but it was below the amount previously
  //there then we should not count it.
  color = mix(color, blurredPreviousColor,cumulative.x * 0.01 * uSmoothing);

  //color = clamp(color, vec4(0), vec4(0.8, 0.8, 0.8, 0.6));

}
