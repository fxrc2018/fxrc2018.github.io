#ifndef __SGLM_H__
#define __SGLM_H__

#include <cmath>

template<int LEN>
class vec{
public:
    float& operator[](int idx){
        return data[idx];
    }
    const float& operator[](int idx) const{
        return data[idx];
    }
private:
    float data[LEN];
};

template<>
class vec<2>{
public:
    float x;
    float y;
    vec():x(0),y(0){}
    vec(float val):x(val),y(val){}
    vec(float x, float y, float z):x(x),y(y){}
    float& operator[](int idx){
        float *data = &x;
        return data[idx];
    }
    const float& operator[](int idx) const{
        const float *data = &x;
        return data[idx];
    }
};

template<>
class vec<3>{
public:
    float x;
    float y;
    float z;
    vec():x(0),y(0),z(0){}
    vec(float val):x(val),y(val),z(val){}
    vec(float x, float y, float z):x(x),y(y),z(z){}
    float& operator[](int idx){
        float *data = &x;
        return data[idx];
    }
    const float& operator[](int idx) const{
        const float *data = &x;
        return data[idx];
    }
};


template<>
class vec<4>{
    float x;
    float y;
    float z;
    float w;
public:
    vec():x(0),y(0),z(0),w(0){}
    vec(float val):x(val),y(val),z(val),w(val){}
    vec(float x, float y, float z, float w):x(x),y(y),z(z),w(w){}
    float& operator[](int idx){
        float *data = &x;
        return data[idx];
    }
    const float& operator[](int idx) const{
        const float *data = &x;
        return data[idx];
    }
};

template<int LEN>
float dot(const vec<LEN> &v1, const vec<LEN> &v2){
    float ret = 0.0f;
    for(int i=0;i<LEN;i++){
        ret += v1[i] * v2[i];
    }
    return ret;
}

template<int LEN>
vec<LEN> operator-(const vec<LEN> &v){
    vec<LEN> ret;
    for(int i=0;i<LEN;i++){
        ret[i] = -v[i];
    }
    return ret;
}

template<int LEN>
vec<LEN> operator-(const vec<LEN> &v1, const vec<LEN> &v2){
    vec<LEN> ret;
    for(int i=0;i<LEN;i++){
        ret[i] = v1[i] - v2[i];
    }
    return ret;
}

template<int LEN>
vec<LEN> operator+(const vec<LEN> &v1, const vec<LEN> &v2){
    vec<LEN> ret;
    for(int i=0;i<LEN;i++){
        ret[i] = v1[i] + v2[i];
    }
    return ret;
}

template<int LEN>
vec<LEN> operator*(float num, const vec<LEN> &v){
    vec<LEN> ret;
    for(int i=0;i<LEN;i++){
        ret[i] = v[i] * num;
    }
    return ret;
}

template<int LEN>
vec<LEN> operator*(const vec<LEN> &v, float num){
    vec<LEN> ret;
    for(int i=0;i<LEN;i++){
        ret[i] = v[i] * num;
    }
    return ret;
}

template<int LEN>
vec<LEN> operator/(const vec<LEN> &v, float num){
    vec<LEN> ret;
    for(int i=0;i<LEN;i++){
        ret[i] = v[i] / num;
    }
    return ret;
}

template<int LEN>
float length(const vec<LEN> &v){
    float ret = 0.0f;
    for(int i=0;i<LEN;i++){
        ret += v[i] * v[i];
    }
    return sqrt(ret);
}

template<int LEN>
vec<LEN> normalize(const vec<LEN> &v){
    return v / length(v);
}

float clamp(float x, float l, float r){
    if(x < l){
        x = l;
    }
    if(x > r){
        x = r;
    }
    return x;
}

typedef vec<2> vec2;
typedef vec<3> vec3;
typedef vec<4> vec4;

#endif