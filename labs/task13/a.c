// Задание 13А: Трассировка лучей. Непрозрачные сферы, тени.
#include "raylib.h"
#include <math.h>

typedef struct { float x, y, z; } V3;
typedef struct { float r, g, b; } Col;
typedef struct { V3 c; float rad; Col col; } Sphere;

V3 v3(float x, float y, float z) { return (V3){x, y, z}; }
V3 vsub(V3 a, V3 b) { return v3(a.x-b.x, a.y-b.y, a.z-b.z); }
V3 vadd(V3 a, V3 b) { return v3(a.x+b.x, a.y+b.y, a.z+b.z); }
V3 vmul(V3 a, float t) { return v3(a.x*t, a.y*t, a.z*t); }
float vdot(V3 a, V3 b) { return a.x*b.x+a.y*b.y+a.z*b.z; }
V3 vnorm(V3 a) { float l=sqrtf(vdot(a,a)); return vmul(a,1.0f/l); }

float hitSphere(V3 o, V3 d, Sphere s) {
    V3 oc = vsub(o, s.c);
    float b = vdot(oc, d), c = vdot(oc, oc) - s.rad*s.rad, disc = b*b - c;
    if (disc < 0) return -1;
    float t = -b - sqrtf(disc);
    return t > 0.001f ? t : -1;
}

#define NS 4
Sphere spheres[NS] = {
    {{0, 0, 8}, 1.5f, {1, 0.2f, 0.2f}},
    {{-2.5f, 0.5f, 6}, 1.0f, {0.2f, 1, 0.2f}},
    {{2.5f, -0.5f, 7}, 1.2f, {0.2f, 0.2f, 1}},
    {{0, -1.5f, 5}, 0.7f, {1, 1, 0.2f}},
};
V3 light = {5, 5, -2};

Col trace(V3 o, V3 d) {
    float tmin = 1e9f; int hit = -1;
    for (int i = 0; i < NS; i++) { float t = hitSphere(o, d, spheres[i]); if (t > 0 && t < tmin) { tmin = t; hit = i; } }
    if (hit < 0) return (Col){0.1f, 0.1f, 0.15f};
    V3 p = vadd(o, vmul(d, tmin));
    V3 n = vnorm(vsub(p, spheres[hit].c));
    V3 ld = vnorm(vsub(light, p));
    int shadow = 0;
    for (int i = 0; i < NS; i++) if (i != hit && hitSphere(p, ld, spheres[i]) > 0) shadow = 1;
    float diff = vdot(n, ld); if (diff < 0) diff = 0;
    if (shadow) diff = 0;
    float amb = 0.15f;
    Col sc = spheres[hit].col;
    return (Col){sc.r*(amb+diff), sc.g*(amb+diff), sc.b*(amb+diff)};
}

int main(void) {
    InitWindow(800, 600, "13А Трассировка лучей");
    SetTargetFPS(60);
    Image img = GenImageColor(800, 600, BLACK);
    for (int py = 0; py < 600; py++) for (int px = 0; px < 800; px++) {
        float u = (px - 400.0f) / 400.0f;
        float v = -(py - 300.0f) / 300.0f;
        V3 d = vnorm(v3(u, v, 1.0f));
        Col c = trace(v3(0,0,0), d);
        if (c.r > 1) c.r = 1;
        if (c.g > 1) c.g = 1;
        if (c.b > 1) c.b = 1;
        ImageDrawPixel(&img, px, py, (Color){(unsigned char)(c.r*255), (unsigned char)(c.g*255), (unsigned char)(c.b*255), 255});
    }
    ExportImage(img, "output.png");
    Texture2D tex = LoadTextureFromImage(img);
    UnloadImage(img);
    while (!WindowShouldClose()) { BeginDrawing(); DrawTexture(tex, 0, 0, WHITE); EndDrawing(); }
    UnloadTexture(tex);
    CloseWindow();
    return 0;
}
