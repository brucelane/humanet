uniform sampler2D colormap;
uniform sampler2D bumpmap;
varying vec2  TexCoord;
uniform float maxHeight;
uniform vec3 twod;
uniform vec4 iBackgroundColor;
uniform float time;

void main(void) {
	//if (maxHeight > 190.0)
	//{
	//	gl_FragColor = texture2D(colormap, TexCoord) + iBackgroundColor;
	//}
	//else {
		gl_FragColor = texture2D(colormap, TexCoord);
	//}
}