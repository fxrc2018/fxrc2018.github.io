const int MAX_STEPS = 100;
const float MAX_DIST =  100.0;
const float SURF_DIST = 0.01;

//计算一个点到场景的距离是多少
float getDist(vec3 p){
    vec4 sphere = vec4(0,1,6,1);
    float sphereDist = length(sphere.xyz-p)-sphere.w;
    float planeDist = p.y;
    float d = min(sphereDist,planeDist);
    return d;
}

//得到法线
vec3 getNormal(vec3 p){
    float d = getDist(p);
    vec2 e = vec2(0.01,0.0);
    vec3 n = d - vec3(
        getDist(p-e.xyy),
        getDist(p-e.yxy),
        getDist(p-e.yyx)
    );
    return normalize(n);
}

//光线前进
float rayMarching(vec3 ro, vec3 rd){
    float d = 0.0;
    for(int i=0;i<MAX_STEPS;i++){
        vec3 p = ro + rd*d;
        float ds = getDist(p); //每次前进到场景的距离
        d += ds;
        //直到和场景的某个物体相交或到达场景之外
        if(d > MAX_DIST || ds < SURF_DIST){
            break;
        }
    }
    return d;
}

float getLight(vec3 p){
    vec3 lightPos = vec3(0.0,5.0,6.0);
    lightPos.xz += vec2(cos(iTime),sin(iTime))*2.0;
    vec3 l = normalize(lightPos - p);
    vec3 n = getNormal(p);
    float hitDist = rayMarching(p+n*SURF_DIST*2.0,l);
    float dif = clamp(dot(n,l),0.0,1.0); //计算从该点到达光源的距离
    if(hitDist<length(lightPos-p)){ // 没有达到光源，中间被物体挡住了，此时处于阴影中
        dif *= 0.1;
    }
    return dif;
}

void mainImage(out vec4 fragColor, in vec2 fragCoord){
    vec2 uv = (fragCoord - 0.5*iResolution.xy)/iResolution.y;
    vec3 col = vec3(0.0);
    vec3 ro = vec3(0.0,1.0,0.0); //ray origin 相机的位置
    vec3 rd = normalize(vec3(uv.x,uv.y,1)); //屏幕在相机前1个单位
    float d = rayMarching(ro,rd);
    vec3 p = ro + rd*d;
    d /= 6.0;
    float dif = getLight(p);
    col = vec3(dif);
    fragColor = vec4(col,1.0);
}