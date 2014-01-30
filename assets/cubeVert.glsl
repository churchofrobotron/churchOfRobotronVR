uniform sampler2DRect tex;

varying vec4 pixelColor;

void main()
{
  vec3 normal = gl_Normal;
  vec3 lightDir = vec3(0.0, -1.0, 0.0);
  float NdotL = max(dot(normal, lightDir), 0.0);
  NdotL = min(1.0, NdotL + 0.2);

  pixelColor = texture2DRect(tex, gl_MultiTexCoord0.xy) * NdotL;
  float offset = (pixelColor.x+pixelColor.y+pixelColor.z) > 4.0/255.0 ? -0.05 : 0.0;
  vec4 v = gl_Vertex;
  v.y += v.y < 0.0 ? offset : 0.0;

  gl_Position = gl_ModelViewProjectionMatrix * v;
  gl_TexCoord[0] = gl_MultiTexCoord0;
  gl_FrontColor = gl_Color;
}

