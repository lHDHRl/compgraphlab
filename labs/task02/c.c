// Задание 2В: Хаотичные прямоугольники, отсечение + заливка видимых частей
#include "raylib.h"
#include <math.h>

static float xmin=250, ymin=150, xmax=550, ymax=450;
enum { LEFT=1, RIGHT=2, BOTTOM=4, TOP=8 };

int outcode(float x, float y) {
    int c=0;
    if (x<xmin) c|=LEFT; else if (x>xmax) c|=RIGHT;
    if (y>ymax) c|=BOTTOM; else if (y<ymin) c|=TOP;
    return c;
}

int cs_clip(float *x0, float *y0, float *x1, float *y1) {
    int c0=outcode(*x0,*y0), c1=outcode(*x1,*y1);
    while (1) {
        if (!(c0|c1)) return 1;
        if (c0&c1) return 0;
        int c=c0?c0:c1; float x,y;
        if (c&TOP)         { x=*x0+(*x1-*x0)*(ymin-*y0)/(*y1-*y0); y=ymin; }
        else if (c&BOTTOM) { x=*x0+(*x1-*x0)*(ymax-*y0)/(*y1-*y0); y=ymax; }
        else if (c&RIGHT)  { y=*y0+(*y1-*y0)*(xmax-*x0)/(*x1-*x0); x=xmax; }
        else               { y=*y0+(*y1-*y0)*(xmin-*x0)/(*x1-*x0); x=xmin; }
        if (c==c0) { *x0=x; *y0=y; c0=outcode(x,y); }
        else       { *x1=x; *y1=y; c1=outcode(x,y); }
    }
}

typedef struct { float x,y; } V2;
typedef struct { V2 v[4]; Color col; } Rect;

int clip_poly(V2 *in, int n, V2 *out) {
    V2 tmp[16], *src=in, *dst=tmp;
    float edges[] = {xmin,ymin,xmax,ymax};
    int cn = n;
    for (int e = 0; e < 4; e++) {
        int nn = 0; dst = (e%2==0) ? tmp : out;
        if (e==2) { src=tmp; dst=out; } if (e==3) { src=out; dst=tmp; }
        // Sutherland-Hodgman per edge
        if (cn == 0) return 0;
        V2 *si = (e<2) ? ((e==0)?in:tmp) : ((e==2)?tmp:out);
        V2 *so = (e<2) ? ((e==0)?tmp:out) : ((e==2)?out:tmp);
        nn = 0;
        for (int i = 0; i < cn; i++) {
            V2 cur = si[i], prev = si[(i+cn-1)%cn];
            int ci, pi;
            switch(e) {
                case 0: ci=cur.x>=xmin; pi=prev.x>=xmin; break;
                case 1: ci=cur.x<=xmax; pi=prev.x<=xmax; break;
                case 2: ci=cur.y>=ymin; pi=prev.y>=ymin; break;
                default:ci=cur.y<=ymax; pi=prev.y<=ymax; break;
            }
            if (ci) {
                if (!pi) {
                    float t;
                    if (e==0||e==1) t=(edges[e]-prev.x)/(cur.x-prev.x);
                    else t=(edges[e]-prev.y)/(cur.y-prev.y);
                    so[nn++]=(V2){prev.x+t*(cur.x-prev.x), prev.y+t*(cur.y-prev.y)};
                }
                so[nn++]=cur;
            } else if (pi) {
                float t;
                if (e==0||e==1) t=(edges[e]-prev.x)/(cur.x-prev.x);
                else t=(edges[e]-prev.y)/(cur.y-prev.y);
                so[nn++]=(V2){prev.x+t*(cur.x-prev.x), prev.y+t*(cur.y-prev.y)};
            }
        }
        cn=nn; si=so;
        if (e==0) src=tmp; if (e==1) src=out; if (e==2) src=tmp;
    }
    V2 *final_src = (3%2==0) ? tmp : out;
    // result is in tmp after edge 3
    for (int i=0;i<cn;i++) out[i]=tmp[i];
    return cn;
}

int main(void) {
    InitWindow(800, 600, "Lab 2C - Хаотичные прямоугольники + заливка");
    SetTargetFPS(60);

    Rect rects[25];
    for (int i = 0; i < 25; i++) {
        float rad = 30 + (i*73%150);
        float ang = (i*137%360)*DEG2RAD;
        float ox = 300 + (i*53%200), oy = 200 + (i*41%200);
        float cs=cosf(ang), sn=sinf(ang);
        float hw=rad, hh=rad*0.6f;
        float cx[]={-hw,hw,hw,-hw}, cy[]={-hh,-hh,hh,hh};
        for (int j=0;j<4;j++) {
            rects[i].v[j].x = ox + cx[j]*cs - cy[j]*sn;
            rects[i].v[j].y = oy + cx[j]*sn + cy[j]*cs;
        }
        unsigned char r = 60+(i*37%180), g = 60+(i*59%180), b = 60+(i*97%180);
        rects[i].col = (Color){r,g,b,120};
    }

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        for (int i = 0; i < 25; i++) {
            for (int j=0;j<4;j++) {
                int k=(j+1)%4;
                DrawLine(rects[i].v[j].x,rects[i].v[j].y,rects[i].v[k].x,rects[i].v[k].y, DARKGRAY);
            }
        }

        for (int i = 0; i < 25; i++) {
            V2 clipped[16];
            int cn = clip_poly(rects[i].v, 4, clipped);
            if (cn >= 3) {
                for (int j = 1; j < cn-1; j++)
                    DrawTriangle(
                        (Vector2){clipped[0].x,clipped[0].y},
                        (Vector2){clipped[j].x,clipped[j].y},
                        (Vector2){clipped[j+1].x,clipped[j+1].y}, rects[i].col);
            }
            for (int j=0;j<4;j++) {
                int k=(j+1)%4;
                float a=rects[i].v[j].x, b=rects[i].v[j].y, c=rects[i].v[k].x, d=rects[i].v[k].y;
                if (cs_clip(&a,&b,&c,&d)) DrawLine(a,b,c,d, GREEN);
            }
        }

        DrawRectangleLines(xmin,ymin,xmax-xmin,ymax-ymin, YELLOW);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
