#include <UIResourceMgr.h>
#include <UIFileHelper.h>
#include "CNodeDemoWnd.h"

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
                            u8"NodeDemoRes";
    UIPaintManager::SetQuitOnLastWindowDestroy(true);
    UIResourceMgr::GetInstance().SetResourcePath(resourcePath);

    CNodeDemoWnd wnd;
    wnd.Create(nullptr, UIString{u8"Node组件化示例"}, UI_WNDSTYLE_FRAME, 0, 0, 0, 600, 200);
    wnd.CenterWindow();
    wnd.ShowWindow();
    UIPaintManager::MessageLoop();
    return 0;
}