#include <UIResourceMgr.h>
#include <UIFileHelper.h>
#include "ExtensionDemoWnd.h"

#if defined(_MSC_VER) && defined(NDEBUG)
#pragma comment(linker,"/subsystem:windows")
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR cmdLine, int show) {
    int argc = __argc;
    char** argv = __argv;
#else
int main(int argc, char** argv) {
#endif
    UIResourceMgr::GetInstance().Init(argc, argv);
    UIString resourcePath = UIResourceMgr::GetInstance().GetCurrentPath() +
                            UIFileHelper::UI_PATH_SEPARATOR +
                            u8"ExtensionDemoRes";
    UIPaintManager::SetQuitOnLastWindowDestroy(true);
    UIResourceMgr::GetInstance().SetResourcePath(resourcePath);

    ExtensionDemoWnd wnd;
    wnd.Create(nullptr, UIString{u8"DuiLib扩展控件Demo"}, UI_WNDSTYLE_FRAME, 0, 0, 0, 800, 600);
    wnd.CenterWindow();
    wnd.ShowWindow();
    UIPaintManager::MessageLoop();
    return 0;
}