// Задание 7Б: Два объекта (сфера и тор), переключение Гуро/Фонг по TAB
#include "raylib.h"
#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define W 800
#define H 600
#define SS 16
#define TS 24

typedef struct { float x, y, z; } V3;
static V3 v3(float x, float y, float z) { return (V3){x,y,z}; }
static V3 v3sub(V3 a, V3 b) { return v3(a.x-b.x,a.y-b.y,a.z-b.z); }
static V3 v3add(V3 a, V3 b) { return v3(a.x+b.x,a.y+b.y,a.z+b.z); }
static V3 v3s(V3 a, float s) { return v3(a.x*s,a.y*s,a.z*s); }
static float v3dot(V3 a, V3 b) { return a.x*b.x+a.y*b.y+a.z*b.z; }
static V3 v3n(V3 a) { float l=sqrtf(v3dot(a,a)); return l>0?v3s(a,1/l):a; }
typedef struct { float x, y; } V2;

static float zbuf[W*H];

static V2 proj(V3 p) {
    float f=500, cz=5; float z=p.z+cz; if(z<0.1f)z=0.1f;
    return (V2){W/2.0f+p.x*f/z, H/2.0f-p.y*f/z};
}

static float shade(V3 pos, V3 norm, V3 light) {
    V3 L=v3n(v3sub(light,pos));
    float d=v3dot(norm,L); if(d<0)d=0;
    return 0.1f+0.9f*d;
}

static float ef(V2 a, V2 b, V2 c) { return (c.x-a.x)*(b.y-a.y)-(c.y-a.y)*(b.x-a.x); }

static void fill_tri(Image *img, V2 p0, V2 p1, V2 p2,
    float c0, float c1, float c2,
    V3 pos0, V3 pos1, V3 pos2,
    V3 n0, V3 n1, V3 n2,
    float z0, float z1, float z2,
    V3 light, int phong) {
    int x0=(int)fminf(fminf(p0.x,p1.x),p2.x), x1=(int)fmaxf(fmaxf(p0.x,p1.x),p2.x);
    int y0=(int)fminf(fminf(p0.y,p1.y),p2.y), y1=(int)fmaxf(fmaxf(p0.y,p1.y),p2.y);
    if(x0<0)x0=0;
    if(y0<0)y0=0;
    if(x1>=W)x1=W-1;
    if(y1>=H)y1=H-1;
    float area=ef(p0,p1,p2); if(fabsf(area)<0.001f)return;
    for(int y=y0;y<=y1;y++) for(int x=x0;x<=x1;x++) {
        V2 p={x+0.5f,y+0.5f};
        float w0=ef(p1,p2,p)/area, w1=ef(p2,p0,p)/area, w2=ef(p0,p1,p)/area;
        if(w0>=0&&w1>=0&&w2>=0) {
            float z=w0*z0+w1*z1+w2*z2;
            int idx=y*W+x;
            if(z<zbuf[idx]) {
                zbuf[idx]=z;
                float c;
                if(phong) {
                    V3 n=v3n(v3add(v3add(v3s(n0,w0),v3s(n1,w1)),v3s(n2,w2)));
                    V3 pos=v3add(v3add(v3s(pos0,w0),v3s(pos1,w1)),v3s(pos2,w2));
                    c=shade(pos,n,light);
                } else {
                    c=w0*c0+w1*c1+w2*c2;
                }
                unsigned char v=(unsigned char)(c*255);
                ImageDrawPixel(img,x,y,(Color){v,v,v,255});
            }
        }
    }
}

static V3 sphere_p(int i, int j, float r, V3 cen) {
    float t=(float)i/SS*M_PI, p=(float)j/SS*2*M_PI;
    return v3add(cen, v3(r*sinf(t)*cosf(p), r*cosf(t), r*sinf(t)*sinf(p)));
}

static V3 torus_p(int i, int j, float R, float r, V3 cen) {
    float t=(float)i/TS*2*M_PI, p=(float)j/TS*2*M_PI;
    return v3add(cen, v3((R+r*cosf(t))*cosf(p), r*sinf(t), (R+r*cosf(t))*sinf(p)));
}

static V3 torus_n(int i, int j) {
    float t=(float)i/TS*2*M_PI, p=(float)j/TS*2*M_PI;
    return v3n(v3(cosf(t)*cosf(p), sinf(t), cosf(t)*sinf(p)));
}

static void mesh_quad(Image *img, V3 v00,V3 v10,V3 v11,V3 v01, V3 n00,V3 n10,V3 n11,V3 n01, V3 light, int phong) {
    V2 s00=proj(v00),s10=proj(v10),s11=proj(v11),s01=proj(v01);
    float c00=shade(v00,n00,light),c10=shade(v10,n10,light),c11=shade(v11,n11,light),c01=shade(v01,n01,light);
    fill_tri(img,s00,s10,s11,c00,c10,c11,v00,v10,v11,n00,n10,n11,v00.z,v10.z,v11.z,light,phong);
    fill_tri(img,s00,s11,s01,c00,c11,c01,v00,v11,v01,n00,n11,n01,v00.z,v11.z,v01.z,light,phong);
}

int main(void) {
    InitWindow(W,H,"Lab 07B - Gouraud / Phong");
    SetTargetFPS(60);
    int phong=0; float angle=0;
    while(!WindowShouldClose()) {
        if(IsKeyPressed(KEY_TAB)) phong=!phong;
        angle+=0.01f;
        V3 light=v3(3*cosf(angle),2,3*sinf(angle));
        Image img=GenImageColor(W,H,BLACK);
        for(int i=0;i<W*H;i++) zbuf[i]=1e9f;
        V3 sc=v3(-1.2f,0,0);
        for(int i=0;i<SS;i++) for(int j=0;j<SS;j++) {
            V3 a=sphere_p(i,j,1,sc),b=sphere_p(i+1,j,1,sc),c=sphere_p(i+1,j+1,1,sc),d=sphere_p(i,j+1,1,sc);
            mesh_quad(&img,a,b,c,d,v3n(v3sub(a,sc)),v3n(v3sub(b,sc)),v3n(v3sub(c,sc)),v3n(v3sub(d,sc)),light,phong);
        }
        V3 tc=v3(1.2f,0,0);
        for(int i=0;i<TS;i++) for(int j=0;j<TS;j++) {
            V3 a=torus_p(i,j,0.7f,0.3f,tc),b=torus_p(i+1,j,0.7f,0.3f,tc),c=torus_p(i+1,j+1,0.7f,0.3f,tc),d=torus_p(i,j+1,0.7f,0.3f,tc);
            mesh_quad(&img,a,b,c,d,torus_n(i,j),torus_n(i+1,j),torus_n(i+1,j+1),torus_n(i,j+1),light,phong);
        }
        Texture2D tex=LoadTextureFromImage(img);
        BeginDrawing();
        DrawTexture(tex,0,0,WHITE);
        DrawText(phong?"Phong (per-pixel)":"Gouraud (per-vertex)",10,10,20,GREEN);
        DrawText("TAB to switch",10,35,16,GRAY);
        EndDrawing();
        UnloadTexture(tex); UnloadImage(img);
    }
    CloseWindow();
    return 0;
}
