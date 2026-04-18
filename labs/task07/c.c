// Задание 7В: Сцена (сфера, тор, плоскость), два источника, specular Фонг, тени на плоскость
#include "raylib.h"
#include <math.h>
#include <stddef.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define W 800
#define H 600
#define SS 20
#define TS 24
#define PG 10

typedef struct { float x,y,z; } V3;
typedef struct { float x,y; } V2;
static V3 v3(float x,float y,float z){return(V3){x,y,z};}
static V3 v3sub(V3 a,V3 b){return v3(a.x-b.x,a.y-b.y,a.z-b.z);}
static V3 v3add(V3 a,V3 b){return v3(a.x+b.x,a.y+b.y,a.z+b.z);}
static V3 v3s(V3 a,float s){return v3(a.x*s,a.y*s,a.z*s);}
static float v3dot(V3 a,V3 b){return a.x*b.x+a.y*b.y+a.z*b.z;}
static V3 v3n(V3 a){float l=sqrtf(v3dot(a,a));return l>0?v3s(a,1/l):a;}
static V3 v3reflect(V3 I,V3 N){return v3sub(I,v3s(N,2*v3dot(I,N)));}

typedef struct { V3 pos; V3 col; } Light;
static float zbuf[W*H];

static V2 proj(V3 p){float f=400,cz=6;float z=p.z+cz;if(z<0.1f)z=0.1f;return(V2){W/2.0f+p.x*f/z,H/2.0f-p.y*f/z};}

static void shade_phong(V3 pos, V3 N, V3 eye, Light *lights, int nl, float *r, float *g, float *b) {
    *r=0.05f; *g=0.05f; *b=0.05f;
    for(int i=0;i<nl;i++){
        V3 L=v3n(v3sub(lights[i].pos,pos));
        float diff=v3dot(N,L); if(diff<0)diff=0;
        V3 V=v3n(v3sub(eye,pos));
        V3 R=v3reflect(v3s(L,-1),N);
        float spec=v3dot(V,R); if(spec<0)spec=0; spec=powf(spec,32);
        *r+=lights[i].col.x*(diff*0.7f+spec*0.5f);
        *g+=lights[i].col.y*(diff*0.7f+spec*0.5f);
        *b+=lights[i].col.z*(diff*0.7f+spec*0.5f);
    }
    if(*r>1)*r=1;
    if(*g>1)*g=1;
    if(*b>1)*b=1;
}

static float ef(V2 a,V2 b,V2 c){return(c.x-a.x)*(b.y-a.y)-(c.y-a.y)*(b.x-a.x);}

static void fill_tri(Image *img, V2 p0,V2 p1,V2 p2, V3 pos0,V3 pos1,V3 pos2, V3 n0,V3 n1,V3 n2, V3 eye, Light *lights, int nl, Color tint) {
    int x0=(int)fminf(fminf(p0.x,p1.x),p2.x), x1=(int)fmaxf(fmaxf(p0.x,p1.x),p2.x);
    int y0=(int)fminf(fminf(p0.y,p1.y),p2.y), y1=(int)fmaxf(fmaxf(p0.y,p1.y),p2.y);
    if(x0<0)x0=0;
    if(y0<0)y0=0;
    if(x1>=W)x1=W-1;
    if(y1>=H)y1=H-1;
    float area=ef(p0,p1,p2);if(fabsf(area)<0.001f)return;
    for(int y=y0;y<=y1;y++) for(int x=x0;x<=x1;x++){
        V2 p={x+0.5f,y+0.5f};
        float w0=ef(p1,p2,p)/area,w1=ef(p2,p0,p)/area,w2=ef(p0,p1,p)/area;
        if(w0>=0&&w1>=0&&w2>=0){
            float z=w0*pos0.z+w1*pos1.z+w2*pos2.z;
            int idx=y*W+x;
            if(z<zbuf[idx]){
                zbuf[idx]=z;
                if(tint.a==1) {
                    ImageDrawPixel(img,x,y,(Color){tint.r,tint.g,tint.b,255});
                } else {
                    V3 N=v3n(v3add(v3add(v3s(n0,w0),v3s(n1,w1)),v3s(n2,w2)));
                    V3 pos=v3add(v3add(v3s(pos0,w0),v3s(pos1,w1)),v3s(pos2,w2));
                    float r,g,b;
                    shade_phong(pos,N,eye,lights,nl,&r,&g,&b);
                    ImageDrawPixel(img,x,y,(Color){(unsigned char)(r*tint.r),(unsigned char)(g*tint.g),(unsigned char)(b*tint.b),255});
                }
            }
        }
    }
}

static void quad(Image *img, V3 a,V3 b,V3 c,V3 d, V3 na,V3 nb,V3 nc,V3 nd, V3 eye, Light *lights, int nl, Color tint) {
    V2 sa=proj(a),sb=proj(b),sc=proj(c),sd=proj(d);
    fill_tri(img,sa,sb,sc,a,b,c,na,nb,nc,eye,lights,nl,tint);
    fill_tri(img,sa,sc,sd,a,c,d,na,nc,nd,eye,lights,nl,tint);
}

static V3 sphere_p(int i,int j,float r,V3 cen){
    float t=(float)i/SS*M_PI,p=(float)j/SS*2*M_PI;
    return v3add(cen,v3(r*sinf(t)*cosf(p),r*cosf(t),r*sinf(t)*sinf(p)));
}
static V3 torus_p(int i,int j,float R,float r,V3 cen){
    float t=(float)i/TS*2*M_PI,p=(float)j/TS*2*M_PI;
    return v3add(cen,v3((R+r*cosf(t))*cosf(p),r*sinf(t),(R+r*cosf(t))*sinf(p)));
}
static V3 torus_n(int i,int j){
    float t=(float)i/TS*2*M_PI,p=(float)j/TS*2*M_PI;
    return v3n(v3(cosf(t)*cosf(p),sinf(t),cosf(t)*sinf(p)));
}

static V3 shadow_proj(V3 p, V3 light, float py) {
    float t=(py-light.y)/(p.y-light.y);
    return v3(light.x+t*(p.x-light.x), py+0.001f, light.z+t*(p.z-light.z));
}

static void draw_sphere(Image *img, V3 cen, float r, V3 eye, Light *lights, int nl, Color tint) {
    for(int i=0;i<SS;i++) for(int j=0;j<SS;j++){
        V3 a=sphere_p(i,j,r,cen),b=sphere_p(i+1,j,r,cen),c=sphere_p(i+1,j+1,r,cen),d=sphere_p(i,j+1,r,cen);
        quad(img,a,b,c,d,v3n(v3sub(a,cen)),v3n(v3sub(b,cen)),v3n(v3sub(c,cen)),v3n(v3sub(d,cen)),eye,lights,nl,tint);
    }
}

static void draw_torus(Image *img, V3 cen, float R, float r, V3 eye, Light *lights, int nl, Color tint) {
    for(int i=0;i<TS;i++) for(int j=0;j<TS;j++){
        V3 a=torus_p(i,j,R,r,cen),b=torus_p(i+1,j,R,r,cen),c=torus_p(i+1,j+1,R,r,cen),d=torus_p(i,j+1,R,r,cen);
        quad(img,a,b,c,d,torus_n(i,j),torus_n(i+1,j),torus_n(i+1,j+1),torus_n(i,j+1),eye,lights,nl,tint);
    }
}

static void draw_shadow_sphere(Image *img, V3 cen, float r, V3 light, float py) {
    V3 up=v3(0,1,0);
    Color dark={30,30,30,1};
    for(int i=0;i<SS;i++) for(int j=0;j<SS;j++){
        V3 a=shadow_proj(sphere_p(i,j,r,cen),light,py);
        V3 b=shadow_proj(sphere_p(i+1,j,r,cen),light,py);
        V3 c=shadow_proj(sphere_p(i+1,j+1,r,cen),light,py);
        V3 d=shadow_proj(sphere_p(i,j+1,r,cen),light,py);
        quad(img,a,b,c,d,up,up,up,up,v3(0,0,-6),NULL,0,dark);
    }
}

static void draw_shadow_torus(Image *img, V3 cen, float R, float r, V3 light, float py) {
    V3 up=v3(0,1,0);
    Color dark={30,30,30,1};
    for(int i=0;i<TS;i++) for(int j=0;j<TS;j++){
        V3 a=shadow_proj(torus_p(i,j,R,r,cen),light,py);
        V3 b=shadow_proj(torus_p(i+1,j,R,r,cen),light,py);
        V3 c=shadow_proj(torus_p(i+1,j+1,R,r,cen),light,py);
        V3 d=shadow_proj(torus_p(i,j+1,R,r,cen),light,py);
        quad(img,a,b,c,d,up,up,up,up,v3(0,0,-6),NULL,0,dark);
    }
}

int main(void) {
    InitWindow(W,H,"Lab 07C - Phong Specular + Shadows");
    SetTargetFPS(60);
    float angle=0;
    while(!WindowShouldClose()){
        angle+=0.01f;
        Light lights[2]={{v3(3*cosf(angle),3,2*sinf(angle)),v3(1,0.6f,0.3f)},{v3(-2,2,-1),v3(0.3f,0.5f,1)}};
        V3 eye=v3(0,0,-6);
        Image img=GenImageColor(W,H,(Color){20,20,30,255});
        for(int i=0;i<W*H;i++) zbuf[i]=1e9f;
        float py=-1.5f;
        V3 up=v3(0,1,0);
        // ground plane
        for(int i=-PG;i<PG;i++) for(int j=-PG;j<PG;j++){
            V3 a=v3(i*0.5f,py,j*0.5f),b=v3((i+1)*0.5f,py,j*0.5f),c=v3((i+1)*0.5f,py,(j+1)*0.5f),d=v3(i*0.5f,py,(j+1)*0.5f);
            Color col=((i+j+200)%2==0)?(Color){180,180,180,255}:(Color){120,120,120,255};
            quad(&img,a,b,c,d,up,up,up,up,eye,lights,2,col);
        }
        V3 sc=v3(-1,0,0), tc=v3(1.2f,-0.5f,0);
        draw_shadow_sphere(&img,sc,0.8f,lights[0].pos,py);
        draw_shadow_torus(&img,tc,0.6f,0.25f,lights[0].pos,py);
        draw_sphere(&img,sc,0.8f,eye,lights,2,(Color){255,200,200,255});
        draw_torus(&img,tc,0.6f,0.25f,eye,lights,2,(Color){200,200,255,255});
        Texture2D tex=LoadTextureFromImage(img);
        BeginDrawing();
        DrawTexture(tex,0,0,WHITE);
        DrawText("Phong specular, 2 lights, planar shadows",10,10,20,GREEN);
        EndDrawing();
        UnloadTexture(tex);UnloadImage(img);
    }
    CloseWindow();
    return 0;
}
