#version 300 es

precision mediump float;

// Interpolated values from the vertex shader
in vec2 UV;

// Ouput data
out vec4 color;

// Values that stay constant for the whole mesh
uniform sampler2D InputTex1;
uniform sampler2D InputTex2;
uniform float Time;

void main()
{
  const float gamma = 1.2;

  vec2 uv = UV;

  // underwater distorsion
  uv.x = UV.x + sin(UV.x * 20.0 + Time * 2.0) * 0.005;
  uv.y = UV.y + sin(UV.y * 20.0 + Time * 2.0) * 0.005;

  vec3 hdrColor = texture(InputTex1, uv).rgb + texture(InputTex2, uv).rgb;

  // underwater blueish
  hdrColor.r *= 0.5;
  hdrColor.g *= 0.5;
  hdrColor.b *= 1.5;

  // reinhard tone mapping
  vec3 mapped = hdrColor / (hdrColor + vec3(1.0));

  // gamma correction
  mapped = pow(mapped, vec3(1.0 / gamma));

  color = vec4(mapped, 1.0);
}

// vim: syntax=glsl
