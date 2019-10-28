uniform sampler2D colormap;
uniform sampler2D bumpmap;
varying vec2  TexCoord;
uniform float maxHeight;
uniform vec3 twod;
uniform float time;

in vec4 position;
uniform mat4 modelViewProjectionMatrix;

void main(void) {
    TexCoord = gl_MultiTexCoord0.st;

    vec4 bumpColor = texture2D(bumpmap, TexCoord);
    float df = twod.x*bumpColor.x + twod.y*bumpColor.y + twod.z*bumpColor.z;
    vec4 newVertexPos = vec4(gl_Normal * df * maxHeight, 0.0) + gl_Vertex; // without 2D lpm

	//newVertexPos = mix(newVertexPos, vec4(TexCoord, 0, 1), fract(time*0.1));
	//newVertexPos = mix(newVertexPos, vec4(TexCoord * vec2(1.0,1.0), 0.5, 1), fract(time*0.1));
	//newVertexPos = mix(newVertexPos, vec4(TexCoord * vec2(0.5,-0.5), 0.5, 0.5), 1.0);
	//newVertexPos = vec4(TexCoord * vec2(1.0,1.0), 0.0, 0.1);
	//newVertexPos = mix(newVertexPos, vec4(TexCoord*vec2(1,-1) * 100, 0, 1), 1);
	newVertexPos = mix(newVertexPos, vec4(TexCoord*vec2(1,-1) * 800.0, 0.0, 1.0), fract( (40.0 - time) * 0.02));
	//newVertexPos = vec4(TexCoord*vec2(1,-1) * 800.0, 0.0, 1.0);
	gl_Position = gl_ModelViewProjectionMatrix * newVertexPos;
}
/*
uniform sampler2D colormap;
uniform sampler2D bumpmap;
varying vec2  TexCoord;
uniform float maxHeight;
uniform vec3 twod;
uniform float time;

in vec4 position;
uniform mat4 modelViewProjectionMatrix;

void main(void) {
    TexCoord = gl_MultiTexCoord0.st;

    vec4 bumpColor = texture2D(bumpmap, TexCoord);
    float df = twod.x*bumpColor.x + twod.y*bumpColor.y + twod.z*bumpColor.z;
    vec4 newVertexPos = vec4(gl_Normal * df * maxHeight, 0.0) + gl_Vertex;

	//newVertexPos = mix(newVertexPos, vec4(TexCoord, 0, 1), fract(time*0.1));
	//newVertexPos = mix(newVertexPos, vec4(TexCoord * vec2(1.0,1.0), 0.5, 1), fract(time*0.1));
	//newVertexPos = mix(newVertexPos, vec4(TexCoord * vec2(0.5,-0.5), 0.5, 0.5), 1.0);
	//newVertexPos = vec4(TexCoord * vec2(1.0,1.0), 0.0, 0.1);
	//newVertexPos = mix(newVertexPos, vec4(TexCoord*vec2(1,-1) * 100, 0, 1), 1);
	//newVertexPos = mix(newVertexPos, vec4(TexCoord*vec2(1,-1) * 800.0, 0.0, 1.0),  fract(time*0.1));
	gl_Position = gl_ModelViewProjectionMatrix * newVertexPos;
}*/