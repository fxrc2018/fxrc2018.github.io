#iChannel0 "file://sakura0.jpg"

const int kSize = (5 - 1)/2;
// 预计算得到的权重值
const float kernel[5] = float[5](0.0545,0.2442,0.4026,0.2442,0.0545);
const int blurSize = 2;

void mainImage(out vec4 fragColor, in vec2 fragCoord){
	vec4 color = vec4(0.0);
	for(int i=-kSize;i<=kSize;i++){
		for(int j=-kSize;j<=kSize;j++){
			// 采样相邻的像素
			vec4 c = texture(iChannel0,(fragCoord.xy + vec2(float(i*blurSize),float(j*blurSize)) )/iResolution.xy);
			color +=  c * kernel[i + kSize] * kernel[j+kSize];
		}
	}
	fragColor = color;
}