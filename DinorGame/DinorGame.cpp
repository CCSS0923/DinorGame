/*
조작법 :
        스페이스바 : 점프
        아래 방향키 : 빠르게 낙하 (5배속)
*/

#include "framework.h"
#include "DinorGame.h"
#include <cstdlib>
#include <ctime>
#include <windows.h>
#include <thread>
#include <atomic>
#include <vector>
#include <algorithm> // sort 함수 위해 필요

using std::atomic; // std:: 생략 가능

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

// 선인장 도트 그래픽 (8x8)
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

// 익룡 도트 그래픽 (21x13)
const int pteroWidth = 21;
const int pteroHeight = 13;
const int pteroPixels[pteroHeight][pteroWidth] = {
    {0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,1,1,0,1,1,1,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,1,1,1,0,0,1,1,1,0,0,0,0,0,0,0,0,0},
    {0,0,0,1,1,1,1,1,0,1,1,1,1,0,0,0,0,0,0,0,0},
    {0,0,1,1,0,1,1,1,0,1,1,1,1,1,0,0,0,0,0,0,0},
    {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0},
    {0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0}
};

const int pixelSize = 5; // 픽셀 크기 절반 축소

atomic<int> dinoRealX(50);
atomic<int> dinoScreenX(50);
atomic<int> cameraXOffset(0);

int dx = 15;

atomic<int> dinoPosY(310);
atomic<bool> isJumping(false);
atomic<float> jumpVelocity(0.0f);

const float gravity = 3.0f;
const int groundY = 310;

atomic<bool> downKeyPressed(false);

struct Cactus {
    int x;
    bool active;
};

struct Ptero {
    int x;
    bool active;
    int assignedInterval; // 익룡이 할당된 선인장 간격 인덱스
};

const int MAX_CACTUS = 30;
const int MAX_PTERO = 10;

Cactus cactusPool[MAX_CACTUS];
atomic<int> activeCactusCount(0);

Ptero pteroPool[MAX_PTERO];
atomic<int> activePteroCount(0);

atomic<bool> running(true);

inline int GetRandomDistance(int min, int max) {
    return min + rand() % (max - min + 1);
}

inline int GetInactiveCactusIndex() {
    for (int i = 0; i < MAX_CACTUS; i++)
        if (!cactusPool[i].active)
            return i;
    return -1;
}

inline int GetInactivePteroIndex() {
    for (int i = 0; i < MAX_PTERO; i++)
        if (!pteroPool[i].active)
            return i;
    return -1;
}

void cactusPoolSortX() {
    vector<Cactus> tempVec;
    for (int i = 0; i < MAX_CACTUS; i++) {
        if (cactusPool[i].active)
            tempVec.push_back(cactusPool[i]);
    }
    sort(tempVec.begin(), tempVec.end(), [](const Cactus& a, const Cactus& b) {
        return a.x < b.x;
        });
    int idx = 0;
    for (int i = 0; i < MAX_CACTUS; i++) {
        if (cactusPool[i].active) {
            cactusPool[i] = tempVec[idx++];
            if (idx >= (int)tempVec.size())
                break;
        }
    }
}

bool isIntervalAssigned(vector<int>& intervals, int interval) {
    for (int i : intervals) {
        if (i == interval) return true;
    }
    return false;
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
            currentX += GetRandomDistance(600, 900);
        }
    }
}

void InitPteros(int screenWidth) {
    for (int i = 0; i < MAX_PTERO; i++) {
        pteroPool[i].active = false;
        pteroPool[i].x = 0;
        pteroPool[i].assignedInterval = -1;
    }
    activePteroCount = 5;

    cactusPoolSortX();

    vector<int> assignedIntervals;
    int lastPteroX = screenWidth + pteroWidth * pixelSize + 1500;

    for (int i = 0; i < activePteroCount; i++) {
        int idx = GetInactivePteroIndex();
        if (idx < 0) break;

        if (activeCactusCount < 2) {
            pteroPool[idx].x = lastPteroX + GetRandomDistance(1500, 1800);
            pteroPool[idx].assignedInterval = -1;
            pteroPool[idx].active = true;
            lastPteroX = pteroPool[idx].x;
            assignedIntervals.push_back(-1);
            continue;
        }

        bool placed = false;
        for (int interval = 0; interval < activeCactusCount - 1; interval++) {
            if (!isIntervalAssigned(assignedIntervals, interval)) {
                int leftX = cactusPool[interval].x;
                int rightX = cactusPool[interval + 1].x;
                int gap = rightX - leftX;
                int maxGap = 900;

                if (gap >= 0.8 * maxGap) {
                    int centerX = (leftX + rightX) / 2;
                    int offset = GetRandomDistance(-50, 50);
                    pteroPool[idx].x = centerX + offset;
                    pteroPool[idx].assignedInterval = interval;
                    pteroPool[idx].active = true;
                    assignedIntervals.push_back(interval);
                    placed = true;
                    lastPteroX = pteroPool[idx].x;
                    break;
                }
            }
        }
        if (!placed) {
            pteroPool[idx].x = lastPteroX + GetRandomDistance(1500, 1800);
            pteroPool[idx].assignedInterval = -1;
            pteroPool[idx].active = true;
            lastPteroX = pteroPool[idx].x;
            assignedIntervals.push_back(-1);
        }
    }
}

void DrawDino(HDC hdc, int posX, int posY, int pixelSize) {
    for (int y = 0; y < dinoHeight; y++)
        for (int x = 0; x < dinoWidth; x++)
            if (dinoPixels[y][x] == 1)
                Rectangle(hdc, posX + x * pixelSize, posY + y * pixelSize, posX + (x + 1) * pixelSize, posY + (y + 1) * pixelSize);
}

void DrawCactus(HDC hdc, int posX, int posY, int pixelSize) {
    for (int y = 0; y < cactusHeight; y++)
        for (int x = 0; x < cactusWidth; x++)
            if (cactusPixels[y][x] == 1)
                Rectangle(hdc, posX + x * pixelSize, posY + y * pixelSize, posX + (x + 1) * pixelSize, posY + (y + 1) * pixelSize);
}

void DrawPtero(HDC hdc, int posX, int posY, int pixelSize) {
    for (int y = 0; y < pteroHeight; y++)
        for (int x = 0; x < pteroWidth; x++)
            if (pteroPixels[y][x] == 1)
                Rectangle(hdc, posX + x * pixelSize, posY + y * pixelSize, posX + (x + 1) * pixelSize, posY + (y + 1) * pixelSize);
}

void CactusAndPteroProcessingThread(HWND hwnd) {
    int lastCactusX;
    int lastPteroX;
    RECT rect;

    while (running) {
        GetClientRect(hwnd, &rect);

        lastCactusX = rect.right + cactusWidth * pixelSize + 600;
        lastPteroX = rect.right + pteroWidth * pixelSize + 1500;

        for (int i = 0; i < MAX_CACTUS; i++) {
            if (cactusPool[i].active) {
                cactusPool[i].x -= dx;

                if (cactusPool[i].x + cactusWidth * pixelSize < 0) {
                    cactusPool[i].x = lastCactusX + cactusWidth * pixelSize + 600 + GetRandomDistance(600, 900);
                }
                if (cactusPool[i].x > lastCactusX)
                    lastCactusX = cactusPool[i].x;
            }
        }

        cactusPoolSortX();

        vector<int> assignedIntervals;
        for (int j = 0; j < MAX_PTERO; j++) {
            if (pteroPool[j].active && pteroPool[j].assignedInterval >= 0)
                assignedIntervals.push_back(pteroPool[j].assignedInterval);
        }

        for (int i = 0; i < MAX_PTERO; i++) {
            if (pteroPool[i].active) {
                pteroPool[i].x -= dx;

                if (pteroPool[i].x + pteroWidth * pixelSize < 0) {
                    if (activeCactusCount < 2) {
                        pteroPool[i].x = lastPteroX + pteroWidth * pixelSize + 1500 + GetRandomDistance(600, 900);
                        pteroPool[i].assignedInterval = -1;
                    }
                    else {
                        int intervalFound = -1;
                        for (int interval = 0; interval < activeCactusCount - 1; interval++) {
                            if (find(assignedIntervals.begin(), assignedIntervals.end(), interval) == assignedIntervals.end()) {
                                intervalFound = interval;
                                break;
                            }
                        }
                        if (intervalFound >= 0) {
                            int leftX = cactusPool[intervalFound].x;
                            int rightX = cactusPool[intervalFound + 1].x;
                            int gap = rightX - leftX;
                            int maxGap = 900;
                            if (gap >= 0.8 * maxGap) {
                                int centerX = (leftX + rightX) / 2;
                                int offset = GetRandomDistance(-50, 50);
                                pteroPool[i].x = centerX + offset;
                                pteroPool[i].assignedInterval = intervalFound;
                            }
                            else {
                                pteroPool[i].x = rightX + 3 * 600 + GetRandomDistance(0, 300);
                                pteroPool[i].assignedInterval = -1;
                            }
                        }
                        else {
                            pteroPool[i].x = lastPteroX + pteroWidth * pixelSize + 1500 + GetRandomDistance(600, 900);
                            pteroPool[i].assignedInterval = -1;
                        }
                    }
                }
                if (pteroPool[i].x > lastPteroX)
                    lastPteroX = pteroPool[i].x;
            }
        }
        InvalidateRect(hwnd, nullptr, TRUE);
        this_thread::sleep_for(chrono::milliseconds(TIMER_INTERVAL));
    }
}

ATOM MyRegisterClass(HINSTANCE);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int nCmdShow) {
    UNREFERENCED_PARAMETER(nCmdShow);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_DINORGAME, szWindowClass, MAX_LOADSTRING);

    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow))
        return FALSE;

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
    InitPteros(rect.right);

    thread movingThread(CactusAndPteroProcessingThread, hwnd);
    movingThread.detach();

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
    }break;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT rect;
        GetClientRect(hwnd, &rect);

        const int floorY = 420;

        HBRUSH brush = CreateSolidBrush(RGB(230, 230, 230));
        RECT marginRect = { 0,floorY + 1,rect.right,rect.bottom };
        FillRect(hdc, &marginRect, brush);
        DeleteObject(brush);

        MoveToEx(hdc, 0, floorY, nullptr);
        LineTo(hdc, rect.right, floorY);

        for (int i = 0; i < MAX_CACTUS; i++) {
            if (cactusPool[i].active)
                DrawCactus(hdc, cactusPool[i].x - cameraXOffset.load(), floorY - pixelSize * cactusHeight, pixelSize);
        }

        int fixedPteroY = groundY - pixelSize * pteroHeight - 80;

        for (int i = 0; i < MAX_PTERO; i++) {
            if (pteroPool[i].active)
                DrawPtero(hdc, pteroPool[i].x - cameraXOffset.load(), fixedPteroY, pixelSize);
        }

        int centerX = rect.right / 2;
        int fixedDinoX = centerX - 100;

        if (dinoRealX.load() < fixedDinoX) {
            int current = dinoRealX.load();
            int step = dx * 3;
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
            bool downPressed = downKeyPressed.load();
            int fallStep = static_cast<int>(oldVel);
            if (downPressed && fallStep > 0) fallStep *= 5;
            dinoPosY.store(oldPos + fallStep);
            jumpVelocity.store(oldVel + gravity);
            if (dinoPosY.load() >= groundY) {
                dinoPosY.store(groundY);
                isJumping.store(false);
                jumpVelocity.store(0.0f);
                downKeyPressed.store(false);
            }
        }

        DrawDino(hdc, dinoScreenX.load(), dinoPosY.load(), pixelSize);

        EndPaint(hwnd, &ps);
    }break;

    case WM_KEYDOWN:
        if (wParam == VK_SPACE && !isJumping.load()) {
            isJumping.store(true);
            jumpVelocity.store(-35.0f);
        }
        else if (wParam == VK_DOWN) {
            if (!downKeyPressed.load())
                downKeyPressed.store(true);
        }
        break;

    case WM_KEYUP:
        if (wParam == VK_DOWN)
            downKeyPressed.store(false);
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
