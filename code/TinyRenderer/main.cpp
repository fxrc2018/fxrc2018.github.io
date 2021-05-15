#include <iostream>
#include <vector>
#include <algorithm>
#include  "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "tgaimage.h"
#include "Model.h"

const float PI = 3.14f;
#include <cmath>

using namespace std;
using namespace glm;

// 渲染上下文
TGAImage* screen = NULL;
const int W = 800;
const int H = 800;
vector<Model> scene; // 场景有许多物体
vec3 triangleWorldPos[3];
vec3 cameraWorldPos;
vec3 pointWorldPos;
vec3 triangleWorldNorm[3];
vec3 pointWorldNorm; // 插值之后点的法线
vec3 triangleUV[3]; //纹理坐标
vec3 directLight;
vec3 viewDirectLight;
vec2 pointUV;
vec2 uv23; // uv坐标
int modelIdx;
int triangleIdx;
float zbuffer[W*H];
float shadowDepth[W*H];
TGAColor background;

vector<vec4> glPosition(3,vec4(1.0f));
TGAColor glColor;
mat4 projMatrix(1.0f);
mat4 vMatrix(1.0f);
mat4 vMatrixI(1.0f);
mat4 mMatrix(1.0f);
mat4 mMatrixIT(1.0f);
mat4 mvpMatrix(1.0f);
mat4 mvMatrix(1.0f);
mat4 mvMatrixIT(1.0f);
mat4 shadowVP;
mat4 shadowP = ortho(0,W,0,H);
mat4 shadowVPIT;
// mat初始化函数，前4个参数会放入第一列
mat4 viewport(
    W/2,0,0,0,
    0,H/2,0,0,
    0,0,1.0f/2.0f,0.0f,
    W/2,H/2,1.0f/2.0f,1.0f
);
float directLightLen = 10.0f;
// 插值之后的数据 该点在屏幕上的坐标 该点的纹理坐标 该点的法线
bool isDepth;



TGAColor getRandomColor(){
    return TGAColor(rand()%255,rand()%255,rand()%255,255);
}

void init(){
    screen = new TGAImage(W,H,TGAImage::Format::RGBA);
    background = TGAColor(0,0,0,255);

    scene.push_back(Model("obj/floor.obj"));
    scene[0].mMatrix = translate(scene[0].mMatrix,vec3(1.0f,-1.0f,1.0f));
    scene[0].mMatrix = scale(scene[0].mMatrix,vec3(3.0f));
    scene.push_back(Model("obj/african_head.obj"));
    

    directLight = normalize(vec3(0,0,-1));
    cameraWorldPos = vec3(0.0f,1.0f,3.0f);
    mMatrixIT = transpose(inverse(mMatrix));
    vMatrix = lookAt(cameraWorldPos,vec3(0,0,0),vec3(0,1,0));
    projMatrix = perspective(3.14f/3.0f,(float)W/H,0.1f,1000.0f);
    // projMatrix = mat4(1.0f);
    mvMatrix = vMatrix * mMatrix;
    mvMatrixIT = transpose(inverse(mvMatrix));
    mvpMatrix = projMatrix * vMatrix * mMatrix;
    shadowP = mat4(
        1,0,0,0,
        0,1,0,0,
        0,0,1/(1000.0f-0.1f),0,
        0,0,-0.1f/(1000.0f - 0.1f),1.0f
    );
    shadowVP = shadowP * lookAt(directLight * directLightLen, vec3(0,0,0),vec3(0,1,0));
    vMatrixI = inverse(vMatrix);
}

void drawPixel(int x, int y, vec4 color){
    TGAColor c(color.x*255, color.y*255,color.z*255,255);
    screen->set(x,y,c);
}

vec3 barycentric(const vector<vec3> &triangle, vec3 P) {
    vec3 A = triangle[0];
    vec3 B = triangle[1];
    vec3 C = triangle[2];
    vec3 s[2];
    for (int i=2; i--; ) {
        s[i][0] = C[i]-A[i];
        s[i][1] = B[i]-A[i];
        s[i][2] = A[i]-P[i];
    }
    vec3 u = cross(s[0], s[1]);
    // dont forget that u[2] is integer. If it is zero then triangle ABC is degenerate
    if (std::abs(u[2])>1e-2) {
        return vec3(1.f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z);
    }
    // in this case generate negative coordinates, it will be thrown away by the rasterizator
    return vec3(-1,1,1); 
}

void drawLine(int x1, int y1, int x2, int y2, vec4 color){
    bool steep = false;
    if(abs(x2 - x1) < abs(y2 - y1)){
        steep = true; //交换x和y的含义 保证从相差较大的坐标开始遍历
        swap(x1,y1); 
        swap(x2,y2);
    }
    if(x2 < x1){ //从小到大
        swap(x1,x2);
        swap(y1,y2);
    }
    for(int x=x1;x<x2;x++){
        float t = (x - x1) / (float)(x2 - x1);
        int y = y1 + (y2 - y1) * t;
        if(steep){
            drawPixel(y,x,color); //x y交换过
        }else{
            drawPixel(x,y,color);
        }
    }
}

vec3 calcNewNorm(){
    // 构造切线空间到世界空间的变换矩阵tbn
    vec3 deltaPos1 = triangleWorldPos[1] - triangleWorldPos[0];
    vec3 deltaPos2 = triangleWorldPos[2] - triangleWorldPos[0];
    vec3 deltaUV1 = triangleUV[1] - triangleUV[0];
    vec3 deltaUV2 = triangleUV[2] - triangleUV[0];
    vec3 tangent = normalize((deltaPos1 * deltaUV2.y   - deltaPos2 * deltaUV1.y));
    vec3 bitangent = normalize((deltaPos2 * deltaUV1.x   - deltaPos1 * deltaUV2.x));
    mat3 tbn(tangent,bitangent,pointWorldNorm);

    // 从法线贴图中查询法向量，并执行转换
    vec3 nm = scene[modelIdx].normal(vec2(pointUV.x,pointUV.y));
    nm = tbn * nm;
    nm = normalize(nm);
    return nm;
}

float getShadow(){
    vec4 sp = shadowVP * vMatrixI * vec4(pointWorldPos,1.0f);
    sp /= sp.w;
    sp = viewport * sp;
    int pos = sp.x + sp.y * W;
    // if(pos >= W*H || pos < 0){
    //     return 1.0f;
    // }
    if(sp.z  < shadowDepth[pos] + 0.01f){
        return 1.0f;
    }else{
        cout<<"here"<<endl;
        return 0.0f;
    }
}

//输入屏幕上的位置 输出对应的颜色
vec4 fragment(){
    // 环境光 漫反射 高光占比为 0.1 0.7 0.2
    vec4 textureColor = scene[modelIdx].diffuse(pointUV);
    glColor = scene[modelIdx].diffuse2(pointUV);
    vec4 color = textureColor * 1.0f; // 环境光
    pointWorldNorm = calcNewNorm(); // 法线贴图
    float diff = std::max(dot(viewDirectLight, pointWorldNorm), 0.0f); 
    vec4 diffColor = textureColor * diff * 0.7f;
    // 在世界空间下计算高光
    vec3 r = 2.0f * dot(pointWorldNorm, viewDirectLight) * pointWorldNorm - viewDirectLight;
    r = normalize(r);
    vec3 eye = normalize(vec3(0,0,0) - pointWorldPos);
    float spec = pow(std::max(dot(eye,r),0.0f),50.0f);
    vec4 specColor = vec4(1.0f) * spec * 0.2f;
    // 如果不在阴影中，加上反射和高光
    color += (diffColor + specColor) * getShadow();
    color = clamp(color,0.0f,1.0f);
    if(getShadow() == 0.0f){
        color = vec4(0);
    }
    color.w = 1.0f;
    // 在切线空间下计算着色
    return color;
}

void clip(){
        // if(glPosition[i].x < -1.0f || glPosition[i].x > 1.0f || glPosition[i].y < -1.0f || glPosition[i].y > 1.0f || glPosition[i].z < -1.0f || glPosition[i].z > 1.0f){
        //     return;
        // }
}

// 负责插值
// 调用片段着色器
void drawTriangle(){
    float zval[3] = {glPosition[0].w,glPosition[1].w,glPosition[2].w}; // 先把-z值保存下来
    vector<vec3> pts(3,vec3());
    auto t = glPosition;
    for(int i=0;i<3;i++){
        glPosition[i] /= glPosition[i].w;
        pts[i] = viewport * glPosition[i]; 
    }
    vec2 bboxmin(FLT_MAX, FLT_MAX);
    vec2 bboxmax(-FLT_MAX, -FLT_MAX);
    for (int i=0; i<3; i++){ //找到上下左右的边界
        bboxmin.x = std::min(bboxmin.x,pts[i].x);
        bboxmin.y = std::min(bboxmin.y,pts[i].y);
        bboxmax.x = std::max(bboxmax.x,pts[i].x);
        bboxmax.y = std::max(bboxmax.y,pts[i].y);
    }
    //将边界修正在屏幕内
    bboxmax.x = std::min(bboxmax.x,(float)W-1);
    bboxmax.y = std::min(bboxmax.y,(float)H-1);
    bboxmin.x = std::max(bboxmin.x,0.0f);
    bboxmin.y = std::max(bboxmin.y,0.0f);
    vec3 p;
    // 一定要注意，p的类型其实是int 有个bug调了好久
    for(p.x = floor(bboxmin.x);p.x<=bboxmax.x && p.x < W;p.x++){
        for(p.y=floor(bboxmin.y);p.y<=bboxmax.y && p.y < W;p.y++){
            vec3 bc = barycentric(pts,p);
            if(bc.x <0||bc.y < 0||bc.z<0){
                continue;
            } 
            // 法线插值
            pointWorldNorm = bc.x * triangleWorldNorm[0] + bc.y * triangleWorldNorm[1] + bc.z * triangleWorldNorm[2];
            pointWorldNorm = normalize(pointWorldNorm);
            // 对点的世界坐标进行插值
            pointWorldPos = bc.x * triangleWorldPos[0] + bc.y * triangleWorldPos[1] + bc.z * triangleWorldPos[2];
            //深度插值
            float pz = bc.x* 1.0f/ zval[0] + bc.y*1.0f / zval[1] + bc.z* 1.0f / zval[2];
            pz = 1.0f / pz;
            //对z进行差值
            p.z = bc.x*pts[0].z + bc.y*pts[1].z + bc.z*pts[2].z;
           int pos = p.x + p.y*W;
            if(isDepth){
                if(shadowDepth[pos] > p.z){
                    shadowDepth[pos] = p.z;
                }
                continue;
            }


            // p.z = pz;
 

            vec2 uv1(bc.x*triangleUV[0].x + bc.y*triangleUV[1].x+bc.z*triangleUV[2].x,bc.x*triangleUV[0].y + bc.y*triangleUV[1].y+bc.z*triangleUV[2].y);
            //对UV进行差值
            pointUV = vec2(
                bc.x*triangleUV[0].x/zval[0] + bc.y*triangleUV[1].x/zval[1] + bc.z*triangleUV[2].x /zval[2],
                bc.x*triangleUV[0].y/zval[0] + bc.y*triangleUV[1].y/zval[1] + bc.z*triangleUV[2].y/zval[2]
            );
            pointUV *= pz;
            // 先用相机空间中的深度值
            if(zbuffer[pos] > pz){
                zbuffer[pos] = pz;
                vec4 color = fragment();
                drawPixel(p.x,p.y,color);
            }
        }
    }
}

// 计算变换
void vertex(){
    vector<vec3> triangle = scene[modelIdx].getFace(triangleIdx);
    vector<vec3> normals = scene[modelIdx].getVn(triangleIdx);
    vector<vec2> uvs = scene[modelIdx].getVt(triangleIdx);
    for(int i=0;i<3;i++){
        vec3 pos = triangle[i];
        vec3 n = normals[i];
        triangleUV[i] = vec3(uvs[i],0);
        triangleWorldPos[i] = vec3(mvMatrix * vec4(pos,1.0f));
        triangleWorldNorm[i] = vMatrix * vec4(vec3(mvMatrixIT * vec4(n,0.0f)),0.0f);
        glPosition[i] = mvpMatrix* vec4(pos,1.0f);;
    }
}

float max_elevation_angle(vec2 p, vec2 dir) {
    float maxangle = 0;
    for (float t=0.; t<1000.; t+=1.) {
        vec2 cur = p + dir*t;
        if (cur.x>=W || cur.y>=H || cur.x<0 || cur.y<0) return maxangle;

        float distance = length((p-cur));
        if (distance < 1.f) continue;
        // 如果遮挡的点在前面，不计算
        float elevation = zbuffer[int(p.x)+int(p.y)*W] - zbuffer[int(cur.x)+int(cur.y)*W];
        maxangle = std::max(maxangle, atanf(elevation/distance));
    }
    return maxangle;
}

void ao(){
    for(int i =0;i<W;i++){
        for(int j=0;j<H;j++){
            if (zbuffer[i+j*W] < -1e5) continue;
            float total = 0;
            for (float a=0; a<PI*2-1e-4; a += PI/4) {
                total += PI/2 - max_elevation_angle(vec2(i, j), vec2(cos(a), sin(a)));
            }
            total /= (PI/2)*8;
            total = pow(total, 100.f);
            cout<<i<<" "<<j<<" "<<total<<endl;
            // drawPixel(i,j,SDL_MapRGBA(screen->format,total*255,total*255,total*255,255));
            drawPixel(i,j,vec4(total,total,total,1.0f));
        }
    }
}

void calcDepth(){
    // 计算场景深度
    // 先计算深度贴图
    isDepth = true;
    for(int i =0;i<W;i++){
        for(int j=0;j<H;j++){
            shadowDepth[i + j * W ] = 1.0f; // 最远
        }
    }
    for(modelIdx=0; modelIdx<scene.size(); modelIdx++){
        vMatrix = lookAt(directLight * directLightLen, vec3(0,0,0),vec3(0,1,0));
        mvMatrix = vMatrix * scene[modelIdx].mMatrix;
        mvpMatrix = projMatrix * mvMatrix;
        shadowVP = projMatrix * vMatrix;
        for(triangleIdx=0;triangleIdx<scene[modelIdx].nfaces();triangleIdx++){
            vertex();
            drawTriangle();
        }
    }
}

void showDepth(){
    for(int i =0;i<W;i++){
        for(int j=0;j<H;j++){
            vec4 color = (1.0f - shadowDepth[i + j * W ]) * vec4(1.0f,1.0f,1.0f,1.0f);
            float d = (1.0f - shadowDepth[i + j * W ]);
            // d = ;
            drawPixel(i,j,vec4(d,d,d,1.0));
        }
    }
}

// 在渲染场景之前，准备好场景数据
void drawScreen(){
    for(int i =0;i<W;i++){
        for(int j=0;j<H;j++){
            zbuffer[i + j * W ] = FLT_MAX;
            drawPixel(i,j,vec4(1.0f));
        }
    }
    
    calcDepth();


    // 把平行光的方向切换到相机空间
    isDepth = false;
    vMatrix = lookAt(cameraWorldPos,vec3(0,0,0),vec3(0,1,0));
    vMatrixI = inverse(vMatrix);
    shadowVP = projMatrix * lookAt(directLight * directLightLen, vec3(0,0,0),vec3(0,1,0));
    viewDirectLight = vMatrix * vec4(directLight,0.0f);
    viewDirectLight = normalize(viewDirectLight);
    // 依次渲染场景中的每个物品的每个面
    for(modelIdx=0; modelIdx<scene.size(); modelIdx++){
        mvMatrix = lookAt(cameraWorldPos,vec3(0,0,0),vec3(0,1,0)) * scene[modelIdx].mMatrix;
        mvMatrixIT = vMatrix * transpose(inverse(scene[modelIdx].mMatrix));
        mvpMatrix = projMatrix * mvMatrix;
        for(triangleIdx=0;triangleIdx<scene[modelIdx].nfaces();triangleIdx++){
            vertex();
            drawTriangle();
        }
    }
    
    // showDepth();

    // ao(); // 环境光照，非常慢

    // screen->flip_vertically();
}


int main(int argc, char *argv[])
{
    init();
    bool  running = true ;
    drawScreen();
    screen->write_tga_file("./output.tga");
    system("Start output.tga");
    system("pause");
    return 0;
}
