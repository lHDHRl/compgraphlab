// Задание 8.3Б: Сохранение с субдискретизацией 4:2:2
#include "../../raylib/src/raylib.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

static void rgb2ycrcb(Color c, unsigned char *y, unsigned char *cr, unsigned char *cb) {
    float Y = 0.299f*c.r + 0.587f*c.g + 0.114f*c.b;
    *y = (unsigned char)Y;
    *cr = (unsigned char)(128 + (c.r - Y)*0.713f);
    *cb = (unsigned char)(128 + (c.b - Y)*0.564f);
}

static Color ycrcb2rgb(unsigned char y, unsigned char cr, unsigned char cb) {
    float Y = y, Cr = cr - 128, Cb = cb - 128;
    int r = Y + Cr/0.713f, g = Y - 0.344136f*Cb/0.564f - 0.714136f*Cr/0.713f, b = Y + Cb/0.564f;
    return (Color){r<0?0:r>255?255:r, g<0?0:g>255?255:g, b<0?0:b>255?255:b, 255};
}

int main(void) {
    int W=400, H=300, n=W*H, cw=W/2;
    Image img = GenImageGradientLinear(W, H, 45, RED, BLUE);
    Color *px = LoadImageColors(img);

    unsigned char *Y = malloc(n), *Cr = malloc(n), *Cb = malloc(n);
    unsigned char *sCr = malloc(cw*H), *sCb = malloc(cw*H);
    for (int i = 0; i < n; i++) rgb2ycrcb(px[i], &Y[i], &Cr[i], &Cb[i]);
    for (int r = 0; r < H; r++)
        for (int c = 0; c < cw; c++) {
            sCr[r*cw+c] = (Cr[r*W+c*2] + Cr[r*W+c*2+1]) / 2;
            sCb[r*cw+c] = (Cb[r*W+c*2] + Cb[r*W+c*2+1]) / 2;
        }

    long fullsz = n*3, filesz = n + cw*H*2 + 8;
    Image rec = GenImageColor(W, H, BLACK);
    int loaded = 0;

    InitWindow(800, 600, "8.3B YCrCb 4:2:2");
    SetTargetFPS(60);
    Texture tOrig = LoadTextureFromImage(img), tRec = LoadTextureFromImage(rec);

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_S)) {
            FILE *f = fopen("output_422.ycrcb", "wb");
            fwrite(&W, 4, 1, f); fwrite(&H, 4, 1, f);
            fwrite(Y, 1, n, f); fwrite(sCr, 1, cw*H, f); fwrite(sCb, 1, cw*H, f);
            fclose(f);
        }
        if (IsKeyPressed(KEY_L)) {
            FILE *f = fopen("output_422.ycrcb", "rb");
            if (f) {
                int w2, h2; fread(&w2, 4, 1, f); fread(&h2, 4, 1, f);
                fread(Y, 1, n, f); fread(sCr, 1, cw*H, f); fread(sCb, 1, cw*H, f);
                fclose(f);
                for (int r = 0; r < H; r++)
                    for (int c = 0; c < W; c++)
                        ((Color*)rec.data)[r*W+c] = ycrcb2rgb(Y[r*W+c], sCr[r*cw+c/2], sCb[r*cw+c/2]);
                UnloadTexture(tRec); tRec = LoadTextureFromImage(rec);
                loaded = 1;
            }
        }
        BeginDrawing();
        ClearBackground(DARKGRAY);
        DrawTexture(tOrig, 0, 0, WHITE);
        if (loaded) DrawTexture(tRec, 400, 0, WHITE);
        DrawText("Original", 10, 310, 20, WHITE);
        DrawText("Reconstructed 4:2:2 (S/L)", 410, 310, 20, WHITE);
        DrawText(TextFormat("Full: %ld B  File: %ld B", fullsz, filesz), 10, 340, 20, YELLOW);
        EndDrawing();
    }
    free(Y); free(Cr); free(Cb); free(sCr); free(sCb);
    UnloadImageColors(px); UnloadTexture(tOrig); UnloadTexture(tRec);
    UnloadImage(img); UnloadImage(rec); CloseWindow();
}
