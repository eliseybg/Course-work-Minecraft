#ifndef VoxelRenderer_hpp
#define VoxelRenderer_hpp

#include <stdlib.h>

class Mesh;
class Chunk;

class VoxelRenderer {
    float* buffer;
    size_t capacity;
public:
    VoxelRenderer(size_t capacity);
    ~VoxelRenderer();

    Mesh* render(Chunk* chunk, const Chunk** chunks, bool ambientOcclusion);
};

#endif /* VoxelRenderer_hpp */
