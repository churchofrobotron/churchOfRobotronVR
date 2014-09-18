//
// pixelModelVert
// 
// Used by PixelModelDirector. Renders PixelModel's (Robotron monsters composed of cubes).
// Color cycling effects: Two specific cube colors (0x888888, 0x8888ff) will be replaced 
//  with uniform colors, which are updated each frame.
//

//uniform vec4 cycleColor0;
//uniform vec4 cycleColor1;

void main()
{
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	
	// Color cycling: Replace two specific colors (0x888888, 0x8888ff) with cycleColor0 or cycleColor1.
	//                Both values have 50% R&G, so test those values first.
	if( (gl_Color.r<0.55) && (gl_Color.g<0.55) && (gl_Color.r>0.45) && (gl_Color.g>0.45) ) {

		// Choose either cycleColor0 or cycleColor1, depending on gl_Color blue value
		if( gl_Color.b < 0.75 ) {
			// Use cycleColor0
			gl_FrontColor = vec4( 1.0, 0.5, 0, 1.0 );	// 0xff8800 orange
			
		} else {
			// Use cycleColor1
			gl_FrontColor = vec4( 1.0, 1.0, 0, 1.0 );	// 0xffff00 yellow
		}
		
	} else {
		// Use the default vertex color
		gl_FrontColor = gl_Color;
	}
}

/*
FIXME

Vec4f cc0( 1.0f, 0.0f, 1.0f, 1.0f );
mCubeShader.uniform("cycleColor0",cc0);
Vec4f cc1( 1.0f, 1.0f, 0.0f, 1.0f );
mCubeShader.uniform("cycleColor1",cc1);
*/
