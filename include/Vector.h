#ifndef VECTOR
#define VECTOR

typedef struct {
    double x;
    double y;
} Vector2D;

void AddVector(Vector2D *to, Vector2D *from);

// Dot, Cross, Nomalize will be added if required.

#endif