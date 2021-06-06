#iChannel0 "file://sakura0.jpg"

vec4 edgeColor = vec4(0.0,0.0,0.0,1.0);
vec4 bgColor = vec4(1.0);
float edgeOnly = 0.0;

float luminance(vec4 color){
    return 0.2125 * color.r + 0.7154 * color.g + 0.0721 * color.b;
}

float sobel(vec4 textureColors[9]){
    const float GX[9] = float[9](
        -1.0,-2.0,-1.0,
        0.0,0.0,0.0,
        1.0,2.0,1.0);
    const float GY[9] = float[9](
        -1.0,0.0,1.0,
        -2.0,0.0,2.0,
        -1.0,0.0,1.0);
    float edgeX = 0.0;
    float edgeY = 0.0;
    float textureColor;
    for(int i=0;i<9;i++){
        textureColor = luminance(textureColors[i]);
        edgeX += textureColor * GX[i];
        edgeY += textureColor * GY[i];
    }
    // edge值越大，该点越有可能是
    float edge = 1.0 - abs(edgeX) - abs(edgeY);
    return edge;
}

void main(){
    vec4 textureColors[9];
    // 计算相邻的纹理坐标
    vec2 firstPos = gl_FragCoord.xy - vec2(1.0,1.0);
    for(int i=0;i<9;i++){
        vec2 offset = vec2(i%3,i/3);
        vec2 uv = (firstPos + offset) / iResolution.xy;
        textureColors[i] = texture(iChannel0,uv);
    }
    float edge = sobel(textureColors);
    vec4 withEdgeColor = mix(edgeColor,textureColors[4],edge);
    vec4 onlyEdgeColor = mix(edgeColor,bgColor,edge);
    vec4 finalColor = mix(withEdgeColor,onlyEdgeColor,edgeOnly);
    gl_FragColor = finalColor;
}