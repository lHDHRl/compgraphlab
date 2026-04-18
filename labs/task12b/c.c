// Задание 12Б.В: Z-буфер — текстурирование куба + туман
#include "raylib.h"
#include <math.h>
#include <stdlib.h>
#include <float.h>

#define W 800
#define H 600

typedef struct { float x,y,z; } V3;
typedef struct { float u,v; } V2;

static V3 v3(float x,float y,float z){return(V3){x,y,z};}
static V3 v3sub(V3 a,V3 b){return v3(a.x-b.x,a.y-b.y,a.z-b.z);}
static V3 v3scale(V3 a,float s){return v3(a.x*s,a.y*s,a.z*s);}
static float v3dot(V3 a,V3 b){return a.x*b.x+a.y*b.y+a.z*b.z;}
static V3 v3cross(V3 a,V3 b){return v3(a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x);}
static V3 v3norm(V3 a){float l=sqrtf(v3dot(a,a));return l>0?v3scale(a,1.0f/l):a;}

static V3 rotY(V3 p,float a){float c=cosf(a),s=sinf(a);return v3(p.x*c+p.z*s,p.y,-p.x*s+p.z*c);}
static V3 rotX(V3 p,float a){float c=cosf(a),s=sinf(a);return v3(p.x,p.y*c-p.z*s,p.y*s+p.z*c);}

static V3 project(V3 p){
    float d=4.0f;float f=d/(d-p.z);
    return v3(W/2+p.x*f*200,H/2-p.y*f*200,p.z);
}

static float zbuf[W*H];
static Color fogColor={40,40,60,255};

// Checkerboard texture
#define TEX_SZ 64
static Color texData[TEX_SZ*TEX_SZ];

static void genTexture(void){
    for(int y=0;y<TEX_SZ;y++)for(int x=0;x<TEX_SZ;x++){
        int check=((x/8)+(y/8))&1;
        texData[y*TEX_SZ+x]=check?(Color){220,200,180,255}:(Color){80,60,40,255};
    }
}

static Color sampleTex(float u,float v){
    u=u-floorf(u);v=v-floorf(v);
    int tx=(int)(u*(TEX_SZ-1));int ty=(int)(v*(TEX_SZ-1));
    return texData[ty*TEX_SZ+tx];
}

static Color mixColor(Color a,Color b,float t){
    if(t<0)t=0;if(t>1)t=1;
    return(Color){a.r+(b.r-a.r)*t,a.g+(b.g-a.g)*t,a.b+(b.b-a.b)*t,255};
}

static float fogDensity=0.3f;

static void drawTriTex(Image*img,V3 s0,V3 s1,V3 s2,V2 uv0,V2 uv1,V2 uv2,float light){
    int minx=(int)fminf(fminf(s0.x,s1.x),s2.x);
    int maxx=(int)fmaxf(fmaxf(s0.x,s1.x),s2.x);
    int miny=(int)fminf(fminf(s0.y,s1.y),s2.y);
    int maxy=(int)fmaxf(fmaxf(s0.y,s1.y),s2.y);
    if(minx<0)minx=0;if(miny<0)miny=0;if(maxx>=W)maxx=W-1;if(maxy>=H)maxy=H-1;
    float dx01=s1.x-s0.x,dy01=s1.y-s0.y,dx02=s2.x-s0.x,dy02=s2.y-s0.y;
    float det=dx01*dy02-dx02*dy01;if(fabsf(det)<1e-6f)return;
    float inv=1.0f/det;
    for(int y=miny;y<=maxy;y++)for(int x=minx;x<=maxx;x++){
        float px=x-s0.x,py=y-s0.y;
        float u=(px*dy02-dx02*py)*inv;
        float v=(dx01*py-px*dy01)*inv;
        if(u>=0&&v>=0&&u+v<=1.0f){
            float z=s0.z+u*(s1.z-s0.z)+v*(s2.z-s0.z);
            int idx=y*W+x;
            if(z<zbuf[idx]){
                zbuf[idx]=z;
                float tu=uv0.u+u*(uv1.u-uv0.u)+v*(uv2.u-uv0.u);
                float tv=uv0.v+u*(uv1.v-uv0.v)+v*(uv2.v-uv0.v);
                Color tc=sampleTex(tu,tv);
                tc.r*=light;tc.g*=light;tc.b*=light;
                // Linear fog based on z
                float fogF=fmaxf(0,fminf(1,(z+2.0f)*fogDensity));
                tc=mixColor(tc,fogColor,fogF);
                ImageDrawPixel(img,x,y,tc);
            }
        }
    }
}

int main(void){
    genTexture();

    // Cube vertices
    V3 cv[8]={v3(-1,-1,-1),v3(1,-1,-1),v3(1,1,-1),v3(-1,1,-1),
              v3(-1,-1,1),v3(1,-1,1),v3(1,1,1),v3(-1,1,1)};
    // 6 faces, each as 4 vertex indices + 4 UVs
    int fi[6][4]={{0,1,2,3},{5,4,7,6},{4,0,3,7},{1,5,6,2},{3,2,6,7},{4,5,1,0}};
    V2 fuv[4]={{0,0},{1,0},{1,1},{0,1}};

    V3 lightDir=v3norm(v3(1,1,-1));
    float ay=0,ax=0;

    InitWindow(W,H,"12Б.В Z-буфер: текстура + туман");
    SetTargetFPS(60);
    Image img=GenImageColor(W,H,fogColor);
    Texture tex=LoadTextureFromImage(img);

    while(!WindowShouldClose()){
        if(IsKeyDown(KEY_LEFT))ay-=0.02f;
        if(IsKeyDown(KEY_RIGHT))ay+=0.02f;
        if(IsKeyDown(KEY_UP))ax-=0.02f;
        if(IsKeyDown(KEY_DOWN))ax+=0.02f;
        if(IsKeyDown(KEY_EQUAL)||IsKeyDown(KEY_KP_ADD))fogDensity+=0.005f;
        if(IsKeyDown(KEY_MINUS)||IsKeyDown(KEY_KP_SUBTRACT))fogDensity-=0.005f;
        if(fogDensity<0)fogDensity=0;if(fogDensity>2)fogDensity=2;

        ImageClearBackground(&img,fogColor);
        for(int i=0;i<W*H;i++)zbuf[i]=FLT_MAX;

        for(int f=0;f<6;f++){
            // Two triangles per face: 0,1,2 and 0,2,3
            int tri[2][3]={{0,1,2},{0,2,3}};
            for(int t=0;t<2;t++){
                V3 wt[3],st[3];V2 uv[3];
                for(int k=0;k<3;k++){
                    V3 p=cv[fi[f][tri[t][k]]];
                    wt[k]=rotY(rotX(p,ax),ay);
                    st[k]=project(wt[k]);
                    uv[k]=fuv[tri[t][k]];
                }
                V3 n=v3norm(v3cross(v3sub(wt[1],wt[0]),v3sub(wt[2],wt[0])));
                float light=fmaxf(0.1f,v3dot(n,lightDir));
                drawTriTex(&img,st[0],st[1],st[2],uv[0],uv[1],uv[2],light);
            }
        }

        UpdateTexture(tex,img.data);
        BeginDrawing();
        DrawTexture(tex,0,0,WHITE);
        DrawText(TextFormat("Fog: %.2f (+/-)",fogDensity),10,30,20,WHITE);
        DrawFPS(10,10);
        EndDrawing();
    }
    UnloadTexture(tex);UnloadImage(img);CloseWindow();
}
