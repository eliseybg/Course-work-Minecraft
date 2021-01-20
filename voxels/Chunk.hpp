#ifndef Chunk_hpp
#define Chunk_hpp

#define CHUNK_W 8
#define CHUNK_H 8
#define CHUNK_D 8
#define CHUNK_VOL (CHUNK_W * CHUNK_H * CHUNK_D)

class voxel;

class Chunk {
public:
    int x,y,z;
    voxel* voxels;
    bool modified = true;
    Chunk(int x, int y, int z);
    ~Chunk();
};

#endif /* Chunk_hpp */
