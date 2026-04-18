// Задание 12Б.А: Z-буфер — куб и сфера, плоская закраска, Lambert shading
#include "raylib.h"
#include <math.h>
#include <stdlib.h>
#include <float.h>

#define W 800
#define H 600

typedef struct { float x,y,z; } V3;
typedef struct { float u,v; } V2;

static V3 v3(float x,float y,float z){return(V3){x,y,z};}
static V3 v3add(V3 a,V3 b){return v3(a.x+b.x,a.y+b.y,a.z+b.z);}
static V3 v3sub(V3 a,V3 b){return v3(a.x-b.x,a.y-b.y,a.z-b.z);}
static V3 v3scale(V3 a,float s){return v3(a.x*s,a.y*s,a.z*s);}
static float v3dot(V3 a,V3 b){return a.x*b.x+a.y*b.y+a.z*b.z;}
static V3 v3cross(V3 a,V3 b){return v3(a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x);}
static V3 v3norm(V3 a){float l=sqrtf(v3dot(a,a));return l>0?v3scale(a,1.0f/l):a;}

static V3 rotY(V3 p,float a){float c=cosf(a),s=sinf(a);return v3(p.x*c+p.z*s,p.y,-p.x*s+p.z*c);}

static V3 project(V3 p){
    float d=4.0f;float f=d/(d-p.z);
    return v3(W/2+p.x*f*200,H/2-p.y*f*200,p.z);
}

static float zbuf[W*H];

static void drawTri(Image*img,V3 s0,V3 s1,V3 s2,Color col){
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
            if(z<zbuf[idx]){zbuf[idx]=z;ImageDrawPixel(img,x,y,col);}
        }
    }
}

int main(void){
    // Cube vertices
    V3 cv[8]={v3(-1,-1,-1),v3(1,-1,-1),v3(1,1,-1),v3(-1,1,-1),
              v3(-1,-1,1),v3(1,-1,1),v3(1,1,1),v3(-1,1,1)};
    int ci[36]={0,1,2,0,2,3, 5,4,7,5,7,6, 4,0,3,4,3,7,
                1,5,6,1,6,2, 3,2,6,3,6,7, 4,5,1,4,1,0};
    Color cfc[6]={RED,GREEN,BLUE,YELLOW,MAGENTA,ORANGE};

    // Sphere
    #define SN 12
    #define SM 12
    V3 sv[(SN+1)*(SM+1)];
    for(int i=0;i<=SN;i++)for(int j=0;j<=SM;j++){
        float th=PI*i/SN,ph=2*PI*j/SM;
        sv[i*(SM+1)+j]=v3(sinf(th)*cosf(ph),cosf(th),sinf(th)*sinf(ph));
    }

    V3 lightDir=v3norm(v3(1,1,-1));
    float angle=0;

    InitWindow(W,H,"12Б.А Z-буфер: куб и сфера");
    SetTargetFPS(60);
    Image img=GenImageColor(W,H,BLACK);
    Texture tex=LoadTextureFromImage(img);

    while(!WindowShouldClose()){
        if(IsKeyDown(KEY_LEFT))angle-=0.02f;
        if(IsKeyDown(KEY_RIGHT))angle+=0.02f;

        // Clear
        ImageClearBackground(&img,BLACK);
        for(int i=0;i<W*H;i++)zbuf[i]=FLT_MAX;

        // Draw cube (offset left)
        for(int f=0;f<12;f++){
            V3 t[3];
            for(int k=0;k<3;k++){
                V3 p=v3scale(cv[ci[f*3+k]],0.6f);
                p.x-=1.2f;
                t[k]=project(rotY(p,angle));
            }
            V3 wt[3];for(int k=0;k<3;k++){V3 p=v3scale(cv[ci[f*3+k]],0.6f);p.x-=1.2f;wt[k]=rotY(p,angle);}
            V3 n=v3norm(v3cross(v3sub(wt[1],wt[0]),v3sub(wt[2],wt[0])));
            float d=fmaxf(0,v3dot(n,lightDir));
            Color c=cfc[f/2];
            c.r*=d;c.g*=d;c.b*=d;
            drawTri(&img,t[0],t[1],t[2],c);
        }

        // Draw sphere (offset right)
        for(int i=0;i<SN;i++)for(int j=0;j<SM;j++){
            V3 q[4]={sv[i*(SM+1)+j],sv[i*(SM+1)+j+1],sv[(i+1)*(SM+1)+j+1],sv[(i+1)*(SM+1)+j]};
            for(int tri=0;tri<2;tri++){
                int idx[3]={0,tri+1,tri+2};
                V3 wt[3],t[3];
                for(int k=0;k<3;k++){
                    V3 p=q[idx[k]];p.x+=1.2f;
                    wt[k]=rotY(p,angle);
                    t[k]=project(wt[k]);
                }
                V3 n=v3norm(v3cross(v3sub(wt[1],wt[0]),v3sub(wt[2],wt[0])));
                float d=fmaxf(0,v3dot(n,lightDir));
                unsigned char b=(unsigned char)(d*200+55);
                drawTri(&img,t[0],t[1],t[2],(Color){b,b/2,b/4,255});
            }
        }

        UpdateTexture(tex,img.data);
        BeginDrawing();
        DrawTexture(tex,0,0,WHITE);
        DrawFPS(10,10);
        EndDrawing();
    }
    UnloadTexture(tex);UnloadImage(img);CloseWindow();
}
