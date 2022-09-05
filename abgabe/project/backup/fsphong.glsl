#version 400

in vec3 Position;
in vec3 Normal;
in vec2 Texcoord;

out vec4 FragColor;

uniform vec3 LightPos;
uniform vec3 LightColor;

uniform vec3 EyePos;
uniform vec3 DiffuseColor;
uniform vec3 SpecularColor;
uniform vec3 AmbientColor;
uniform float SpecularExp;
uniform sampler2D DiffuseTexture;

const int MAX_LIGHTS=14;
struct Light
{
	int Type;
	vec3 Color;
	vec3 Position;
	vec3 Direction;
	vec3 Attenuation;
	vec3 SpotRadius;
	int ShadowIndex;
};

uniform Lights 
{
	int LightCount;
	Light lights[MAX_LIGHTS];
};

float sat( in float a)
{
    return clamp(a, 0.0, 1.0);
}
/*
void main()
{
    vec4 DiffTex = texture( DiffuseTexture, Texcoord);
    if(DiffTex.a <0.3f) discard;
    vec3 N = normalize(Normal);
    vec3 L = normalize(LightPos-Position);
    vec3 E = normalize(EyePos-Position);
    vec3 R = reflect(-L,N);
    vec3 H = normalize(E+L);
    vec3 DiffuseComponent = LightColor * DiffuseColor * sat(dot(N,L));
    //vec3 SpecularComponent = LightColor * SpecularColor * pow( sat(dot(R,E)), SpecularExp);
    vec3 SpecularComponent = LightColor * SpecularColor * pow( sat(dot(N,H)), SpecularExp);
    FragColor = vec4((DiffuseComponent + AmbientColor)*DiffTex.rgb + SpecularComponent ,DiffTex.a);
}
*/

void main()
{
    vec4 DiffTex = texture( DiffuseTexture, Texcoord);
    if(DiffTex.a <0.3f) discard;
    vec3 N = normalize(Normal);
    vec3 E = normalize(EyePos-Position);

	vec3 Color = vec3(0,0,0);
	for(int i=0; i<LightCount;i++){
        //point light
        if(lights[i].Type == 0){
            vec3 L = normalize(lights[i].Position-Position);
            vec3 H = normalize(E+L);
            float Distance = length(lights[i].Position - Position);
            float Intensity = 1.0f/(lights[i].Attenuation.x + lights[i].Attenuation.y*Distance + lights[i].Attenuation.z*Distance*Distance);

            vec3 DiffuseComponent = Intensity * lights[i].Color * DiffuseColor * sat(dot(N,L));
            vec3 SpecularComponent = Intensity * lights[i].Color * SpecularColor * pow( sat(dot(N,H)), SpecularExp);
            Color += (DiffuseComponent + AmbientColor)*DiffTex.rgb + SpecularComponent;
        } //directional
        else if(lights[i].Type == 1){
            vec3 L = normalize(-lights[i].Direction);
            vec3 H = normalize(E+L);
            vec3 DiffuseComponent = lights[i].Color * DiffuseColor * sat(dot(N,L));
            vec3 SpecularComponent = lights[i].Color * SpecularColor * pow( sat(dot(N,H)), SpecularExp);

            Color += (DiffuseComponent + AmbientColor)*DiffTex.rgb + SpecularComponent;
        } //spot light
        else if(lights[i].Type == 2){
            vec3 L = normalize(lights[i].Position-Position); //lightDir in learnopengl
            vec3 H = normalize(E+L);
            float Distance = length(lights[i].Position - Position);
            float Intensity = 1.0f/(lights[i].Attenuation.x + lights[i].Attenuation.y*Distance + lights[i].Attenuation.z*Distance*Distance);
            float spotlightCalc = 1 - sat((dot(L,lights[i].Direction)-lights[i].SpotRadius.x) / lights[i].SpotRadius.y - lights[i].SpotRadius.x);

            vec3 DiffuseComponent = Intensity * spotlightCalc * lights[i].Color * DiffuseColor * sat(dot(N,L));
            vec3 SpecularComponent = Intensity * spotlightCalc *lights[i].Color * SpecularColor * pow( sat(dot(N,H)), SpecularExp);
            
            Color += (DiffuseComponent + AmbientColor)*DiffTex.rgb + SpecularComponent;
            //use SpotRadius x und y
        }
        
    }
		
		
	FragColor = vec4(Color,DiffTex.a);
}
