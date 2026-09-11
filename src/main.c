#include "../headers/app.h"

int user_main();

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    user_main();
}
#endif // _WIN32