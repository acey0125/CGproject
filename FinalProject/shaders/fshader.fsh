#version 330 core

in vec3 worldPos;   // 当前片元的世界坐标
in vec2 texcoord;   // 纹理坐标
in vec3 normal;     // 法向量

out vec4 fColor;    // 片元输出像素的颜色

uniform sampler2D texture;  // 纹理图片
uniform sampler2D shadowtex;	// shadow纹理
uniform mat4 shadowVP;  // 转换到光源坐标的变换矩阵
uniform vec3 lightPos;  // 光源位置
uniform vec3 cameraPos; // 相机位置
uniform vec3 lightColor; //光照颜色

float shadowMapping(sampler2D tex, mat4 shadowVP, vec4 worldPos) {
	// 转换到光源坐标
	vec4 lightPos = shadowVP * worldPos;
	lightPos = vec4(lightPos.xyz/lightPos.w, 1.0);
	lightPos = lightPos*0.5 + 0.5;

	// 计算shadowmapping
	float closestDepth = texture2D(tex, lightPos.xy).r;	// shadowmap中最近点的深度
	float currentDepth = lightPos.z;	// 当前点的深度
	float isInShadow = (currentDepth>closestDepth+0.005) ? (1.0) : (0.0);

	return isInShadow;
}

float phong(vec3 worldPos, vec3 cameraPos, vec3 lightPos, vec3 normal)
{
    vec3 N = normalize(normal);
    vec3 V = normalize(worldPos - cameraPos);
    vec3 L = normalize(worldPos - lightPos);
    vec3 R = reflect(L, N);

    float ambient = 0.3;
    float diffuse = max(dot(N, -L), 0) * 0.7;
    float specular = pow(max(dot(-R, V), 0), 50.0) * 1.1;

    return ambient + diffuse + specular;
}

void main()
{
    fColor.rgb =  texture2D(texture, texcoord).rgb;

    float isInShadow = shadowMapping(shadowtex, shadowVP, vec4(worldPos, 1.0));

    if(isInShadow==0) {
        fColor.rgb *= phong(worldPos, cameraPos, lightPos, normal)*lightColor;

    } else {
        fColor.rgb *= 0.3 * lightColor;  // only ambient
    }

}


