// Задание 13В: Трассировка лучей + преломление (закон Снеллиуса), рекурсия глубина 3.
#include "raylib.h"
#include <math.h>

typedef struct { float x, y, z; } V3;
typedef struct { float r, g, b; } Col;
typedef struct { V3 c; float rad; Col col; float refl; float transp; float ior; } Sphere;

V3 v3(float x, float y, float z) { return (V3){x, y, z}; }
V3 vsub(V3 a, V3 b) { return v3(a.x-b.x, a.y-b.y, a.z-b.z); }
V3 vadd(V3 a, V3 b) { return v3(a.x+b.x, a.y+b.y, a.z+b.z); }
V3 vmul(V3 a, float t) { return v3(a.x*t, a.y*t, a.z*t); }
float vdot(V3 a, V3 b) { return a.x*b.x+a.y*b.y+a.z*b.z; }
float vlen(V3 a) { return sqrtf(vdot(a,a)); }
V3 vnorm(V3 a) { return vmul(a, 1.0f/vlen(a)); }
V3 vreflect(V3 d, V3 n) { return vsub(d, vmul(n, 2*vdot(d,n))); }

int vrefract(V3 d, V3 n, float eta, V3 *out) {
    float cosi = -vdot(d, n);
    float sin2t = eta * eta * (1 - cosi * cosi);
    if (sin2t > 1) return 0;
    *out = vadd(vmul(d, eta), vmul(n, eta * cosi - sqrtf(1 - sin2t)));
    return 1;
}

float hitSphere(V3 o, V3 d, Sphere s) {
    V3 oc = vsub(o, s.c);
    float b = vdot(oc, d), c = vdot(oc, oc) - s.rad*s.rad, disc = b*b - c;
    if (disc < 0) return -1;
    float sq = sqrtf(disc), t1 = -b - sq, t2 = -b + sq;
    if (t1 > 0.001f) return t1;
    if (t2 > 0.001f) return t2;
    return -1;
}

#define NS 3
Sphere spheres[NS] = {
    {{0, 0, 8}, 1.5f, {0.9f, 0.9f, 0.95f}, 0.1f, 0.9f, 1.5f},
    {{-2.5f, 0.3f, 6}, 1.0f, {0.8f, 0.8f, 0.9f}, 0.9f, 0.0f, 1.0f},
    {{2.5f, -0.2f, 7}, 1.2f, {0.9f, 0.3f, 0.2f}, 0.0f, 0.0f, 1.0f},
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
    for (int i = 0; i < NS; i++) if (spheres[i].transp < 0.5f && hitSphere(p, ld, spheres[i]) > 0) return 1;
    return 0;
}

Col trace(V3 o, V3 d, int depth) {
    if (depth <= 0) return (Col){0, 0, 0};
    float tmin = 1e9f; int hit = -1; int isFloor = 0;
    for (int i = 0; i < NS; i++) { float t = hitSphere(o, d, spheres[i]); if (t > 0 && t < tmin) { tmin = t; hit = i; } }
    float tf = hitFloor(o, d);
    if (tf > 0 && tf < tmin) { tmin = tf; isFloor = 1; hit = -1; }
    if (hit < 0 && !isFloor) return (Col){0.05f, 0.05f, 0.12f};
    V3 p = vadd(o, vmul(d, tmin));
    if (isFloor) {
        int cx = (int)(floorf(p.x)) + (int)(floorf(p.z));
        Col fc = (cx & 1) ? (Col){0.9f, 0.9f, 0.9f} : (Col){0.2f, 0.2f, 0.2f};
        V3 ld = vnorm(vsub(light, p));
        float diff = vdot(v3(0,1,0), ld); if (diff < 0) diff = 0;
        if (inShadow(p)) diff = 0;
        return (Col){fc.r*(0.2f+diff), fc.g*(0.2f+diff), fc.b*(0.2f+diff)};
    }
    V3 n = vnorm(vsub(p, spheres[hit].c));
    int inside = 0;
    if (vdot(d, n) > 0) { n = vmul(n, -1); inside = 1; }
    V3 ld = vnorm(vsub(light, p));
    float diff = vdot(n, ld); if (diff < 0) diff = 0;
    if (inShadow(p)) diff = 0;
    Col sc = spheres[hit].col;
    Col base = {sc.r*(0.15f+diff), sc.g*(0.15f+diff), sc.b*(0.15f+diff)};
    float rf = spheres[hit].refl, tr = spheres[hit].transp;
    Col reflC = {0,0,0}, refrC = {0,0,0};
    if (rf > 0) reflC = trace(p, vreflect(d, n), depth - 1);
    if (tr > 0) {
        float eta = inside ? spheres[hit].ior : 1.0f / spheres[hit].ior;
        V3 refr;
        if (vrefract(d, n, eta, &refr)) refrC = trace(p, vnorm(refr), depth - 1);
        else refrC = trace(p, vreflect(d, n), depth - 1);
    }
    float local = 1.0f - rf - tr; if (local < 0) local = 0;
    return (Col){base.r*local + reflC.r*rf + refrC.r*tr, base.g*local + reflC.g*rf + refrC.g*tr, base.b*local + reflC.b*rf + refrC.b*tr};
}

int main(void) {
    InitWindow(800, 600, "13В Преломление");
    SetTargetFPS(60);
    Image img = GenImageColor(800, 600, BLACK);
    for (int py = 0; py < 600; py++) for (int px = 0; px < 800; px++) {
        float u = (px - 400.0f) / 400.0f, v = -(py - 300.0f) / 300.0f;
        Col c = trace(v3(0,0,0), vnorm(v3(u, v, 1.0f)), 3);
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
