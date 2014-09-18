//
// pixelModelVert
// 
// Used by PixelModelDirector. Renders PixelModel's (Robotron monsters composed of cubes).
// Color cycling effects: Two specific cube colors (0x8888ff, 0x888888, 0x888800) will be replaced 
//  with uniform colors, which are updated each frame.
//

uniform vec4 colorCycle0;	// 0x8888ff -> becomes cc0
uniform vec4 colorCycle1;	// 0x888888 -> becomes cc1
uniform vec4 colorCycle2;	// 0x888800 -> becomes cc2

void main()
{
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	
	// Color cycling: Replace two specific colors (0x888888, 0x8888ff) with colorCycle0 or colorCycle1.
	//                Both values have 50% R&G, so test those values first.
	if( (gl_Color.r<0.55) && (gl_Color.g<0.55) && (gl_Color.r>0.45) && (gl_Color.g>0.45) ) {

		// Choose either colorCycle0 or colorCycle1, depending on gl_Color blue value
		if( gl_Color.b < 0.75 ) {
			// Use colorCycle0
			//gl_FrontColor = vec4( 1.0, 0.5, 0, 1.0 );	// 0xff8800 orange
			gl_FrontColor = colorCycle0;
			
		} else {
			// Use colorCycle1
			//gl_FrontColor = vec4( 1.0, 1.0, 0, 1.0 );	// 0xffff00 yellow
			if( gl_Color.b > 0.25 ) {
				gl_FrontColor = colorCycle1;
			} else {
				gl_FrontColor = colorCycle2;
			}
		}
		
	} else {
		// Use the default vertex color
		gl_FrontColor = gl_Color;
	}
}

/*
FIXME

Vec4f cc0( 1.0f, 0.0f, 1.0f, 1.0f );
mCubeShader.uniform("colorCycle0",cc0);
Vec4f cc1( 1.0f, 1.0f, 0.0f, 1.0f );
mCubeShader.uniform("colorCycle1",cc1);
*/
