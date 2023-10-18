#version 430

#define shininess 10

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
in vec4 LightFragPost;

out vec4 color;

uniform sampler2D ourTexture;
uniform sampler2D shadowMap;

uniform vec3 viewPos;
uniform vec3 fakeLightPos;

uniform int isSun;
uniform int enableShadow;

struct DirectionLight {
    vec3 position;
    vec3 direction;  
    float constant;
    float linear;
    float quadratic;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform DirectionLight dl;

float pointDis(vec3 p1, vec3 p2){
    return sqrt(pow(p1.x-p2.x,2) + pow(p1.y-p2.y,2) + pow(p1.z-p2.z,2));
}

float vecLength(vec3 v){
    return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

float ShadowCalculation()
{
    float bias = 0.002;
    // TODO
    vec3 projCoords = LightFragPost.xyz;
    projCoords = LightFragPost.xyz / LightFragPost.w;
    projCoords = projCoords * 0.5 + 0.5;
    if(projCoords.z > 1.0)
        return 0.0;

    float shadow = 0.0;
    float currentDepth = projCoords.z; 
    
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;

    return shadow;
}

void main() {
    if(isSun==1){color=vec4(texture(ourTexture, TexCoord).xyz,0);return;}
    vec3 ambient = dl.ambient;
    vec3 viewVec = normalize(viewPos-FragPos);
    vec3 direction = normalize(FragPos-dl.position);
  	
    // Diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(-direction);  
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = dl.diffuse * diff;
    
    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = dl.specular * spec;
    /*
    float P_dis = pointDis(dl.position, FragPos);
    float P_atten = 1/(dl.constant + dl.linear*P_dis + dl.quadratic*P_dis*P_dis);

    vec3 P_dir = normalize(FragPos-dl.position);
    vec3 P_lightVec = -P_dir;
    float normDotPoint = max(dot(Normal, P_lightVec), 0.0);
    vec3 diffuse = dl.diffuse * max(normDotPoint, 0.0);
    
    vec3 temp = viewVec + P_lightVec;
    vec3 P_halfDir = temp/vecLength(temp);
    float NormDotPhalf = dot(Normal, P_halfDir);
    vec3 specular = dl.specular * pow(max(NormDotPhalf, 0.0), shininess);
*/

    float shadow = enableShadow > 0 ? ShadowCalculation() : 0.0;
    vec3 total = (ambient + (1.0 - shadow) * (diffuse + specular));
    total = clamp(total, vec3(0, 0, 0), vec3(1, 1, 1));
        
    vec3 result = total * texture(ourTexture, TexCoord).xyz;
    color = vec4(result, 1.0f);
}
