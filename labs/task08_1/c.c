// Задание 8.1В: Сравнение 3 алгоритмов дизеринга: Bayer, Floyd-Steinberg, Jarvis-Judice-Ninke
#include "raylib.h"
#include <math.h>

#define W 800
#define H 600
#define IW 400
#define IH 300

static const int bayer4[4][4]={{0,8,2,10},{12,4,14,6},{3,11,1,9},{15,7,13,5}};

static void gen_image(Image *img) {
    *img = GenImageColor(IW, IH, BLACK);
    for (int y = 0; y < IH; y++)
        for (int x = 0; x < IW; x++) {
            float fx = (float)x/IW, fy = (float)y/IH;
            unsigned char v = (unsigned char)((0.5f + 0.3f*sinf(fx*12.56f) + 0.2f*cosf(fy*9.42f)) * 255);
            ImageDrawPixel(img, x, y, (Color){v,v,v,255});
        }
}

static Image dither_bayer(Image *src) {
    Image d = ImageCopy(*src);
    for (int y = 0; y < IH; y++)
        for (int x = 0; x < IW; x++) {
            float g = GetImageColor(*src, x, y).r / 255.0f;
            float t = (bayer4[y%4][x%4] + 0.5f) / 16.0f;
            unsigned char v = g > t ? 255 : 0;
            ImageDrawPixel(&d, x, y, (Color){v,v,v,255});
        }
    return d;
}

static void diffuse(float *buf, int x, int y, float err, int dx, int dy, float w) {
    int nx = x+dx, ny = y+dy;
    if (nx >= 0 && nx < IW && ny < IH)
        buf[ny*IW+nx] += err * w;
}

static Image dither_fs(Image *src) {
    float *buf = MemAlloc(IW*IH*sizeof(float));
    for (int y=0;y<IH;y++) for(int x=0;x<IW;x++) buf[y*IW+x]=GetImageColor(*src,x,y).r/255.0f;
    for (int y=0;y<IH;y++) for(int x=0;x<IW;x++) {
        float old=buf[y*IW+x], nw=old>0.5f?1:0, err=old-nw;
        buf[y*IW+x]=nw;
        diffuse(buf,x,y,err,1,0,7.0f/16);
        diffuse(buf,x,y,err,-1,1,3.0f/16);
        diffuse(buf,x,y,err,0,1,5.0f/16);
        diffuse(buf,x,y,err,1,1,1.0f/16);
    }
    Image d=GenImageColor(IW,IH,BLACK);
    for(int y=0;y<IH;y++) for(int x=0;x<IW;x++){
        unsigned char v=buf[y*IW+x]>0.5f?255:0;
        ImageDrawPixel(&d,x,y,(Color){v,v,v,255});
    }
    MemFree(buf);
    return d;
}

static Image dither_jjn(Image *src) {
    float *buf = MemAlloc(IW*IH*sizeof(float));
    for(int y=0;y<IH;y++) for(int x=0;x<IW;x++) buf[y*IW+x]=GetImageColor(*src,x,y).r/255.0f;
    static const int jdx[]={1,2,-2,-1,0,1,2,-2,-1,0,1,2};
    static const int jdy[]={0,0,1,1,1,1,1,2,2,2,2,2};
    static const float jw[]={7,5,3,5,7,5,3,1,3,5,3,1};
    for(int y=0;y<IH;y++) for(int x=0;x<IW;x++){
        float old=buf[y*IW+x], nw=old>0.5f?1:0, err=old-nw;
        buf[y*IW+x]=nw;
        for(int k=0;k<12;k++) diffuse(buf,x,y,err,jdx[k],jdy[k],jw[k]/48.0f);
    }
    Image d=GenImageColor(IW,IH,BLACK);
    for(int y=0;y<IH;y++) for(int x=0;x<IW;x++){
        unsigned char v=buf[y*IW+x]>0.5f?255:0;
        ImageDrawPixel(&d,x,y,(Color){v,v,v,255});
    }
    MemFree(buf);
    return d;
}

int main(void) {
    InitWindow(W,H,"Lab 08C - Dithering Comparison");
    SetTargetFPS(60);
    Image orig; gen_image(&orig);
    Image db=dither_bayer(&orig), dfs=dither_fs(&orig), djjn=dither_jjn(&orig);
    Texture2D t0=LoadTextureFromImage(orig), t1=LoadTextureFromImage(db);
    Texture2D t2=LoadTextureFromImage(dfs), t3=LoadTextureFromImage(djjn);
    UnloadImage(orig);UnloadImage(db);UnloadImage(dfs);UnloadImage(djjn);
    while(!WindowShouldClose()){
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexture(t0,0,0,WHITE);
        DrawTexture(t1,IW,0,WHITE);
        DrawTexture(t2,0,IH,WHITE);
        DrawTexture(t3,IW,IH,WHITE);
        DrawText("Original",5,5,16,GREEN);
        DrawText("Bayer 4x4",IW+5,5,16,GREEN);
        DrawText("Floyd-Steinberg",5,IH+5,16,GREEN);
        DrawText("Jarvis-Judice-Ninke",IW+5,IH+5,16,GREEN);
        EndDrawing();
    }
    UnloadTexture(t0);UnloadTexture(t1);UnloadTexture(t2);UnloadTexture(t3);
    CloseWindow();
    return 0;
}
