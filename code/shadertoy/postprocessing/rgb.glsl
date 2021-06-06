#iChannel0 "file://sakura0.jpg"

float brightness = 1.0; // 亮度
float saturation = 1.0; // 饱和度
float contrast = 1.0; // 对比度

void mainImage(out vec4 fragColor, in vec2 fragCoord){
    vec2 uv = fragCoord.xy / iResolution.xy;
    vec4 textureColor = texture(iChannel0, uv);
    
    // 亮度
    vec3 finalColor = textureColor.rgb * brightness;
    
    // 饱和度
    float luminance = 0.2125 * textureColor.r + 0.7154 * textureColor.g + 0.0721 * textureColor.b;
    vec3 luminanceColor = vec3(luminance);
    finalColor = mix(luminanceColor, finalColor, saturation);
    
    // 对比度    
    vec3 avgColor = vec3(0.5,0.5,0.5);
    finalColor = mix(avgColor,finalColor,contrast);

    fragColor = vec4(finalColor,textureColor.a);
}