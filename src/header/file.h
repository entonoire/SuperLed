namespace SAVE {
    void init();
    void sendLastState();
    bool exist();
    File openRead();
    const char* saveColors(const String& newColor);
    void applyLastScene();
    // extern bool busy;
}