#include <cstdint>

#define FACE_TOP    0b00000001
#define FACE_BOTTOM 0b00000010
#define FACE_NORTH  0b00000100
#define FACE_SOUTH  0b00001000
#define FACE_EAST   0b00010000
#define FACE_WEST   0b00100000

// Math
struct Int3 {
    int x,y,z;
};

typedef struct Int3 Int3;

struct Float3 {
    float x,y,z;
};

typedef struct Float3 Float3;

struct Vector3 {
    double x,y,z;
};

typedef struct Vector3 Vector3;

// Primitives

struct Cube {
    Int3 cornerA, cornerB;
    std::string textureName;
    float textureScale;
    Int3 lightMapScale;
};

typedef struct Cube Cube;

struct PointLight {
    Int3 pos;
    float falloff = 0.01;
};

typedef struct PointLight PointLight;