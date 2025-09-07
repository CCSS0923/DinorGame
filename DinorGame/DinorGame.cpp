#include "framework.h"
#include "DinorGame.h"

#define MAX_LOADSTRING 100
#define TIMER_ID 1
#define TIMER_INTERVAL 50 // 50ms 간격 (약 20fps)

HINSTANCE hInst;                                // 현재 인스턴스
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름

// 공룡 도트 그래픽 정의 (1: 픽셀 있음, 0: 없음)
const int dinoWidth = 20;
const int dinoHeight = 22;
const int dinoPixels[dinoHeight][dinoWidth] =
{
    {0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,0},
    {0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1},
    {0,0,0,0,0,0,0,0,0,0,1,1,0,1,1,1,1,1,1,1},
    {0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1},
    {0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1},
    {0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1},
    {0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0},
    {1,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0},
    {1,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0},
    {1,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0},
    {1,1,1,0,0,1,1,1,1,1,1,1,1,1,0,1,0,0,0,0},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0},
    {0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0},
    {0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0},
    {0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0},
    {0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,1,1,1,0,1,1,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,1,1,0,0,0,1,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,0,0,0,0},
};



int dinoPosX = 50;
int dinoPosY = 100;

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_DINORGAME, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    MSG msg;
    // 타이머 설정: 윈도우 생성 후 타이머로 주기적 WM_TIMER 발생
    SetTimer(NULL, TIMER_ID, TIMER_INTERVAL, NULL);

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DINORGAME));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_DINORGAME);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, 640, 480, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // 타이머 설정: 창 핸들을 타이머에 연결
    SetTimer(hWnd, TIMER_ID, TIMER_INTERVAL, NULL);

    return TRUE;
}

void DrawDino(HDC hdc, int posX, int posY, int pixelSize)
{
    for (int y = 0; y < dinoHeight; ++y)
    {
        for (int x = 0; x < dinoWidth; ++x)
        {
            if (dinoPixels[y][x] == 1)
            {
                Rectangle(hdc,
                    posX + x * pixelSize,
                    posY + y * pixelSize,
                    posX + (x + 1) * pixelSize,
                    posY + (y + 1) * pixelSize);
            }
        }
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static int dx = 5; // 공룡의 x방향 이동 속도

    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        // 공룡 도트 그래픽 그리기
        DrawDino(hdc, dinoPosX, dinoPosY, 10);

        EndPaint(hWnd, &ps);
    }
    break;

    case WM_TIMER:
        if (wParam == TIMER_ID)
        {
            // 공룡 좌표 이동 (간단한 애니메이션)
            dinoPosX += dx;

            // 윈도우 클라이언트 영역 크기 얻기
            RECT rect;
            GetClientRect(hWnd, &rect);

            // 화면 밖으로 나가지 않도록 방향 반전
            if (dinoPosX + dinoWidth * 10 > rect.right || dinoPosX < 0)
            {
                dx = -dx;
            }

            InvalidateRect(hWnd, NULL, TRUE); // 화면 갱신 요청 (WM_PAINT 호출 유발)
        }
        break;

    case WM_DESTROY:
        KillTimer(hWnd, TIMER_ID);
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
