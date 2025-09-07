/*
조작법 : 
        스페이스바 : 점프
        아래 방향키 : 빠르게 낙하
*/

#include "framework.h"
#include "DinorGame.h"
#include <cstdlib>
#include <ctime>
#include <windows.h>
#include <thread>
#include <atomic>

#define MAX_LOADSTRING 100
#define TIMER_INTERVAL 50 // 20fps

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

// 공룡 도트 그래픽 (20x22)
const int dinoWidth = 20;
const int dinoHeight = 22;
const int dinoPixels[dinoHeight][dinoWidth] = {
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

const int cactusWidth = 8;
const int cactusHeight = 8;
const int cactusPixels[cactusHeight][cactusWidth] = {
    {0,1,0,0,0,1,0,0},
    {0,1,0,0,0,1,0,0},
    {1,1,1,0,1,1,1,0},
    {1,1,1,1,1,1,1,0},
    {0,1,1,0,1,1,0,0},
    {0,1,0,0,0,1,0,0},
    {0,1,0,0,0,1,0,0},
    {0,1,0,0,0,1,0,0},
};

const int pixelSize = 10;

static std::atomic<int> dinoRealX(50);
static std::atomic<int> dinoScreenX(50);
static std::atomic<int> cameraXOffset(0);

static int dx = 15; // 이동속도

static std::atomic<int> dinoPosY(200);
static std::atomic<bool> isJumping(false);
static std::atomic<float> jumpVelocity(0.0f);

const float gravity = 3.0f;
const int groundY = 200;

// 아래키 눌림 상태
static std::atomic<bool> downKeyPressed(false);

struct Cactus {
    int x;
    bool active;
};

const int MAX_CACTUS = 30;
static Cactus cactusPool[MAX_CACTUS];
static std::atomic<int> activeCactusCount(0);
static std::atomic<bool> running(true);

inline int GetRandomDistance() {
    return 600 + rand() % 301; // 넉넉한 랜덤 간격
}

inline int GetInactiveCactusIndex() {
    for (int i = 0; i < MAX_CACTUS; i++)
        if (!cactusPool[i].active)
            return i;
    return -1;
}

void InitCactuses(int screenWidth) {
    for (int i = 0; i < MAX_CACTUS; i++) {
        cactusPool[i].active = false;
        cactusPool[i].x = 0;
    }
    int currentX = screenWidth + cactusWidth * pixelSize + 300;
    activeCactusCount = 10;
    for (int i = 0; i < activeCactusCount; i++) {
        int idx = GetInactiveCactusIndex();
        if (idx >= 0) {
            cactusPool[idx].active = true;
            cactusPool[idx].x = currentX;
            currentX += GetRandomDistance();
        }
    }
}

void DrawDino(HDC hdc, int posX, int posY, int pixelSize) {
    for (int y = 0; y < dinoHeight; y++) {
        for (int x = 0; x < dinoWidth; x++) {
            if (dinoPixels[y][x] == 1)
                Rectangle(hdc, posX + x * pixelSize, posY + y * pixelSize, posX + (x + 1) * pixelSize, posY + (y + 1) * pixelSize);
        }
    }
}

void DrawCactus(HDC hdc, int posX, int posY, int pixelSize) {
    for (int y = 0; y < cactusHeight; y++) {
        for (int x = 0; x < cactusWidth; x++) {
            if (cactusPixels[y][x] == 1)
                Rectangle(hdc, posX + x * pixelSize, posY + y * pixelSize, posX + (x + 1) * pixelSize, posY + (y + 1) * pixelSize);
        }
    }
}

void CactusProcessingThread(HWND hwnd) {
    int lastX;
    while (running) {
        RECT rect;
        GetClientRect(hwnd, &rect);

        lastX = rect.right + cactusWidth * pixelSize + 600;

        for (int i = 0; i < MAX_CACTUS; i++) {
            if (cactusPool[i].active) {
                cactusPool[i].x -= dx;

                if (cactusPool[i].x + cactusWidth * pixelSize < 0) {
                    cactusPool[i].x = lastX + cactusWidth * pixelSize + 600 + GetRandomDistance();
                }

                if (cactusPool[i].x > lastX) {
                    lastX = cactusPool[i].x;
                }
            }
        }

        InvalidateRect(hwnd, nullptr, TRUE);

        std::this_thread::sleep_for(std::chrono::milliseconds(TIMER_INTERVAL));
    }
}

ATOM MyRegisterClass(HINSTANCE);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int nCmdShow) {
    UNREFERENCED_PARAMETER(nCmdShow);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_DINORGAME, szWindowClass, MAX_LOADSTRING);

    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow)) return FALSE;

    SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance) {
    WNDCLASSEXW wcex = { sizeof(WNDCLASSEX) };
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0; wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DINORGAME));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_DINORGAME);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
    hInst = hInstance;
    HWND hwnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, 640, 580, nullptr, nullptr, hInstance, nullptr);
    if (!hwnd) return FALSE;
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    RECT rect;
    GetClientRect(hwnd, &rect);
    srand((unsigned int)time(NULL));

    InitCactuses(rect.right);

    std::thread cactusThread(CactusProcessingThread, hwnd);
    cactusThread.detach();

    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_COMMAND: {
        int wmId = LOWORD(wParam);
        switch (wmId) {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hwnd, About);
            break;
        case IDM_EXIT:
            running.store(false);
            DestroyWindow(hwnd);
            break;
        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
        }
    } break;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT rect;
        GetClientRect(hwnd, &rect);

        const int floorY = 420;

        HBRUSH brush = CreateSolidBrush(RGB(230, 230, 230));
        RECT marginRect = { 0, floorY + 1, rect.right, rect.bottom };
        FillRect(hdc, &marginRect, brush);
        DeleteObject(brush);

        MoveToEx(hdc, 0, floorY, nullptr);
        LineTo(hdc, rect.right, floorY);

        for (int i = 0; i < MAX_CACTUS; i++) {
            if (cactusPool[i].active) {
                DrawCactus(hdc, cactusPool[i].x - cameraXOffset.load(), floorY - pixelSize * cactusHeight, pixelSize);
            }
        }

        int centerX = rect.right / 2;
        int fixedDinoX = centerX - 100;

        if (dinoRealX.load() < fixedDinoX) {
            int current = dinoRealX.load();
            int step = dx * 3; // 3배 빠른 중앙 이동
            int nextPos = current + step;
            if (nextPos > fixedDinoX) nextPos = fixedDinoX;
            dinoRealX.store(nextPos);
            dinoScreenX.store(nextPos);
            cameraXOffset.store(0);
        }
        else {
            dinoScreenX.store(fixedDinoX);
            cameraXOffset.store(dinoRealX.load() - fixedDinoX);
        }

        if (isJumping.load()) {
            int oldPos = dinoPosY.load();
            float oldVel = jumpVelocity.load();

            // 아래키 누름에 따른 가속 관리
            bool downPressed = downKeyPressed.load();

            int fallStep = static_cast<int>(oldVel);
            // 아래방향키 눌렀으면 기본 낙하 속도의 5배로 이동
            if (downPressed && fallStep > 0) {
                fallStep *= 5;
            }

            dinoPosY.store(oldPos + fallStep);
            // 가속도 적용
            jumpVelocity.store(oldVel + gravity);

            if (dinoPosY.load() >= groundY) {
                dinoPosY.store(groundY);
                isJumping.store(false);
                jumpVelocity.store(0.0f);
                downKeyPressed.store(false); // 착지 후 아래키 상태 초기화
            }
        }

        DrawDino(hdc, dinoScreenX.load(), dinoPosY.load(), pixelSize);
        EndPaint(hwnd, &ps);
        break;
    }
    case WM_KEYDOWN:
        if (wParam == VK_SPACE && !isJumping.load()) {
            isJumping.store(true);
            jumpVelocity.store(-35.0f);
        }
        else if (wParam == VK_DOWN) {
            if (!downKeyPressed.load()) {
                downKeyPressed.store(true);
            }
        }
        break;
    case WM_KEYUP:
        if (wParam == VK_DOWN) {
            downKeyPressed.store(false);
        }
        break;
    case WM_DESTROY:
        running.store(false);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
