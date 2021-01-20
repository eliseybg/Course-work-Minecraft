#ifndef Mesh_hpp
#define Mesh_hpp

#include <stdlib.h>

class Mesh {
    unsigned int vao;
    unsigned int vbo;
    size_t vertices;
    size_t vertexSize;
public:
    Mesh(const float* buffer, size_t vertices, const int* attrs);
    ~Mesh();

    void reload(const float* buffer, size_t vertices);
    void draw(unsigned int primitive);
};


#endif /* Mesh_hpp */
