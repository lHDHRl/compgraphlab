// Задание 8.3В: Субдискретизация 4:2:0 + RLE-сжатие канала яркости
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

static int rle_encode(unsigned char *in, int n, unsigned char *out) {
    int o = 0;
    for (int i = 0; i < n;) {
        unsigned char v = in[i]; int c = 1;
        while (i+c < n && in[i+c] == v && c < 255) c++;
        out[o++] = v; out[o++] = c; i += c;
    }
    return o;
}

static void rle_decode(unsigned char *in, int insz, unsigned char *out) {
    int o = 0;
    for (int i = 0; i < insz; i += 2)
        for (int j = 0; j < in[i+1]; j++) out[o++] = in[i];
}

int main(void) {
    int W=400, H=300, n=W*H, cw=W/2, ch=H/2, cn=cw*ch;
    Image img = GenImageGradientLinear(W, H, 45, RED, BLUE);
    Color *px = LoadImageColors(img);

    unsigned char *Y = malloc(n), *Cr = malloc(n), *Cb = malloc(n);
    unsigned char *sCr = malloc(cn), *sCb = malloc(cn);
    unsigned char *rle = malloc(n*2);
    for (int i = 0; i < n; i++) rgb2ycrcb(px[i], &Y[i], &Cr[i], &Cb[i]);
    for (int r = 0; r < ch; r++)
        for (int c = 0; c < cw; c++) {
            int i = r*2*W+c*2;
            sCr[r*cw+c] = (Cr[i]+Cr[i+1]+Cr[i+W]+Cr[i+W+1])/4;
            sCb[r*cw+c] = (Cb[i]+Cb[i+1]+Cb[i+W]+Cb[i+W+1])/4;
        }
    int rlesz = rle_encode(Y, n, rle);
    long uncomp = n*3, comp = rlesz + cn*2 + 12;

    Image rec = GenImageColor(W, H, BLACK);
    int loaded = 0;

    InitWindow(800, 600, "8.3V YCrCb 4:2:0 + RLE");
    SetTargetFPS(60);
    Texture tOrig = LoadTextureFromImage(img), tRec = LoadTextureFromImage(rec);

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_S)) {
            FILE *f = fopen("output_420rle.bin", "wb");
            fwrite(&W, 4, 1, f); fwrite(&H, 4, 1, f); fwrite(&rlesz, 4, 1, f);
            fwrite(rle, 1, rlesz, f); fwrite(sCr, 1, cn, f); fwrite(sCb, 1, cn, f);
            fclose(f);
        }
        if (IsKeyPressed(KEY_L)) {
            FILE *f = fopen("output_420rle.bin", "rb");
            if (f) {
                int w2, h2, rs; fread(&w2, 4, 1, f); fread(&h2, 4, 1, f); fread(&rs, 4, 1, f);
                fread(rle, 1, rs, f); fread(sCr, 1, cn, f); fread(sCb, 1, cn, f);
                fclose(f);
                rle_decode(rle, rs, Y);
                for (int r = 0; r < H; r++)
                    for (int c = 0; c < W; c++)
                        ((Color*)rec.data)[r*W+c] = ycrcb2rgb(Y[r*W+c], sCr[(r/2)*cw+c/2], sCb[(r/2)*cw+c/2]);
                UnloadTexture(tRec); tRec = LoadTextureFromImage(rec);
                loaded = 1;
            }
        }
        BeginDrawing();
        ClearBackground(DARKGRAY);
        DrawTexture(tOrig, 0, 0, WHITE);
        if (loaded) DrawTexture(tRec, 400, 0, WHITE);
        DrawText("Original", 10, 310, 20, WHITE);
        DrawText("Reconstructed 4:2:0+RLE (S/L)", 410, 310, 20, WHITE);
        DrawText(TextFormat("Uncompressed: %ld B", uncomp), 10, 340, 20, YELLOW);
        DrawText(TextFormat("Compressed: %ld B (%.1f%%)", comp, 100.0f*comp/uncomp), 10, 365, 20, YELLOW);
        EndDrawing();
    }
    free(Y); free(Cr); free(Cb); free(sCr); free(sCb); free(rle);
    UnloadImageColors(px); UnloadTexture(tOrig); UnloadTexture(tRec);
    UnloadImage(img); UnloadImage(rec); CloseWindow();
}
