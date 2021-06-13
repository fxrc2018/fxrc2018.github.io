#iChannel0 "file://sakura1.jpg"

const int kSize = (5 - 1)/2;
// 预计算得到的权重值
const float kernel[5] = float[5](0.0545,0.2442,0.4026,0.2442,0.0545);
const int blurSize = 2;

float luminance(vec4 color){
    return 0.2125 * color.r + 0.7154 * color.g + 0.0721 * color.b;
}

vec4 getBlurColor(vec2 fragCoord){
    vec4 color = vec4(0.0);
	for(int i=-kSize;i<=kSize;i++){
		for(int j=-kSize;j<=kSize;j++){
			// 采样相邻的像素
            vec2 offset = vec2(float(i * blurSize),float(j * blurSize));
			vec4 c = texture(iChannel0,(fragCoord.xy + offset)/iResolution.xy);
			color +=  c * kernel[i + kSize] * kernel[j+kSize];
		}
	}
    float val = clamp(luminance(color) - 0.2,0.0,1.0);
    return color * val;
}

void mainImage(out vec4 fragColor, in vec2 fragCoord){
    vec4 color = texture(iChannel0,fragCoord.xy / iResolution.xy);
    vec4 blurColor = getBlurColor(fragCoord);
	fragColor = color + blurColor;
}