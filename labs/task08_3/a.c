// Задание 8.3А: Сохранение в YCrCb без сжатия (4:4:4)
#include "../../raylib/src/raylib.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct { unsigned char y, cr, cb; } YCrCb;

static YCrCb rgb2ycrcb(Color c) {
    float Y = 0.299f*c.r + 0.587f*c.g + 0.114f*c.b;
    return (YCrCb){(unsigned char)Y,
        (unsigned char)(128 + (c.r - Y)*0.713f),
        (unsigned char)(128 + (c.b - Y)*0.564f)};
}

static Color ycrcb2rgb(YCrCb v) {
    float Y = v.y, Cr = v.cr - 128, Cb = v.cb - 128;
    int r = Y + Cr/0.713f, g = Y - 0.344136f*Cb/0.564f - 0.714136f*Cr/0.713f, b = Y + Cb/0.564f;
    return (Color){r<0?0:r>255?255:r, g<0?0:g>255?255:g, b<0?0:b>255?255:b, 255};
}

int main(void) {
    int W=400, H=300, n=W*H;
    Image img = GenImageGradientLinear(W, H, 45, RED, BLUE);
    Color *px = LoadImageColors(img);
    YCrCb *buf = malloc(n * sizeof(YCrCb));
    for (int i = 0; i < n; i++) buf[i] = rgb2ycrcb(px[i]);

    Image rec = GenImageColor(W, H, BLACK);
    Color *rpx = LoadImageColors(rec);
    int loaded = 0;

    InitWindow(800, 600, "8.3A YCrCb 4:4:4");
    SetTargetFPS(60);
    Texture tOrig = LoadTextureFromImage(img);
    Texture tRec = LoadTextureFromImage(rec);

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_S)) {
            FILE *f = fopen("output.ycrcb", "wb");
            fwrite(&W, 4, 1, f); fwrite(&H, 4, 1, f);
            fwrite(buf, sizeof(YCrCb), n, f);
            fclose(f);
        }
        if (IsKeyPressed(KEY_L)) {
            FILE *f = fopen("output.ycrcb", "rb");
            if (f) {
                int w2, h2; fread(&w2, 4, 1, f); fread(&h2, 4, 1, f);
                fread(buf, sizeof(YCrCb), n, f); fclose(f);
                for (int i = 0; i < n; i++) rpx[i] = ycrcb2rgb(buf[i]);
                UnloadImage(rec); rec = GenImageColor(W, H, BLACK);
                Color *tmp = LoadImageColors(rec);
                for (int i = 0; i < n; i++) ((Color*)rec.data)[i] = rpx[i];
                UnloadTexture(tRec); tRec = LoadTextureFromImage(rec);
                loaded = 1;
            }
        }
        BeginDrawing();
        ClearBackground(DARKGRAY);
        DrawTexture(tOrig, 0, 0, WHITE);
        if (loaded) DrawTexture(tRec, 400, 0, WHITE);
        DrawText("Original", 10, 310, 20, WHITE);
        DrawText("Reconstructed (S=save, L=load)", 410, 310, 20, WHITE);
        EndDrawing();
    }
    free(buf); UnloadImageColors(px); UnloadImageColors(rpx);
    UnloadTexture(tOrig); UnloadTexture(tRec);
    UnloadImage(img); UnloadImage(rec);
    CloseWindow();
}
