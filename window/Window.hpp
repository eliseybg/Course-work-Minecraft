#ifndef Window_hpp
#define Window_hpp

class GLFWwindow;

class Window {
public:
    static int width;
    static int height;
    static GLFWwindow* window;
    static int initialize(int width, int height, const char* title);
    static void terminate();

    static void setCursorMode(int mode);
    static bool isShouldClose();
    static void setShouldClose(bool flag);
    static void swapBuffers();
};

#endif /* Window_hpp */
