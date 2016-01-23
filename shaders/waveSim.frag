#version 150
uniform vec2 uViewportSize;
uniform vec2 uFBOSize;
uniform vec2 uMouseLocation;
uniform sampler2D uTexture;
uniform sampler2D uSensorData;
uniform sampler2D uSensorDataHighRez;
uniform float uMouseRadius;
uniform float uMouseDown;
uniform float uMousePressure;
uniform float uDamping;
in vec2 vUV;
out vec4 color;

layout(origin_upper_left) in vec4 gl_FragCoord;

void main()
{
  vec4 orig_color = vec4(0, 0, 0, 1.0);
  vec4 mouse_color = vec4(-0.005, 0.0, 0.0, 1.0) * uMousePressure;

  float dist = length(
                      vec2(gl_FragCoord) - 
                      vec2(
                           uMouseLocation.x - uMouseRadius,
                           uMouseLocation.y + uMouseRadius
                          )
                     );
  vec2 offset = 1.0/uFBOSize;
  vec2 hoffset = vec2(offset.x, 0);
  vec2 voffset = vec2(0,offset.y);

  vec2 uv2 = vec2(vUV.x, 1.0 - vUV.y);
  vec4 sensorInput =  texture(uSensorDataHighRez,uv2);
  sensorInput = mix(orig_color, mouse_color, sensorInput);

  vec4 s =  texture(uTexture,vUV);
  float average =  (
     texture(uTexture,vUV+hoffset).r +
     texture(uTexture,vUV-hoffset).r + 
     texture(uTexture,vUV+voffset).r + 
     texture(uTexture,vUV-voffset).r
     ) * 0.25;
  s.g += (average - s.r) * 1.0;
  s.g *= uDamping;
  s.r += s.g;
  s.r *= uDamping;
  s.b = 0.0;

  float t =  smoothstep(0.0, uMouseRadius, dist);

  vec4 m = mix(mouse_color, orig_color, t) * uMouseDown;
  color = m + s;

  //color.r = clamp(-0.1, 0.1, color.r + sensorInput.r);//This seems to break the sim on nVidia cards.

}
