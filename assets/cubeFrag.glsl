uniform sampler2DRect tex;

void main(void)
{
	vec4 c = texture2DRect(tex, gl_TexCoord[0].xy);
	gl_FragColor = vec4(c.xyz, 1.0);
}
