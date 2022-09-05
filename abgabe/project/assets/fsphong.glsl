#version 400

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec3 Position;
in vec3 Normal;
in vec2 Texcoord;

in vec3 Tangent;
in vec3 Bitangent;

//out vec4 FragColor;

uniform vec3 LightPos;
uniform vec3 LightColor;

uniform vec3 EyePos;
uniform vec3 DiffuseColor;
uniform vec3 SpecularColor;
uniform vec3 AmbientColor;
uniform float SpecularExp;
uniform sampler2D DiffuseTexture;
uniform sampler2D NormalTexture;

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
void main(){
    vec3 G, H;
    mat3 MatNormal = mat3(Tangent,-Bitangent,Normal);
    vec4 NormTex = texture(NormalTexture, Texcoord);
    G.x = NormTex.r*2-1;
    G.y = NormTex.g*2-1;
    G.z = NormTex.b*2-1;

    G = MatNormal*G;
    G = normalize(G);
    FragColor = vec4(G, 1);
}*/

void main()
{
    vec3 N; 
    mat3 MatNormal = mat3(Tangent,-Bitangent,Normal);
    vec4 NormTex = texture(NormalTexture, Texcoord);
    N.x = NormTex.r*2-1;
    N.y = NormTex.g*2-1;
    N.z = NormTex.b*2-1;

    N = MatNormal*N;
    N = normalize(N);

    vec4 DiffTex = texture( DiffuseTexture, Texcoord);
    if(DiffTex.a <0.3f) discard;
    //vec3 N = normalize(Normal);
    vec3 E = normalize(EyePos-Position);

	vec3 Color = vec3(0,0,0);
	for(int i=0; i<LightCount;i++){
        //point light
        if(lights[i].Type == 0){
            vec3 L = normalize(lights[i].Position-Position);
            vec3 H = normalize(E+L); //H ist für aufgabe 1
            float Distance = length(lights[i].Position - Position);
            float Intensity = 1.0f/(lights[i].Attenuation.x + lights[i].Attenuation.y*Distance + lights[i].Attenuation.z*Distance*Distance);

            vec3 DiffuseComponent = Intensity * lights[i].Color * DiffuseColor * sat(dot(N,L));
            vec3 SpecularComponent = Intensity * lights[i].Color * SpecularColor * pow( sat(dot(N,H)), SpecularExp);
            Color += (DiffuseComponent)*DiffTex.rgb + SpecularComponent;
        } //directional
        else if(lights[i].Type == 1){
            vec3 L = normalize(-lights[i].Direction);
            vec3 H = normalize(E+L);
            vec3 DiffuseComponent = lights[i].Color * DiffuseColor * sat(dot(N,L));
            vec3 SpecularComponent = lights[i].Color * SpecularColor * pow( sat(dot(N,H)), SpecularExp);

            Color += (DiffuseComponent)*DiffTex.rgb + SpecularComponent;
        } //spot light
        else if(lights[i].Type == 2){
            vec3 L = normalize(lights[i].Position-Position); //lightDir in learnopengl
            vec3 H = normalize(E+L);
            float Distance = length(lights[i].Position - Position);
            float Intensity = 1.0f/(lights[i].Attenuation.x + lights[i].Attenuation.y*Distance + lights[i].Attenuation.z*Distance*Distance);
            float spotlightCalc = 1 - sat((acos(dot(L,normalize(-lights[i].Direction)))-lights[i].SpotRadius.x) / (lights[i].SpotRadius.y - lights[i].SpotRadius.x));
								//1 - sat((acos(dot(normalize(L),normalize(-lights[i].Direction)))-lights[i].SpotRadius.x)/(lights[i].SpotRadius.y - lights[i].SpotRadius.x));
            vec3 DiffuseComponent = Intensity * spotlightCalc * lights[i].Color * DiffuseColor * sat(dot(N,L));
            vec3 SpecularComponent = Intensity * spotlightCalc *lights[i].Color * SpecularColor * pow( sat(dot(N,H)), SpecularExp);
            
            Color += (DiffuseComponent)*DiffTex.rgb + SpecularComponent;
            //use SpotRadius x und y
        }
        
    }
		
		
	FragColor = vec4((Color + AmbientColor)*DiffTex.rgb,DiffTex.a);

    //Quelle:https://learnopengl.com/Advanced-Lighting/Bloom
    // check whether fragment output is higher than threshold, if so output as brightness color
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        BrightColor = vec4(FragColor.rgb, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);

}


/*#version 400


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

float sat( in float a)
{
    return clamp(a, 0.0, 1.0);
}

void main()
{
    vec4 DiffTex = texture( DiffuseTexture, Texcoord);
    if(DiffTex.a <0.3f) discard;
    vec3 N = normalize(Normal);
    vec3 L = normalize(LightPos-Position);
    vec3 E = normalize(EyePos-Position);
    vec3 R = reflect(-L,N);
    vec3 H = normalize(E+L); //AUFGABE 1
    vec3 DiffuseComponent;
	vec3 SpecularComponent;
	vec3 Color = vec3(0,0,0);
	

	for(int i=0; i<LightCount;i++){
	vec3 distance = lights[i].Position - Position;
	float distanceLength = length(distance);
	float attenuation = 1.0;

	//Point light
	if(lights[i].Type == 0){
	attenuation /=(lights[i].Attenuation.x + (lights[i].Attenuation.y*distanceLength) + (lights[i].Attenuation.z*distanceLength*distanceLength));
	L = normalize(lights[i].Position - Position);
	}
	//Directional Light
	else if(lights[i].Type == 1){
	L = normalize(-lights[i].Direction);
	}
	//Spotlight
	else if(lights[i].Type == 2){
	attenuation /= (lights[i].Attenuation.x + (lights[i].Attenuation.y*distanceLength) + (lights[i].Attenuation.z*distanceLength*distanceLength));
	L = normalize(lights[i].Position - Position);
	 attenuation *= 1-sat((acos(dot(normalize(L),normalize(-lights[i].Direction)))-lights[i].SpotRadius.x)/(lights[i].SpotRadius.y - lights[i].SpotRadius.x));
	}


	vec3 lColor = lights[i].Color *attenuation;
	H = normalize(E+L);
	DiffuseComponent = lColor * DiffuseColor* sat(dot(N,L));
    SpecularComponent = lColor * SpecularColor * pow( sat(dot(H,N)), SpecularExp);
	
	Color += DiffuseComponent*DiffTex.rgb + SpecularComponent;
	}
		

    FragColor = vec4(Color + AmbientColor*DiffTex.rgb  ,DiffTex.a);

    
		
	

}*/