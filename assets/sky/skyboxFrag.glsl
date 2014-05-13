uniform samplerCube skyTexture;

void main()
{
	gl_FragColor = vec4(textureCube(skyTexture, gl_TexCoord[0].xyz).xyz, 1.0);
  // gl_FragColor = vec4(abs(gl_TexCoord[0].xyz), 1.0);
}
