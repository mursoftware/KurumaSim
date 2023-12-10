
#include "Pch.h"
#include "Main.h"
#include "GameManager.h"




#define APP_NAME "KurumaSimulator  [F1]DebugWindow  [F11]FullScreen"
#define CLASS_NAME "KurumaSimulator"




HINSTANCE   g_Instance{};
HWND        g_Window{};
bool        g_FullWindow{};
int         g_WindowWidth{};
int         g_WindowHeight{};

HWND GetWindow()
{
    return g_Window;
}


LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);





int APIENTRY wWinMain(  _In_ HINSTANCE hInstance,
                        _In_opt_ HINSTANCE hPrevInstance,
                        _In_ LPWSTR    lpCmdLine,
                        _In_ int       nCmdShow)
{

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);


    g_Instance = hInstance;
    g_FullWindow = false;


    //パラメータ読み込み
    int windowWidth{}, windowHeight{};
    {
        std::string path;
        path = ".\\Setting.ini";

        windowWidth = GetPrivateProfileInt("WINDOW", "WIDTH", 0, path.c_str());
        windowHeight = GetPrivateProfileInt("WINDOW", "HEIGHT", 0, path.c_str());

    }


    {
        WNDCLASSEX wcex{};
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = 0;
        wcex.lpfnWndProc = WndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = g_Instance;
        wcex.hIcon = nullptr;
        wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wcex.hbrBackground = nullptr;
        wcex.lpszMenuName = nullptr;
        wcex.lpszClassName = CLASS_NAME;
        wcex.hIconSm = nullptr;

        RegisterClassEx(&wcex);


        RECT rc = { 0, 0, (LONG)windowWidth, (LONG)windowHeight };
        AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

        g_WindowWidth = rc.right - rc.left;
        g_WindowHeight = rc.bottom - rc.top;

        g_Window = CreateWindow(CLASS_NAME, APP_NAME, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
            rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, g_Instance, nullptr);
    }





    {


        GameManager gameManager;


        ShowWindow(g_Window, SW_SHOW);



        //フレームカウント初期化
        //DWORD dwExecLastTime{};
        //DWORD dwCurrentTime{};
        //timeBeginPeriod(1);
        //dwExecLastTime = timeGetTime();
        //dwCurrentTime = 0;

        LARGE_INTEGER qpf;
        QueryPerformanceFrequency(&qpf);

        LARGE_INTEGER qpc;
        QueryPerformanceCounter(&qpc);
        LONGLONG lastTime = qpc.QuadPart;


        while (true)
        {

            MSG msg{};

            if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
            {
                if (msg.message == WM_QUIT)
                {
                    break;
                }
                else
                {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
            }
            else
            {
                //dwCurrentTime = timeGetTime();

                //if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60 / 5))
                //{
                //    gameManager.Update();

                //    dwExecLastTime = dwCurrentTime;
                //}
                //else
                //{
                //    //Sleep(1);
                //}



                QueryPerformanceCounter(&qpc);
                LONGLONG time = qpc.QuadPart;

                float drawTime = (float)(time - lastTime) / qpf.QuadPart;

                if (drawTime >= 1.0f / 60.0f / 10.0f)
                {
                    gameManager.Update();

                    lastTime = time;
                }

            }
        }
    }



    return 0;
}






void ChangeFullWindow()
{
    g_FullWindow = !g_FullWindow;

    if (g_FullWindow)
    {
        SetWindowLong(g_Window, GWL_STYLE, WS_POPUP);
        ShowWindow(g_Window, SW_MAXIMIZE);
    }
    else
    {
        SetWindowLong(g_Window, GWL_STYLE, WS_OVERLAPPEDWINDOW);

        SetWindowPos(g_Window, NULL, 0, 0, g_WindowWidth, g_WindowHeight, (SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED));
        ShowWindow(g_Window, SW_NORMAL);
    }

    //SetWindowPos(g_Window, NULL, 0, 0, 0, 0, (SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED));
}



LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
        return true;

    switch (message)
    {

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_ESCAPE:
            DestroyWindow(hWnd);
            break;
        case VK_F11:
            ChangeFullWindow();
            break;
        }
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);

    }

    return 0;
}
