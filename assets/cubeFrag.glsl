varying vec4 pixelColor;

void main(void)
{
	gl_FragColor = vec4(pixelColor.xyz, 1.0);
}
