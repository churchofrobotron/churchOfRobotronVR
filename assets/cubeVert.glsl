uniform sampler2DRect tex;

varying vec4 pixelColor;

void main()
{
  vec3 normal = gl_Normal;
  vec3 lightDir = vec3(0.0, -1.0, 0.0);
  float NdotL = max(dot(normal, lightDir), 0.0);
  NdotL = min(1.0, NdotL + 0.2);

  pixelColor = texture2DRect(tex, gl_MultiTexCoord0.xy);

  float intensity = dot(pixelColor.xyz, vec3(0.2989, 0.5870, 0.1140));
  //float offset = intensity * -0.05;
  float offset = intensity > 4.0/255.0 ? ((-0.015 * intensity) + -0.04) : 0.0;

  pixelColor *= NdotL;

  vec4 v = gl_Vertex;
  v.y += v.y < 0.0 ? offset : 0.0;

  gl_Position = gl_ModelViewProjectionMatrix * v;
  gl_TexCoord[0] = gl_MultiTexCoord0;
  gl_FrontColor = gl_Color;
}

