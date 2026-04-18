// Задание 13Б: Трассировка лучей + отражения, плоскость-пол, шахматный паттерн.
#include "raylib.h"
#include <math.h>

typedef struct { float x, y, z; } V3;
typedef struct { float r, g, b; } Col;
typedef struct { V3 c; float rad; Col col; float refl; } Sphere;

V3 v3(float x, float y, float z) { return (V3){x, y, z}; }
V3 vsub(V3 a, V3 b) { return v3(a.x-b.x, a.y-b.y, a.z-b.z); }
V3 vadd(V3 a, V3 b) { return v3(a.x+b.x, a.y+b.y, a.z+b.z); }
V3 vmul(V3 a, float t) { return v3(a.x*t, a.y*t, a.z*t); }
float vdot(V3 a, V3 b) { return a.x*b.x+a.y*b.y+a.z*b.z; }
V3 vnorm(V3 a) { float l=sqrtf(vdot(a,a)); return vmul(a,1.0f/l); }
V3 vreflect(V3 d, V3 n) { return vsub(d, vmul(n, 2*vdot(d,n))); }

float hitSphere(V3 o, V3 d, Sphere s) {
    V3 oc = vsub(o, s.c);
    float b = vdot(oc, d), c = vdot(oc, oc) - s.rad*s.rad, disc = b*b - c;
    if (disc < 0) return -1;
    float t = -b - sqrtf(disc);
    return t > 0.001f ? t : -1;
}

#define NS 3
Sphere spheres[NS] = {
    {{0, 0, 8}, 1.5f, {0.8f, 0.2f, 0.2f}, 0.8f},
    {{-2.5f, 0.5f, 6}, 1.0f, {0.2f, 0.8f, 0.2f}, 0.6f},
    {{2.5f, -0.3f, 7}, 1.2f, {0.2f, 0.2f, 0.9f}, 0.9f},
};
V3 light = {5, 5, -2};
float floorY = -1.5f;

float hitFloor(V3 o, V3 d) {
    if (fabsf(d.y) < 1e-6f) return -1;
    float t = (floorY - o.y) / d.y;
    return t > 0.001f ? t : -1;
}

int inShadow(V3 p) {
    V3 ld = vnorm(vsub(light, p));
    for (int i = 0; i < NS; i++) if (hitSphere(p, ld, spheres[i]) > 0) return 1;
    return 0;
}

Col trace(V3 o, V3 d, int depth) {
    if (depth <= 0) return (Col){0, 0, 0};
    float tmin = 1e9f; int hit = -1; int isFloor = 0;
    for (int i = 0; i < NS; i++) { float t = hitSphere(o, d, spheres[i]); if (t > 0 && t < tmin) { tmin = t; hit = i; } }
    float tf = hitFloor(o, d);
    if (tf > 0 && tf < tmin) { tmin = tf; isFloor = 1; hit = -1; }
    if (hit < 0 && !isFloor) return (Col){0.05f, 0.05f, 0.1f};
    V3 p = vadd(o, vmul(d, tmin));
    if (isFloor) {
        int cx = (int)(floorf(p.x)) + (int)(floorf(p.z));
        Col fc = (cx & 1) ? (Col){0.9f, 0.9f, 0.9f} : (Col){0.2f, 0.2f, 0.2f};
        V3 n = v3(0, 1, 0);
        V3 ld = vnorm(vsub(light, p));
        float diff = vdot(n, ld); if (diff < 0) diff = 0;
        if (inShadow(p)) diff = 0;
        float a = 0.2f;
        Col refl = trace(p, vreflect(d, n), depth - 1);
        float rf = 0.3f;
        return (Col){fc.r*(a+diff)*(1-rf)+refl.r*rf, fc.g*(a+diff)*(1-rf)+refl.g*rf, fc.b*(a+diff)*(1-rf)+refl.b*rf};
    }
    V3 n = vnorm(vsub(p, spheres[hit].c));
    V3 ld = vnorm(vsub(light, p));
    float diff = vdot(n, ld); if (diff < 0) diff = 0;
    if (inShadow(p)) diff = 0;
    float a = 0.15f;
    Col sc = spheres[hit].col;
    float rf = spheres[hit].refl;
    Col base = {sc.r*(a+diff), sc.g*(a+diff), sc.b*(a+diff)};
    Col refl = trace(p, vreflect(d, n), depth - 1);
    return (Col){base.r*(1-rf)+refl.r*rf, base.g*(1-rf)+refl.g*rf, base.b*(1-rf)+refl.b*rf};
}

int main(void) {
    InitWindow(800, 600, "13Б Отражения + пол");
    SetTargetFPS(60);
    Image img = GenImageColor(800, 600, BLACK);
    for (int py = 0; py < 600; py++) for (int px = 0; px < 800; px++) {
        float u = (px - 400.0f) / 400.0f, v = -(py - 300.0f) / 300.0f;
        Col c = trace(v3(0,0,0), vnorm(v3(u, v, 1.0f)), 2);
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
