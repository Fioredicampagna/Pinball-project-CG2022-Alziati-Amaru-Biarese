#version 450

layout(set = 0, binding = 0) uniform globalUniformBufferObject {
	mat4 view;
	mat4 proj;
	vec3 lightPos;
    vec3 lightColor; 
    vec2 lightParameters;
} gubo;

layout(set=1, binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragViewDir;
layout(location = 1) in vec3 fragNorm;
layout(location = 2) in vec2 fragTexCoord;
layout(location = 3) in vec3 fragPos;

layout(location = 0) out vec4 outColor;

vec3 point_light_dir(vec3 pos) {
    // Point light direction
    return normalize(gubo.lightPos - pos);
}

vec3 point_light_color(vec3 pos) {
    // Point light color
    
    return gubo.lightColor * pow(gubo.lightParameters.x  / length(gubo.lightPos - pos), gubo.lightParameters.y);
}


void main() {
	const vec3  diffColor = texture(texSampler, fragTexCoord).rgb;
	const vec3  specColor = vec3(1.0f, 1.0f, 1.0f);
	const float specPower = 150.0f;
    const vec3 ambColor = vec3(1.0f, 0.65f, 0.48f);
	

    vec3 lD;    // light direction from the light model
    vec3 lC;    // light color and intensity from the light model
    
    lD = point_light_dir(fragPos);
    lC = point_light_color(fragPos);
    
    vec3 N = normalize(fragNorm);
    vec3 R = -reflect(lD, N);
    vec3 V = normalize(fragViewDir);
    
	 
	// Lambert diffuse
	vec3 diffuse  = diffColor * max(dot(N,lD), 0.0f) * 1.5f;
	// Phong specular
	vec3 specular = /* specColor * */ vec3(pow(max(dot(R,V), 0.0f), specPower));
	// Hemispheric ambient
    vec3 ambient  = vec3(0.3f, 0.3f, 0.3f) * diffColor + vec3(0.02f, 0.02f, 0.02f) * ambColor;
	
	outColor = vec4(clamp(diffuse + specular, vec3(0.0f), vec3(1.0f)) * lC + ambient, 1.0f);
}
