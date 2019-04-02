#ifndef __ATS_TOOL_H__
#define __ATS_TOOL_H__

#include <GL/glu.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

typedef     int32_t     b32;

typedef     float       r32;
typedef     double      r64;

typedef     uint8_t     u8;
typedef     uint16_t    u16;
typedef     uint32_t    u32;
typedef     uint64_t    u64;

typedef     int8_t      i8;
typedef     int16_t     i16;
typedef     int32_t     i32;
typedef     int64_t     i64;

#define setbit(n, b)    ((n) |=  (1 << (b)))
#define clrbit(n, b)    ((n) &= ~(1 << (b)))
#define chkbit(n, b)    (((n) & (1 << (b))) == (1 << (b)))

#define count_of(arr)   (sizeof (arr) / sizeof (arr[0]))

#define false   (0)
#define true    (!false)

#define for_v(v, s, e)  for (int (v) = (int)(s); (v) < (int)(e); ++(v))
#define for_i(s, e)     for_v ((i), (s), (e))
#define for_j(s, e)     for_v ((j), (s), (e))
#define for_k(s, e)     for_v ((k), (s), (e))

#define for_ij(si, ei, sj, ej)  for_j ((sj), (ej)) for_i ((si), (ei))

#define for_all(v, s, e)   for_v ((v), (s), (e) + 1)

#define repeat(n)   for (int __ITER__ = 0; __ITER__ < (n); ++__ITER__)

// ============================================== ScopedExit/Defer =============================================== //

// imitation of golangs defer statment!
template <typename T>
struct Scoped_Exit {
    T func;
    //
     Scoped_Exit(T func) : func(func) {}
    ~Scoped_Exit() { func(); }
};

// private macros to help add the line number to defer! to avoid name clashes!
#define __ADD_LINE_NUM1(arg1, arg2) __ADD_LINE_NUM2(arg1, arg2)
#define __ADD_LINE_NUM2(arg1, arg2) arg1 ## arg2

// making ScopedExit abit nicer!
#define defer Scoped_Exit __ADD_LINE_NUM1(scoped_exit_, __LINE__) = [&]

// ===================================================== 1D MATH ================================================ //

#define MIN(a, b)   ((a) < (b)? (a) : (b))
#define MAX(a, b)   ((a) > (b)? (a) : (b))

static const r32 PI = 3.14159265359f;

#define to_rad(deg) ((deg) * ((PI) / 180.0f))
#define to_deg(rad) ((rad) * (180.0f / (PI)))

#define fabsf   __builtin_fabsf
#define cosf    __builtin_cosf
#define sinf    __builtin_sinf
#define tanf    __builtin_tanf
#define roundf  __builtin_roundf
#define floorf  __builtin_floorf
#define ceilf   __builtin_ceilf
#define sqrtf   __builtin_sqrtf
#define atan2f  __builtin_atan2f

template <typename T> inline constexpr T square (T n)               { return n * n; }
template <typename T> inline constexpr T lerp   (T a, T b, r32 t)   { return a + t * (b - a); }
template <typename T> inline constexpr T unlerp (T a, T b, r32 t)   { return (t - a) / (b - a); }

constexpr r32 rsqrt(r32 n) { return n == 0? n : 1.0f / sqrtf(n); }

#define round_up_to_multiple_of_8(N) ((N + 7) & ~7)

// ======================================= DYNAMIC HEAP ARRAY ======================================== //

template <typename T>
struct Array {
    i64         _cap;
    i64         _len;
    T*          _buf;

    T&       operator[](i64 i)          { return _buf[i]; }
    const T& operator[](i64 i) const    { return _buf[i]; }

    inline i64      cap   () const      { return _cap; }
    inline i64      len   () const      { return _len; }

    inline T*       ptr   ()            { return _buf; }
    inline const T* ptr   () const      { return _buf; }

    inline T*       get   (i64 i)       { return &_buf[i]; }
    inline const T* get   (i64 i) const { return &_buf[i]; }

    inline void     clear   ()            { _len = 0; }
    inline void     destroy ()            { free(_buf); *this = {}; }
    inline void     rem     (i64 i)       { _buf[i] = _buf[--_len]; }

    inline void grow(i64 n) {
        if (_len + n >= _cap) {
            _cap =  round_up_to_multiple_of_8(MAX(_cap << 1, _len + n));
            _buf =  (T*)realloc(_buf, sizeof (T) * _cap);
        }
    }

    inline void reserve(i64 size) { if (size > _cap) { grow(size - _cap); } }

    inline void resize(i64 size) { reserve(size); _len = size; }

    inline void add(const T& e) {
        grow(1);
        _buf[_len++] = e;
    }

    inline T* create() {
        grow(1);
        T* e    = &_buf[_len++];
        *e      = {};
        return e;
    }
};

template <typename T> inline static Array<T> array_make() { return { 0, 0, nullptr }; }

// ============================================= STACK ARRAY ========================================= //

template <typename T, i64 N>
struct Stack {
    // @data:
    i64         _len;
    T           _buf[round_up_to_multiple_of_8(N)];
    
    inline       T& operator[](i32 i)       { return _buf[i]; }
    inline const T& operator[](i32 i) const { return _buf[i]; }

    inline void  init   ()              { _len = 0; }

    inline T*       get    (i64 i)         { return &_buf[i]; }
    inline const T* get    (i64 i) const   { return &_buf[i]; }

    inline T*       ptr    ()              { return _buf; }
    inline const T* ptr    () const        { return _buf; }

    inline void     add    (const T& e)    { _buf[_len++] = e; }
    inline void     rem    (i64 i)         { _buf[i] = _buf[--_len]; }
    inline T*       create ()              { T* e = get(_len++); *e = {}; return e; }
    inline void     resize (i64 new_len)   { _len = new_len; }
    inline void     clear  ()              { _len = 0; }
    inline u32      len    ()              { return _len; }
    inline u32      cap    ()              { return N; }
};

// ================================================== FILE IO ============================================= //

static size_t file_get_size(FILE* fp) {
    fseek(fp, 0, SEEK_END);
    size_t size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    return size;
}

static char* file_read_str(const char* file_name) {
    char*   buffer  = NULL;
    
    if (auto* fp = fopen(file_name, "r")) {
        size_t size = file_get_size(fp);

        buffer = (char*)malloc(sizeof (char) * (size + 1));

        if (buffer) {
            buffer[size] = 0;

            if (fread(buffer, 1, size, fp) == 0) {
                free(buffer);
                buffer = 0;
            }
        }

        fclose(fp);
    }

    return buffer;
}

static b32 file_write_str(const char* file_name, const char* buffer) {
    if (auto* fp = fopen(file_name, "w")) {
        size_t size = strlen(buffer);
        size_t n    = fwrite(buffer, 1, size, fp);
        fclose(fp);
        return n == size;
    }

    return false;
}

static b32 file_append_str(const char* file_name, const char* buffer) {
    if (auto* fp = fopen(file_name, "a")) {
        size_t size = strlen(buffer);
        size_t n    = fwrite(buffer, 1, size, fp);

        fclose(fp);

        return n == size;
    }
    return false;
}

// ============================================== BINARY FILE IO ================================================== //

static b32 file_read_bin(const char* file_name, void* data, size_t size) {
    if (auto* fp = fopen(file_name, "rb")) {
        fread(data, size, 1, fp);
        fclose(fp);
        return true;
    }
    return false;
} 

static b32 file_write_bin(const char* file_name, const void* data, size_t size) {
    if (auto* fp = fopen(file_name, "wb")) {
        fwrite(data, size, 1, fp);
        fclose(fp);
        return true;
    }
    return false;
}

// ================================================ MATH 2D =========================================== //

struct v2 { r32 x, y; };

inline constexpr void  operator+=(v2& a, v2 b)   { a.x += b.x;  a.y += b.y; }
inline constexpr void  operator-=(v2& a, v2 b)   { a.x -= b.x;  a.y -= b.y; }
inline constexpr void  operator*=(v2& a, r32 s)  { a.x *= s;    a.y *= s; }
inline constexpr void  operator/=(v2& a, r32 s)  { a.x /= s;    a.y /= s; }

inline constexpr v2    operator+ (v2 a, v2 b)    { return { a.x + b.x, a.y + b.y }; }
inline constexpr v2    operator- (v2 a, v2 b)    { return { a.x - b.x, a.y - b.y }; }

inline constexpr v2    operator* (v2 a, r32 s)   { return { a.x * s, a.y * s }; }
inline constexpr v2    operator* (r32 s, v2 a)   { return { a.x * s, a.y * s }; }

inline constexpr v2    operator/ (v2  a, r32 s)  { return { a.x / s, a.y / s }; }
inline constexpr v2    operator/ (r32 s, v2  a)  { return { a.x / s, a.y / s }; }

inline constexpr r32   dot          (v2 a, v2 b)    { return a.x * b.x + a.y * b.y; }
inline constexpr r32   len_sq       (v2 a)          { return dot(a, a); }
inline constexpr r32   len          (v2 a)          { return sqrtf(dot(a, a)); }
inline constexpr r32   rlen         (v2 a)          { return rsqrt(dot(a, a)); }
inline constexpr v2    norm         (v2 a)          { return a * rlen(a); }
inline constexpr r32   dist         (v2 a, v2 b)    { return len(a - b);  }
inline constexpr r32   dist_sq      (v2 a, v2 b)    { return len_sq(a - b); }

inline constexpr r32   det          (v2 a, v2 b)    { return a.x * b.y - b.x * a.y; }
inline constexpr r32   cross        (v2 a, v2 b)    { return a.x * b.y - a.y * b.x; }

//inline constexpr v2 lerp(v2 a, v2 b, r32 t) { return { lerp(a.x, b.x, t), lerp(a.y, b.y, t) }; }

// HERE
inline constexpr r32 get_angle(v2 a, v2 b) { return atan2f(det(a, b), dot(a, b)); }

inline constexpr v2 get_dir(v2 from, v2 to) {
    return norm(to - from);
}

inline constexpr v2 round(v2 a) { return { roundf(a.x), roundf(a.y) }; }

inline constexpr v2 rotate_args(v2 a, r32 c, r32 s) {
    return { a.x * c - a.y * s, a.x * s + a.y * c };
}

inline constexpr v2 rotate(v2 a, r32 rad) {
    r32 c = cosf(rad);
    r32 s = sinf(rad);

    return { a.x * c - a.y * s, a.x * s + a.y * c };
}

inline constexpr void rotate_us(v2* us, i32 size, r32 rad) {
    r32 c = cosf(rad);
    r32 s = sinf(rad);

    for (i32 i = 0; i < size; i++) {
        us[i] = { c * us[i].x - s * us[i].y, s * us[i].x + c * us[i].y };
    }
}

inline static void print(v2 v) { printf("x: %.2f, y: %.2f\n", v.x, v.y); }

// @NOTE: row major!
struct m2 {
    v2 x, y;
};

inline constexpr m2 m2_rot(r32 angle) {
    r32 c = cosf(angle);
    r32 s = sinf(angle);

    return { { c, -s }, { s,  c } };
}

inline constexpr void  operator+=(m2& a, m2 b)   { a.x += b.x;  a.y += b.y; }
inline constexpr void  operator-=(m2& a, m2 b)   { a.x -= b.x;  a.y -= b.y; }
inline constexpr void  operator*=(m2& a, r32 s)  { a.x *= s;    a.y *= s; }
inline constexpr void  operator/=(m2& a, r32 s)  { a.x /= s;    a.y /= s; }

inline constexpr m2    operator+ (m2 a, m2 b)    { return { a.x + b.x, a.y + b.y }; }
inline constexpr m2    operator- (m2 a, m2 b)    { return { a.x - b.x, a.y - b.y }; }

inline constexpr m2    operator* (m2 a, r32 s)   { return { a.x * s, a.y * s }; }
inline constexpr m2    operator* (r32 s, m2 a)   { return { a.x * s, a.y * s }; }

inline constexpr m2    operator/ (m2  a, r32 s)  { return { a.x / s, a.y / s }; }
inline constexpr m2    operator/ (r32 s, m2  a)  { return { a.x / s, a.y / s }; }

inline constexpr v2    operator*(m2 a, v2 u) { return { dot(a.x, u), dot(a.y, u) }; }

// ================================================ MATH 3D =========================================== //

union v3 {
    struct { r32 x, y, z; };
    v2 xy;
};

inline constexpr void  operator+=  (v3& a, v3 b)   { a.x += b.x;  a.y += b.y;    a.z += b.z; }
inline constexpr void  operator-=  (v3& a, v3 b)   { a.x -= b.x;  a.y -= b.y;    a.z -= b.z; }
inline constexpr void  operator*=  (v3& a, r32 s)  { a.x *= s;    a.y *= s;      a.z *= s; }
inline constexpr void  operator/=  (v3& a, r32 s)  { a.x /= s;    a.y /= s;      a.z /= s; }

inline constexpr v3    operator+      (v3 a,  v3  b)   { return { a.x + b.x, a.y + b.y, a.z + b.z }; }
inline constexpr v3    operator-      (v3 a,  v3  b)   { return { a.x - b.x, a.y - b.y, a.z - b.z }; }
inline constexpr v3    operator*      (v3 a,  r32 s)   { return { a.x * s, a.y * s, a.z * s }; }
inline constexpr v3    operator*      (r32 s, v3  a)   { return { a.x * s, a.y * s, a.z * s }; }
inline constexpr v3    operator/      (v3 a,  r32 s)   { return { a.x / s, a.y / s, a.z / s }; }

inline constexpr v3    x_axis   ()              { return { 1, 0, 0 }; }
inline constexpr v3    y_axis   ()              { return { 0, 1, 0 }; }
inline constexpr v3    z_axis   ()              { return { 0, 0, 1 }; }

inline constexpr b32   equal    (v3 a, v3 b)    { return a.x == b.x && a.y == b.y && a.z == b.y; }

inline constexpr r32   dot        (v3 u, v3 v)  { return u.x * v.x + u.y * v.y + u.z * v.z; }
inline constexpr r32   len_sq     (v3 a)        { return dot(a, a); }
inline constexpr r32   len        (v3 a)        { return sqrtf(dot(a, a)); }
inline constexpr r32   rlen       (v3 a)        { return rsqrt(dot(a, a)); }
inline constexpr v3    norm       (v3 a)        { return a * rlen(a); }
inline constexpr r32   dist       (v3 a, v3 b)  { return len(a - b);  }
inline constexpr r32   dist_sq    (v3 a, v3 b)  { return len_sq(a - b); }

//inline constexpr v3 lerp(v3 a, v3 b, r32 t) { return { lerp(a.x, b.x, t), lerp(a.y, b.y, t), lerp(a.z, b.z, t) }; }

inline constexpr v3 cross(v3 u, v3 v) {
    return {
        u.y * v.z - v.y * u.z,
        u.z * v.x - v.z * u.x,
        u.x * v.y - v.x * u.y
    };
}

inline constexpr v3 rotate_xy(v3 u, r32 rot) {
    u.xy = rotate(u.xy, rot);
    return u;
}

// Rodrigues rotation formula!
// r:       result
// k:       axis to rotate around, has to be normalized!
inline constexpr v3 rotate(v3 v, r32 angle, v3 k) {
    r32 c   = cosf(angle);
    r32 c1  = 1 - c;
    r32 s   = sinf(angle);
    v3  cr  = cross(k, v);

    return {
        v.x + cr.x * s + k.x * c1,
        v.y + cr.y * s + k.x * c1,
        v.z + cr.z * s + k.x * c1
    };
}

// ================================================ MATH 4D =========================================== //

struct v4 { r32 x, y, z, w; };

inline constexpr void operator+=(v4& a, v4 b)  { a.x += b.x;   a.y += b.y; a.z += b.z; a.w += b.w; }
inline constexpr void operator-=(v4& a, v4 b)  { a.x -= b.x;   a.y -= b.y; a.z -= b.z; a.w -= b.w; }
inline constexpr void operator*=(v4& a, r32 s) { a.x *= s;     a.y *= s;   a.z *= s;   a.w *= s; }
inline constexpr void operator/=(v4& a, r32 s) { a.x /= s;     a.y /= s;   a.z /= s;   a.w /= s; }

inline constexpr v4 operator+(v4 a,  v4 b)  { return { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w }; }
inline constexpr v4 operator-(v4 a,  v4 b)  { return { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w }; }

inline constexpr v4 operator*(v4  a, r32 s) { return { a.x * s, a.y * s, a.z * s, a.w * s }; }
inline constexpr v4 operator*(r32 s, v4  a) { return { a.x * s, a.y * s, a.z * s, a.w * s }; }

inline constexpr v4 operator/(v4  a, r32 s) { return { a.x / s, a.y / s, a.z / s, a.w / s }; }
inline constexpr v4 operator/(r32 s, v4  a) { return { a.x / s, a.y / s, a.z / s, a.w / s }; }

inline constexpr r32 dot      (v4 u, v4 v) { return u.x * v.x + u.y * v.y + u.z * v.z + u.w * v.w; }
inline constexpr r32 len_sq   (v4 a)       { return dot(a, a); }
inline constexpr r32 len      (v4 a)       { return sqrtf(dot(a, a)); }
inline constexpr r32 rlen     (v4 a)       { return rsqrt(dot(a, a)); }
inline constexpr v4  norm     (v4 a)       { return a * rlen(a); }
inline constexpr r32 dist     (v4 a, v4 b) { return len(a - b);  }
inline constexpr r32 dist_sq  (v4 a, v4 b) { return len_sq(a - b); }

// =================================================== RECT 2D ====================================================== //

struct Rect2 {
    v2      min;
    v2      max;
};

static b32 is_point_in_rect(const Rect2 rect, v2 point) {
    if (point.x < rect.min.x || point.x > rect.max.x) { return false; }
    if (point.y < rect.min.x || point.y > rect.max.y) { return false; }
    return true;
}

static b32 is_rects_intersecting(const Rect2 a, const Rect2 b) {
    if (a.max.x < b.min.x || b.max.x < a.min.x) { return false; }
    if (a.max.y < b.min.y || b.max.y < a.min.y) { return false; }
    return true;
}

// =================================================== RECT 3D ====================================================== //

struct Rect3 {
    v3      min;
    v3      max;
};

static b32 is_point_in_rect(const Rect3 rect, v3 point) {
    if (point.x < rect.min.x || point.x > rect.max.x) { return false; }
    if (point.y < rect.min.y || point.y > rect.max.y) { return false; }
    if (point.z < rect.min.z || point.z > rect.max.z) { return false; }
    return true;
}

static b32 is_rects_intersecting(const Rect3 a, const Rect3 b) {
    if (a.max.x < b.min.x || b.max.x < a.min.x) { return false; }
    if (a.max.y < b.min.y || b.max.y < a.min.y) { return false; }
    if (a.max.z < b.min.z || b.max.z < a.min.z) { return false; }
    return true;
}

// =============================================== COLOR ======================================================= //

struct Color {
    u8      r;
    u8      g;
    u8      b;
    u8      a;
};

inline constexpr Color color_lerp(Color a, Color b, r32 t) {
    return {
        lerp(a.r, b.r, t),
        lerp(a.g, b.g, t),
        lerp(a.b, b.b, t),
        lerp(a.a, b.a, t)
    };
}

// ================================================== RANDOM ==================================================== //

struct Rnd_Gen {
    u32     x;
    u32     y;
    u32     z;
    
    constexpr u32 gen();
};

constexpr u32 Rnd_Gen::gen() {
    x   ^= x << 16;
    x   ^= x >> 5;
    x   ^= x << 1;

    u32 t  = x;

    x   = y;
    y   = z;
    z   = t ^ x ^ y;

    return z;
}

static Rnd_Gen default_rnd = { 123456789u, 362436069u, 521288629u };

inline static int randi  (int min, int max) { return min + default_rnd.gen() % (max - min); }
inline static r32 randf  (r32 min, r32 max) { return min + ((r32)default_rnd.gen() / (r32)0xFFFFFFFF) * (max - min);  }

inline static v2 randv2(r32 min, r32 max) {
    return randf(min, max) * norm(v2 { r32(default_rnd.gen()), r32(default_rnd.gen()) });
}

inline static v3 randv3(r32 min, r32 max) {
    return randf(min, max) * norm(v3 { r32(default_rnd.gen()), r32(default_rnd.gen()), r32(default_rnd.gen()) });
}

// =============================================== TIMER ======================================================= //

typedef r32 Timer;

static Timer    timer_create()                   { return (r32)glfwGetTime(); }
static r32      timer_elapsed(const Timer* t)    { return (r32)glfwGetTime() - *t; }
static r32      timer_restart(Timer* t)          { r32 e = timer_elapsed(t); *t = (r32)glfwGetTime(); return e; }

// ======================================= TEXTURES ======================================== //

#ifdef ATS_TEXTURES

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include "stb_image.h" 

struct Texture {
    u32         id;
    i32         width;
    i32         height;
};

static Texture texture_load(const char* path, i32 is_smooth) {
    Texture texture = {0};
    i32 channels    = 0;
    u8* pixels      = NULL;

    pixels = stbi_load(path, &texture.width, &texture.height, &channels, 0);

    assert(pixels);

    glGenTextures(1, &texture.id);
    glBindTexture(GL_TEXTURE_2D, texture.id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.width, texture.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, is_smooth ? GL_LINEAR : GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, is_smooth ? GL_LINEAR : GL_NEAREST);

    stbi_image_free(pixels);

    return texture;
}

static void texture_bind(const Texture* texture) {
    glBindTexture(GL_TEXTURE_2D, texture->id);
}

#endif

// ================================================= OPENGL ================================================== //

static void gl_light_set (int light, int type, v4 arg) { glLightfv(GL_LIGHT0 + light, type, (r32*)&arg); }
static void gl_light_set (int light, int type, r32 arg) { glLightf(GL_LIGHT0 + light, type, arg); }

static void gl_light_set_ambient    (int light, v4 arg) { glLightfv(GL_LIGHT0 + light, GL_AMBIENT, (r32*)&arg); }
static void gl_light_set_diffuse    (int light, v4 arg) { glLightfv(GL_LIGHT0 + light, GL_DIFFUSE, (r32*)&arg); }
static void gl_light_set_specular   (int light, v4 arg) { glLightfv(GL_LIGHT0 + light, GL_SPECULAR, (r32*)&arg); }
static void gl_light_set_position   (int light, v4 arg) { glLightfv(GL_LIGHT0 + light, GL_POSITION, (r32*)&arg); }

static void gl_light_set_material(int face, int type, v4 color) { glMaterialfv(face, type, (r32*)&color); }
static void gl_light_set_material(int face, int type, r32 val)  { glMaterialf(face, type, val); }

static void gl_light_global_ambient(v4 color) { glLightModelfv(GL_LIGHT_MODEL_AMBIENT, (r32*)&color); }

void gl_light_simple(int light, r32 bright, r32 x, r32 y, r32 z) {
    r32 d = (r32) (1.0f / sqrtf(x*x+y*y+z*z));

    gl_light_set(light, GL_POSITION, { x * d, y * d, z * d, 0 });
    gl_light_set(light, GL_DIFFUSE,  { bright, bright, bright, 0 });
    gl_light_set(light, GL_AMBIENT,  {});
    gl_light_set(light, GL_SPECULAR, {});

    glEnable(GL_LIGHT0 + light);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
}

// ================================================= WINDOW =================================================== //

struct Key_Event {
    int         key;
    int         type;
};

inline static b32 _is_key_event(Key_Event* e, int key, int type) { return e->key == key && e->type == type; }

#define is_key_event(e, key, type)  _is_key_event(e, GLFW_KEY_##key, GLFW_##type)

typedef Stack<Key_Event, 256> Key_Events;

static Key_Events key_events;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Key_Event* event = key_events.create();

    event->key          = key;
    event->type         = action;
}

typedef     GLFWwindow*     Render_Window;

// @NOTE: vsync - 1 = 60, 2 = 30,
static Render_Window window_create(i32 width, i32 height, const char* title, u32 vsync) {
    glfwInit();
    glfwWindowHint(GLFW_SAMPLES, 8);

    GLFWwindow* window = glfwCreateWindow(width, height, title, glfwGetPrimaryMonitor(), NULL);
    
    assert(window);

    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, key_callback);

    glfwSwapInterval(vsync);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); 
    glClearDepth(1.0f);
    glDepthFunc(GL_LESS);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    glAlphaFunc(GL_GREATER, 0.0);
    glEnable(GL_ALPHA_TEST);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    {
        //gl_light_global_ambient({ 0.2, 0.2, 0.2, 1.0 });
        //gl_light_simple(0, 0.9, 0, 0, -2);
        //glEnable(GL_LIGHTING);
    }


    //glEnable(GL_AUTO_NORMAL);
    glEnable(GL_NORMALIZE);
    return window;
}

static void window_destroy(Render_Window window) {
    glfwTerminate();
}

static inline b32  window_is_open (Render_Window window)  { return !glfwWindowShouldClose(window); }
static inline void window_close   (Render_Window window)  { glfwSetWindowShouldClose(window, 1); }

static inline void window_set_title(Render_Window window, const char* title) { glfwSetWindowTitle(window, title); }

static inline void window_clear(Render_Window window) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

static inline void window_update(Render_Window window) {
    key_events.clear();

    glfwSwapBuffers(window);
    glfwPollEvents();
}

static v2 window_get_mouse_position(Render_Window window) {
    r64 x = 0;
    r64 y = 0;

    glfwGetCursorPos(window, &x, &y);

    return { (r32)x, (r32)y };
}

static v2 window_get_size(Render_Window window) {
    i32 w = 0;
    i32 h = 0;

    glfwGetWindowSize(window, &w, &h);

    return { (r32)w, (r32)h };
}

static void window_update_view(
        Render_Window window,
        r32 pos_x,  r32 pos_y,       r32 pos_z,
        r32 look_x, r32 look_y,      r32 look_z,
        r32 up_x,   r32 up_y,        r32 up_z,
        r32 fov,    r32 near_plane,  r32 far_plane) {
    i32 w = 0;
    i32 h = 0;

    glfwGetWindowSize(window, &w, &h);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glViewport(0, 0, w, h);
    gluPerspective(fov, (r32)w / (r32)h, near_plane, far_plane);
    gluLookAt(pos_x,    pos_y,  pos_z,
              look_x,   look_y, look_z,
              up_x,     up_y,   up_z);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

static v3 get_3d_point(v2 pos) {
    i32 viewport[4];
    r64 model_view[16];
    r64 projection[16];

    r32 win_x, win_y, win_z;
    r64 pos_x, pos_y, pos_z;

    glGetDoublev(GL_MODELVIEW_MATRIX, model_view);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);

    win_x = pos.x;
    win_y = viewport[3] - pos.y;
    glReadPixels(pos.x, win_y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &win_z);

    gluUnProject(win_x, win_y, win_z, model_view, projection, viewport, &pos_x, &pos_y, &pos_z);

    return { r32(pos_x), r32(pos_y), r32(pos_z) };
}

#define is_key_pressed(window, key)             (glfwGetKey(window, GLFW_KEY_##key) == GLFW_PRESS)

#define render_begin(type)                      (glBegin(GL_##type))
#define render_end()                            (glEnd())

#define render_color(r, g, b, a)                (glColor4ub((r), (g), (b), (a)))
#define render_vertex(x, y, z)                  (glVertex3f((x), (y), (z)))

#define render_col_vertex(x, y, z, r, g, b, a)  (glColor4ub((r), (g), (b), (a)),    glVertex3f((x), (y), (z)))
#define render_tex_vertex(x, y, z, tx, ty)      (glTexCoord2f((tx), (ty)),          glVertex3f((x), (y), (z)))

u32 displaylist_cube;
static void render_init(){
    displaylist_cube = glGenLists(1);

    // compile the display list, store a triangle in it
    glNewList(displaylist_cube, GL_COMPILE);
        render_begin(QUADS);
        // UP
        render_vertex(-1, -1, -1);
        render_vertex(1, -1, -1);
        render_vertex(1, 1, -1);
        render_vertex(-1, 1, -1);
        // Right
        render_vertex(-1, 1, -1);
        render_vertex(-1, 1, 1);
        render_vertex(1, 1, 1);
        render_vertex(1, 1, -1);
        // LEFT
        render_vertex(-1, -1, -1);
        render_vertex(-1, -1, 1);
        render_vertex(1, -1, 1);
        render_vertex(1, -1, -1);
        // FRONT
        render_vertex(-1, -1, -1);
        render_vertex(-1, -1, 1);
        render_vertex(-1, 1, 1);
        render_vertex(-1, 1, -1);

        render_end();
    glEndList();
}

static void render_rectangle(r32 px, r32 py, r32 qx, r32 qy, r32 z, u8 r, u8 g, u8 b, u8 a) {
    render_color(r, g, b, a);

    render_begin(QUADS);
    //
    render_vertex(px, py, z);
    render_vertex(qx, py, z);
    render_vertex(qx, qy, z);
    render_vertex(px, qy, z);
    //
    render_end();
}

static void render_cube(r32 px, r32 py, r32 qx, r32 qy, r32 pz, r32 qz, u8 r, u8 g, u8 b, u8 a) {

    render_color(r, g, b, a);

    glLoadIdentity();
    glTranslatef(px+(qx-px)/2, py+(qy-py)/2, pz+(qz-pz)/2);
    glScalef((qx-px)/2,(qy-py)/2,(qz-pz)/2);

    glCallList(displaylist_cube);

    /*render_begin(QUADS);
    // UP
    render_vertex(-1, -1, -1);
    render_vertex(1, -1, -1);
    render_vertex(1, 1, -1);
    render_vertex(-1, 1, -1);

    //render_color(r, g, b, a*0.9);
    
    // Right
    render_vertex(-1, 1, -1);
    render_vertex(-1, 1, 1);
    render_vertex(1, 1, 1);
    render_vertex(1, 1, -1);
    // LEFT
    render_vertex(-1, -1, -1);
    render_vertex(-1, -1, 1);
    render_vertex(1, -1, 1);
    render_vertex(1, -1, -1);
    // FRONT
    render_vertex(-1, -1, -1);
    render_vertex(-1, -1, 1);
    render_vertex(-1, 1, 1);
    render_vertex(-1, 1, -1);

    render_end();*/
}

static void render_triangle(
        r32 p0_x, r32 p0_y, r32 p0_z,
        r32 p1_x, r32 p1_y, r32 p1_z,
        r32 p2_x, r32 p2_y, r32 p2_z,
        u8 r, u8 g, u8 b, u8 a) {
    render_color(r, g, b, a);

    render_begin(TRIANGLES);
    //
    render_vertex(p0_x, p0_y, p0_z);
    render_vertex(p1_x, p1_y, p1_z);
    render_vertex(p2_x, p2_y, p2_z);
    //
    render_end();
}

// =============================================== VERTEX ARRAY ========================================= //

union Vertex {
    struct {
        v3      pos;
        Color   color;
    };

    struct {
        r32         x;
        r32         y;
        r32         z;
        //
        u8          r;
        u8          g;
        u8          b;
        u8          a;
    };
};

typedef Array<Vertex> Vertex_Array;

static void vertex_array_render(const Vertex_Array* verts) {
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, sizeof (Vertex), &verts->get(0)->x);

    glEnableClientState(GL_COLOR_ARRAY);
    glColorPointer(4, GL_UNSIGNED_BYTE, sizeof (Vertex), &verts->get(0)->r);

    glDrawArrays(GL_TRIANGLES, 0, verts->len());

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}

//#define vertex_array_render(b)   (vertex_array__render_func((b)))

inline static void vertex_array_add_rectangle(Vertex_Array* verts,
                                              r32 px,   r32 py,
                                              r32 qx,   r32 qy,
                                              r32 z, 
                                              u8  r, u8 g, u8 b, u8 a) {
    verts->add({ px, py, z, r, g, b, a });
    verts->add({ px, qy, z, r, g, b, a });
    verts->add({ qx, py, z, r, g, b, a });
    verts->add({ px, qy, z, r, g, b, a });
    verts->add({ qx, qy, z, r, g, b, a });
    verts->add({ qx, py, z, r, g, b, a });
}

inline static void add_vertex(Vertex_Array* verts, v2 pos, r32 z, Color color) {
    verts->add({ pos.x, pos.y, z, color.r, color.g, color.b, color.a });
}

static void vertex_array_add_triangle(Vertex_Array* verts, v3 p, v3 q, v3 r, Color color) {
    verts->add({ p, color });
    verts->add({ q, color });
    verts->add({ r, color });
}

static void vertex_array_add_box(Vertex_Array* verts, v3 min, v3 max, Color color) {
    verts->add({ v3 { min.x, min.y, min.z }, color });
    verts->add({ v3 { max.x, min.y, min.z }, color });
    verts->add({ v3 { min.x, max.y, min.z }, color });
    verts->add({ v3 { max.x, max.y, min.z }, color });
    verts->add({ v3 { min.x, max.y, min.z }, color });
    verts->add({ v3 { max.x, min.y, min.z }, color });
    //
    verts->add({ v3 { min.x, min.y, max.z }, color });
    verts->add({ v3 { max.x, min.y, max.z }, color });
    verts->add({ v3 { min.x, max.y, max.z }, color });
    verts->add({ v3 { max.x, max.y, max.z }, color });
    verts->add({ v3 { min.x, max.y, max.z }, color });
    verts->add({ v3 { max.x, min.y, max.z }, color });
    //
    verts->add({ v3 { min.x, min.y, min.z }, color });
    verts->add({ v3 { min.x, max.y, min.z }, color });
    verts->add({ v3 { min.x, min.y, max.z }, color });
    verts->add({ v3 { min.x, max.y, max.z }, color });
    verts->add({ v3 { min.x, min.y, max.z }, color });
    verts->add({ v3 { min.x, max.y, min.z }, color });
    //
    verts->add({ v3 { max.x, min.y, min.z }, color });
    verts->add({ v3 { max.x, max.y, min.z }, color });
    verts->add({ v3 { max.x, min.y, max.z }, color });
    verts->add({ v3 { max.x, max.y, max.z }, color });
    verts->add({ v3 { max.x, min.y, max.z }, color });
    verts->add({ v3 { max.x, max.y, min.z }, color });
    //
    verts->add({ v3 { min.x, min.y, min.z }, color });
    verts->add({ v3 { max.x, min.y, min.z }, color });
    verts->add({ v3 { min.x, min.y, max.z }, color });
    verts->add({ v3 { max.x, min.y, max.z }, color });
    verts->add({ v3 { min.x, min.y, max.z }, color });
    verts->add({ v3 { max.x, min.y, min.z }, color });
    //
    verts->add({ v3 { min.x, max.y, min.z }, color });
    verts->add({ v3 { max.x, max.y, min.z }, color });
    verts->add({ v3 { min.x, max.y, max.z }, color });
    verts->add({ v3 { max.x, max.y, max.z }, color });
    verts->add({ v3 { min.x, max.y, max.z }, color });
    verts->add({ v3 { max.x, max.y, min.z }, color });
}

// ================================================== TILEMAP ========================================= //

#ifdef ATS_TILEMAP

#define STB_PERLIN_IMPLEMENTATION
#include "stb_perlin.h" 

enum Tile_Attribute {
    Tile_None       = 0,
    Tile_Exit_N     = (1 << 0),
    Tile_Exit_S     = (1 << 1),
    Tile_Exit_W     = (1 << 2),
    Tile_Exit_E     = (1 << 3),
    Tile_Room       = (1 << 4),
};

struct Tilemap {
    // data:
    i32         width;
    i32         height;
    Array<u32>  tiles;

    // methods:
    inline u32  get     (i32 x, i32 y)           const;
    inline void set     (i32 x, i32 y, u32 tile);
};

inline void Tilemap::set(i32 x, i32 y, u32 tile) {
    if (x < 0 || x >= width)    { return; }
    if (y < 0 || y >= height)   { return; }

    tiles[y * width + x] = tile;
}

inline u32  Tilemap::get(i32 x, i32 y) const {
    if (x < 0 || x >= width)    { return 0; }
    if (y < 0 || y >= height)   { return 0; }

    return tiles[y * width + x];
}

inline void tilemap_init(Tilemap* tiles, int w, int h) {
    tiles->width    = w;
    tiles->height   = h;

    tiles->tiles.resize(w * h);
}

inline void tilemap_destroy(Tilemap* tiles) {
    tiles->tiles.destroy();

    tiles->width   = 0;
    tiles->height  = 0;
}

inline static void add_bit(Tilemap* tiles, i32 x, i32 y, u32 bit) {
    tiles->tiles[y * tiles->width + x] |= bit;
}

static void tilemap_add_tile(Tilemap* tiles, int x, int y, u32 type, i32 tile_size) {
    i32 ht = tile_size / 2;
    if ((type & Tile_Exit_N) == Tile_Exit_N) {
        for (int i = 0; i <= tile_size; i++) { tiles->set(x + ht, y - i,  1); }
    }

    if ((type & Tile_Exit_S) == Tile_Exit_S) {
        for (int i = 0; i < tile_size; i++) { tiles->set(x + ht, y + i,  1); }
    }

    if ((type & Tile_Exit_W) == Tile_Exit_W) {
        for (int i = 0; i <= tile_size; i++) { tiles->set(x - i, y + ht, 1); }
    }

    if ((type & Tile_Exit_E) == Tile_Exit_E) {
        for (int i = 0; i < tile_size; i++) { tiles->set(x + i, y + ht, 1); }
    }
}

static void tilemap_rec_map_gen(Tilemap* tiles, Tilemap* gen_tiles, i32 x, i32 y, u32 current, i32 depth, i32 max_depth, int tile_size) {
    if (depth == max_depth)               { return; }
    if (x < 0 || x > tiles->width - 1)    { return; }
    if (y < 0 || y > tiles->height - 1)   { return; }

    i32 x_div   = x / tile_size;
    i32 y_div   = y / tile_size;
    
    if (gen_tiles->get(x_div, y_div) != Tile_None)  { return; }

    tilemap_add_tile(tiles, x, y, current, tile_size);
    add_bit(gen_tiles, x_div, y_div, current);

    if (default_rnd.gen() % 2) {
        tilemap_add_tile(tiles, x, y, Tile_Exit_N, tile_size);

        add_bit(gen_tiles, x_div, y_div, Tile_Exit_N);

        tilemap_rec_map_gen(tiles, gen_tiles, x, y - tile_size, (Tile_Exit_S), depth + 1, max_depth, tile_size);
    }

    if (default_rnd.gen() % 2) {
        tilemap_add_tile(tiles, x, y, Tile_Exit_S, tile_size);

        add_bit(gen_tiles, x_div, y_div, Tile_Exit_S);

        tilemap_rec_map_gen(tiles, gen_tiles, x, y + tile_size, (Tile_Exit_N), depth + 1, max_depth, tile_size);
    }
    
    if (default_rnd.gen() % 2) {
        tilemap_add_tile(tiles, x, y, Tile_Exit_W, tile_size);

        add_bit(gen_tiles, x_div, y_div, Tile_Exit_W);

        tilemap_rec_map_gen(tiles, gen_tiles, x - tile_size, y, (Tile_Exit_E), depth + 1, max_depth, tile_size);
    }

    if (default_rnd.gen() % 2) {
        tilemap_add_tile(tiles, x, y, Tile_Exit_E, tile_size);

        add_bit(gen_tiles, x_div, y_div, Tile_Exit_E);

        tilemap_rec_map_gen(tiles, gen_tiles, x + tile_size, y, (Tile_Exit_W), depth + 1, max_depth, tile_size);
    }

    // make room
    if (rand() % 3 == 0) {
        add_bit(gen_tiles, x_div, y_div, Tile_Room);
        for (i32 ty = 1; ty < tile_size - 1; ty++) {
            for (i32 tx = 1; tx < tile_size - 1; tx++) {
                tiles->set(x + tx, y + ty, 1);
            }
        }
    }
}

static void tilemap_generate(Tilemap* tiles, int width, int height, int max_depth, int tile_size) {
    Tilemap gen_tiles = {0};
    tilemap_init(&gen_tiles, width / tile_size, height / tile_size);

    defer { tilemap_destroy(&gen_tiles); };

    tilemap_init(tiles, width, height);

    tilemap_rec_map_gen(
                tiles,
                &gen_tiles,
                tiles->width / 2,
                tiles->height / 2,
                Tile_Exit_N | Tile_Exit_S | Tile_Exit_W | Tile_Exit_E,
                0,
                max_depth,
                tile_size);

    // showing map in console for debugging!!
    for (i32 y = 0; y < gen_tiles.height; y++) {
        for (i32 x = 0; x < gen_tiles.width; x++) {
            u32 tile = gen_tiles.get(x, y);
            i32 p = 0;

            if  ((tile & Tile_Exit_N) == Tile_Exit_N) { p++; }
            if  ((tile & Tile_Exit_S) == Tile_Exit_S) { p++; }
            if  ((tile & Tile_Exit_W) == Tile_Exit_W) { p++; }
            if  ((tile & Tile_Exit_E) == Tile_Exit_E) { p++; }
        }
    }
}

static void tilemap_generate_noise(Tilemap* tiles, int width, int height, r32 scale_x, r32 scale_y, r32 tresh) {
    tilemap_init(tiles, width, height);

    for_ij(0, width, 0, height) {
        tiles->set(i, j, stb_perlin_noise3(scale_x * i, scale_y * j, 0, 0, 0, 0) < tresh);
    }
}

// =============================================== MAP COLLISION SYSTEM =================================== //

enum Collision_Type {
    Collision_None    = (0 << 0),
    Collision_Top     = (1 << 0),
    Collision_Bot     = (1 << 1),
    Collision_Left    = (1 << 2),
    Collision_Right   = (1 << 3),
};

#define COLLISION(var, location)    (((var) & (Collision_##location)) == (Collision_##location))
#define COLLISION_VER(var)          (COLLISION(var, Top)  || COLLISION(var, Bot))
#define COLLISION_HOR(var)          (COLLISION(var, Left) || COLLISION(var, Right))

static u32 tilemap_get_collision(const Tilemap* tiles, v2 pos, r32 r, float offset) {
    u32 col = 0;
    pos.x -= offset;
    i32 x   = floorf(pos.x);
    i32 y   = floorf(pos.y);

    if (x < 0)              { col |= Collision_Left; }
    if (y < 0)              { col |= Collision_Top; }
    if (x >= tiles->width)  { col |= Collision_Right; }
    if (y >= tiles->height) { col |= Collision_Bot; }

    if      (tiles->get(pos.x + r-0.2f, pos.y + r) || tiles->get(pos.x - r+0.2f, pos.y + r)) { col |= Collision_Top; }
    else if (tiles->get(pos.x + r-0.2f, pos.y - r) || tiles->get(pos.x - r+0.2f, pos.y - r)) { col |= Collision_Bot; }
    if      (tiles->get(pos.x - r, pos.y + r-0.2f) || tiles->get(pos.x - r, pos.y - r+0.2f)) { col |= Collision_Left; }
    else if (tiles->get(pos.x + r, pos.y + r-0.2f) || tiles->get(pos.x + r, pos.y - r+0.2f)) { col |= Collision_Right; }
    
    return col;
}

#endif

#endif

