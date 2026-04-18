// Задание 12Б.Б: Z-буфер — пирамида, цилиндр, тор, закраска Гуро
#include "raylib.h"
#include <math.h>
#include <stdlib.h>
#include <float.h>

#define W 800
#define H 600

typedef struct { float x,y,z; } V3;

static V3 v3(float x,float y,float z){return(V3){x,y,z};}
static V3 v3add(V3 a,V3 b){return v3(a.x+b.x,a.y+b.y,a.z+b.z);}
static V3 v3sub(V3 a,V3 b){return v3(a.x-b.x,a.y-b.y,a.z-b.z);}
static V3 v3scale(V3 a,float s){return v3(a.x*s,a.y*s,a.z*s);}
static float v3dot(V3 a,V3 b){return a.x*b.x+a.y*b.y+a.z*b.z;}
static V3 v3cross(V3 a,V3 b){return v3(a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x);}
static V3 v3norm(V3 a){float l=sqrtf(v3dot(a,a));return l>0?v3scale(a,1.0f/l):a;}
static float v3len(V3 a){return sqrtf(v3dot(a,a));}

static V3 rotY(V3 p,float a){float c=cosf(a),s=sinf(a);return v3(p.x*c+p.z*s,p.y,-p.x*s+p.z*c);}
static V3 rotX(V3 p,float a){float c=cosf(a),s=sinf(a);return v3(p.x,p.y*c-p.z*s,p.y*s+p.z*c);}

static V3 project(V3 p){
    float d=5.0f;float f=d/(d-p.z);
    return v3(W/2+p.x*f*150,H/2-p.y*f*150,p.z);
}

static float zbuf[W*H];

static float lightVert(V3 pos,V3 norm,V3 lightPos){
    V3 toL=v3norm(v3sub(lightPos,pos));
    float diff=fmaxf(0,v3dot(norm,toL));
    return fminf(1.0f,0.15f+0.85f*diff);
}

static void drawTriGouraud(Image*img,V3 s0,V3 s1,V3 s2,float i0,float i1,float i2,Color base){
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
                float intensity=i0+u*(i1-i0)+v*(i2-i0);
                intensity=fmaxf(0,fminf(1,intensity));
                Color c={base.r*intensity,base.g*intensity,base.b*intensity,255};
                ImageDrawPixel(img,x,y,c);
            }
        }
    }
}

// Pyramid
#define PYR_VERTS 5
static V3 pyrV[PYR_VERTS];
static V3 pyrN[PYR_VERTS]; // averaged normals
static int pyrIdx[]={0,1,2, 0,2,3, 0,3,4, 0,4,1, 1,3,2, 1,4,3}; // 4 sides + 2 base tris

// Cylinder
#define CYL_SEG 20
static V3 cylV[(CYL_SEG+1)*2];
static V3 cylN[(CYL_SEG+1)*2];

// Torus
#define TR 16
#define TP 16
static V3 torV[(TR+1)*(TP+1)];
static V3 torN[(TR+1)*(TP+1)];

static void initGeom(void){
    // Pyramid: apex + 4 base
    pyrV[0]=v3(0,0.8f,0);
    for(int i=0;i<4;i++){float a=PI/2+i*PI/2;pyrV[1+i]=v3(0.5f*cosf(a),-0.5f,0.5f*sinf(a));}
    // Compute averaged normals
    for(int i=0;i<PYR_VERTS;i++)pyrN[i]=v3(0,0,0);
    for(int f=0;f<6;f++){
        V3*a=&pyrV[pyrIdx[f*3]],*b=&pyrV[pyrIdx[f*3+1]],*c=&pyrV[pyrIdx[f*3+2]];
        V3 n=v3cross(v3sub(*b,*a),v3sub(*c,*a));
        for(int k=0;k<3;k++)pyrN[pyrIdx[f*3+k]]=v3add(pyrN[pyrIdx[f*3+k]],n);
    }
    for(int i=0;i<PYR_VERTS;i++)pyrN[i]=v3norm(pyrN[i]);

    // Cylinder
    for(int i=0;i<=CYL_SEG;i++){
        float a=2*PI*i/CYL_SEG;float c=cosf(a),s=sinf(a);
        cylV[i]=v3(0.4f*c,0.6f,0.4f*s);
        cylV[CYL_SEG+1+i]=v3(0.4f*c,-0.6f,0.4f*s);
        cylN[i]=cylN[CYL_SEG+1+i]=v3norm(v3(c,0,s));
    }

    // Torus R=0.6 r=0.2
    for(int i=0;i<=TR;i++)for(int j=0;j<=TP;j++){
        float u=2*PI*i/TR,v=2*PI*j/TP;
        float cx=0.6f*cosf(u),cz=0.6f*sinf(u);
        torV[i*(TP+1)+j]=v3(cx+0.2f*cosf(v)*cosf(u),0.2f*sinf(v),cz+0.2f*cosf(v)*sinf(u));
        torN[i*(TP+1)+j]=v3norm(v3(cosf(v)*cosf(u),sinf(v),cosf(v)*sinf(u)));
    }
}

int main(void){
    initGeom();
    float ay=0,ax=0;
    V3 lightPos=v3(3,3,-3);

    InitWindow(W,H,"12Б.Б Z-буфер: Гуро");
    SetTargetFPS(60);
    Image img=GenImageColor(W,H,BLACK);
    Texture tex=LoadTextureFromImage(img);

    while(!WindowShouldClose()){
        if(IsKeyDown(KEY_LEFT))ay-=0.02f;
        if(IsKeyDown(KEY_RIGHT))ay+=0.02f;
        if(IsKeyDown(KEY_UP))ax-=0.02f;
        if(IsKeyDown(KEY_DOWN))ax+=0.02f;

        ImageClearBackground(&img,BLACK);
        for(int i=0;i<W*H;i++)zbuf[i]=FLT_MAX;

        // Helper: transform + project + light
        #define XFORM(p,n,off) { V3 wp=v3add(rotY(rotX(p,ax),ay),off); \
            V3 wn=v3norm(rotY(rotX(n,ax),ay)); \
            sp=project(wp); intensity=lightVert(wp,wn,lightPos); }

        // Pyramid (left)
        for(int f=0;f<6;f++){
            V3 sp; float intensity;
            V3 s[3]; float li[3];
            for(int k=0;k<3;k++){
                int vi=pyrIdx[f*3+k];
                XFORM(pyrV[vi],pyrN[vi],v3(-1.8f,0,0));
                s[k]=sp; li[k]=intensity;
            }
            drawTriGouraud(&img,s[0],s[1],s[2],li[0],li[1],li[2],(Color){200,100,50,255});
        }

        // Cylinder (center)
        for(int i=0;i<CYL_SEG;i++){
            int a=i,b=i+1,c=CYL_SEG+1+i,d=CYL_SEG+1+i+1;
            int tris[]={a,b,d, a,d,c};
            for(int t=0;t<2;t++){
                V3 s[3];float li[3];V3 sp;float intensity;
                for(int k=0;k<3;k++){
                    int vi=tris[t*3+k];
                    XFORM(cylV[vi],cylN[vi],v3(0,0,0));
                    s[k]=sp;li[k]=intensity;
                }
                drawTriGouraud(&img,s[0],s[1],s[2],li[0],li[1],li[2],(Color){50,180,50,255});
            }
        }

        // Torus (right)
        for(int i=0;i<TR;i++)for(int j=0;j<TP;j++){
            int idx[4]={i*(TP+1)+j,i*(TP+1)+j+1,(i+1)*(TP+1)+j+1,(i+1)*(TP+1)+j};
            int tris[]={0,1,2, 0,2,3};
            for(int t=0;t<2;t++){
                V3 s[3];float li[3];V3 sp;float intensity;
                for(int k=0;k<3;k++){
                    int vi=idx[tris[t*3+k]];
                    XFORM(torV[vi],torN[vi],v3(1.8f,0,0));
                    s[k]=sp;li[k]=intensity;
                }
                drawTriGouraud(&img,s[0],s[1],s[2],li[0],li[1],li[2],(Color){80,80,220,255});
            }
        }
        #undef XFORM

        UpdateTexture(tex,img.data);
        BeginDrawing();
        DrawTexture(tex,0,0,WHITE);
        DrawFPS(10,10);
        EndDrawing();
    }
    UnloadTexture(tex);UnloadImage(img);CloseWindow();
}
