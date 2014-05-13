void main()
{
  vec4 pos = gl_ModelViewProjectionMatrix * vec4(gl_Vertex.xyz, 0);
  gl_Position = pos.xyzz;
  // Crappy hack to fix zfighting on skybox
  gl_Position.z -= 0.00001;
  gl_TexCoord[0] = gl_Vertex;
}