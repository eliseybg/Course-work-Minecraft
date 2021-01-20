#ifndef Texture_hpp
#define Texture_hpp
#include <string>

class Texture{
public:
    unsigned int id;
    int width;
    int height;
    Texture(unsigned int id, int width, int height);
    ~Texture();
    
    void bind();
};

Texture* load_texture(std::string filename);
#endif /* Texture_hpp */
