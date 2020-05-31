// 坦克.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "坦克.h"
#include <time.h>

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

enum Dir { UP, DOWN, LEFT, RIGHT };
typedef struct {
    int x, y;
    Dir dir;
    int v;
    int s;
    int b;
    int p;
    int e;
    COLORREF c;
}Entity;
#define MAX_ENEMY 16
#define MAX_BULLETS 32
int nlife = 3;
int nScore = 0;
int nBullet = 0;
int nEnemyBullet = 0;
int nEnemy = 0;
int timeStep = 20;
int sz = 50;
int velf = 4;
int vels = 2;
int szb = 20;
int velb = 6;
int enemyFirePer = 300;
int enemyDir = 200;
int bFire = 0;
Entity enemys[MAX_ENEMY];
Entity player;
Entity bullets[MAX_BULLETS];
Entity enemyBullets[MAX_BULLETS];
int wndWidth = 0; int wndHeight = 0;
void ResetPlayer()
{
    player.s = sz;
    player.b = 0;
    player.c = RGB(122, 30, 0);
    player.dir = UP;
    player.v = vels;
    player.x = wndWidth / 2;
    player.y = wndHeight - sz;
    player.p = 1;
    player.e = 0;
}
void Init()
{
    for (nEnemy = 0; nEnemy < MAX_ENEMY; nEnemy++)
    {
        enemys[nEnemy].s = sz;
        enemys[nEnemy].b = 0;
        enemys[nEnemy].e = 1;
        enemys[nEnemy].dir = Dir(UP + rand() % 4);
        enemys[nEnemy].v = rand() % 2 == 0 ? velf : vels;
        enemys[nEnemy].c = enemys[nEnemy].v == velf ? RGB(0, 122, 122) : RGB(0, 60, 30);
        enemys[nEnemy].x = (rand() % 3) * (wndWidth - sz) / 2 + sz / 2;
        enemys[nEnemy].y = sz;
        enemys[nEnemy].p = 0;
    }
    ResetPlayer();
}
void Move(Entity* ent, int ts)
{
    if (ent->p)
        return;
    switch (ent->dir)
    {
    case UP:
        ent->y -= ent->v * ts;
        break;
    case DOWN:
        ent->y += ent->v * ts;
        break;
    case LEFT:
        ent->x -= ent->v * ts;
        break;
    case RIGHT:
        ent->x += ent->v * ts;
        break;
    }
}
void Fire(const Entity* ent)
{
    Entity* pBullets = (ent->e) ? enemyBullets : bullets;
    int nB = (ent->e) ? nEnemyBullet : nBullet;
    if (nB >= MAX_BULLETS)
        return;
    (pBullets + nB)->s = szb;
    (pBullets + nB)->b = 1;
    (pBullets + nB)->e = 0;
    (pBullets + nB)->c = (ent->e) ? RGB(0, 0, 255) : RGB(255, 0, 0);
    (pBullets + nB)->dir = ent ->dir;
    (pBullets + nB)->v = velb;
    (pBullets + nB)->x = ent ->x;
    (pBullets + nB)->p = 0;
    (pBullets + nB)->y = ent->y;
    switch (ent->dir)
    {
    case UP:
        (pBullets + nB)->y -= ent->s;
        break;
    case DOWN:
        (pBullets + nB)->y += ent ->s;
        break;
    case LEFT:
        (pBullets + nB)->x -= ent->s;
        break;
    case RIGHT:
        (pBullets + nB)->x += ent->s;
        break;
    }
    if (ent->e)
        nEnemyBullet++;
    else
        nBullet++;
}
void Destroy(Entity ents[], int n, int* num)
{
    memcpy(ents + n, ents + n - 1, sizeof(Entity) * ((*num) - 1 - n));
    (*num)--;
}
int IsCollider(const Entity* ent1, const Entity* ent2)
{
    if (ent1->x + ent1->s / 2 <= ent2->x - ent2->s / 2
        || ent1->x - ent1->s / 2 >= ent2->x + ent2->s / 2 / 2)
        return 0;
    if (ent1->y + ent1->s / 2 <= ent2->y - ent2->s / 2
        || ent1->y - ent1->s / 2 >= ent2->y + ent2->s / 2 / 2)
        return 0;
    return 1;
}
int WallCollider(Entity* ent)
{
    int bC = 0;
    switch (ent->dir)
    {
    case UP:
        if ((ent->y - ent->s / 2) < 0) {
            bC = 1;
            ent->y - ent->s / 2;
        }
        break;
    case DOWN:
        if ((ent->y + ent->s / 2) > wndHeight) {
            bC = 1;
            ent->y = wndHeight - ent->s / 2;
        }
        break;
    case LEFT:
        if ((ent->x - ent->s / 2) < 0) {
            bC = 1;
            ent->x - ent->s / 2;
        }
        break;
    case RIGHT:
        if ((ent->x + ent->s / 2) > wndHeight) {
            bC = 1;
            ent->x = wndHeight - ent->s / 2;
        }
        break;
    }
    if (bC)
    {
        if (ent->e)
            ent->dir = Dir((ent->dir + 1 + rand() % 3) % 4);
        else
            ent->p = 1;
    }
    return bC;
}
void Update(int ts)
{
    Entity* ent = NULL;
    for (int i = 0; i < nEnemy; i++) {
        ent = enemys + i;
        Move(ent, ts);
        if ((rand() % enemyFirePer) == 0)
            Fire(ent);
    }
    for (int i = 0; i < nBullet; i++) {
        ent = bullets + i;
        Move(ent, ts);
    }
    for (int i = 0; i < nEnemyBullet; i++) {
        ent = enemyBullets + i;
        Move(ent, ts);
    }
    Move(&player, ts);
    if (bFire)
    {
        Fire(&player);
        bFire = 0;
    }
    for (int i = 0; i < nBullet; i++) {
        for (int j = 0; j < nEnemy; j++) {
            if (IsCollider(&bullets[i], &enemys[j])) {
                Destroy(bullets, i, &nBullet);
                Destroy(enemys, j, &nEnemy);
                nScore++;
                i--;
                j--;
                break;
            }
        }
    }
    for(int i=0;i<nEnemyBullet;i++){
        if (IsCollider(&enemyBullets[i], &player)) {
            Destroy(enemyBullets,i, &nEnemyBullet);
            ResetPlayer();
            nlife--;
            i--;
            break;
        }
    }
    for (int i = 0; i < nEnemy; i++) {
        if (IsCollider(&player, &enemys[i])) {
            ResetPlayer();
            nlife--;
        }
    }
    for (int i = 0; i < nEnemy; i++) {
        ent = enemys + i;
        if (!WallCollider(ent)) {
            if (rand() % enemyDir == 0)
                ent->dir = Dir((ent->dir + 1 + rand() % 3) % 4);
        }
    }
    for (int i = 0; 1 < nBullet; i++) {
        ent = bullets + i;
        if (WallCollider(ent)) {
            Destroy(bullets, i, &nBullet);
            i--;
        }
    }
    for (int i = 0; i < nEnemyBullet; i++) {
        ent = enemyBullets + i;
        if (WallCollider(ent)) {
            Destroy(enemyBullets, i, &nEnemyBullet);
            i--;
        }
    }
    WallCollider(&player);
}
void DrawEntity(HDC hdc, const Entity* ent)
{
    HBRUSH brush;
    brush = CreateSolidBrush(ent->c);
    RECT rc;
    rc.top = ent->y - ent->s / 2;
    rc.left = ent->x - ent->s / 2;
    rc.bottom = ent->y + ent->s / 2;
    rc.right = ent->x + ent->s / 2;
    FillRect(hdc, &rc, brush);
    if (!ent->b) {
        switch (ent->dir) {
        case UP:
            rc.bottom = rc.top;
            rc.top = rc.bottom - ent->s / 2;
            rc.left = rc.right + ent->s / 4;
            rc.right = rc.right - ent->s / 4;
            break;
        case DOWN:
            rc.top = rc.bottom;
            rc.bottom = rc.bottom + ent->s / 2;
            rc.left = rc.left + ent->s / 4;
            rc.right = rc.right - ent->s / 4;
            break;
        case LEFT:
            rc.right = rc.left;
            rc.left = rc.left - ent->s / 2;
            rc.bottom = rc.bottom - ent->s / 4;
            rc.top = rc.top + ent->s / 4;
            break;
        case RIGHT:
            rc.left = rc.right;
            rc.right = rc.right + ent->s / 2;
            rc.bottom = rc.bottom, -ent->s / 4;
            rc.top = rc.top + ent->s / 4;
            break;
        }
        FillRect(hdc, &rc,brush);
    }
    DeleteObject(brush);
}
void DrawScene(HDC hdc)
{
    HFONT hf;
    WCHAR str[32];
    long lfHeight;
    lfHeight = -MulDiv(16, GetDeviceCaps(hdc, LOGPIXELSY), 72);
    hf = CreateFont(lfHeight, 0, 0, 0, 0, TRUE, 0, 0, 0, 0, 0, 0, 0, L"Times New Roman");
    HFONT hfOld = (HFONT)SelectObject(hdc, hf);
    if (nlife <= 0) {
        SetTextColor(hdc, RGB(122, 0, 0));
        TextOut(hdc, wndWidth / 2 - 100, wndHeight / 2 - 40, L"Game Over", 9);
        SelectObject(hdc, hfOld);
        return;
    }
    SetTextColor(hdc, RGB(100, 100, 100));
    wsprintf(str, L"Life:%d Score:%d", nlife, nScore);
    TextOut(hdc, wndWidth / 2 - 100, wndHeight / 2 - 40, str, wcslen(str));
    SelectObject(hdc, hfOld);
    DeleteObject(hf);
    Entity* ent = NULL;
    for (int i = 0; i < nEnemy; i++) {
        ent = enemys + i;
        DrawEntity(hdc, ent);
    }
    for (int i = 0; i < nBullet; i++) {
        ent = bullets + i;
        DrawEntity(hdc, ent);
    }
    for (int i = 0; i < nEnemyBullet; i++) {
        ent = enemyBullets + i;
        DrawEntity(hdc, ent);
    }
    DrawEntity(hdc, &player);
}
// 此代码模块中包含的函数的前向声明:
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

    // TODO: 在此处放置代码。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MY, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MY));

    MSG msg;

    srand(time(NULL));
    Init();

    // 主消息循环:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_MY);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 分析菜单选择:
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
    case WM_CREATE:
        SetTimer(hWnd, 1, timeStep, NULL);
        break;
    case WM_TIMER:
        if (wParam == 1) {
            if (nlife > 0)
                Update(timeStep / 10);
                InvalidateRect(hWnd, NULL, TRUE);
            }
            break;
    case WM_SIZE:
        wndWidth = LOWORD(lParam);
        wndHeight = HIWORD(lParam);
        break;
    case WM_KEYDOWN:
    {
        InvalidateRect(hWnd, NULL, TRUE);
        switch (wParam) {
        case VK_LEFT:
            player.dir = LEFT;
            player.p = 0;
            break;
        case VK_RIGHT:
            player.dir = RIGHT;
            player.p = 0;
            break;
        case VK_UP:
            player.dir = UP;
            player.p = 0;
            break;
        case VK_DOWN:
            player.dir = DOWN;
            player.p = 0;
            break;
        case VK_SPACE:
            bFire = 1;
            break;
        }
    }
        break;
    case WM_ERASEBKGND:
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
            HDC memHDC = CreateCompatibleDC(hdc);
            RECT rectClient;
            GetClientRect(hWnd, &rectClient);
            HBITMAP bmpBuff = CreateCompatibleBitmap(hdc, wndWidth, wndHeight);
            HBITMAP pOldBMP = (HBITMAP)SelectObject(memHDC, bmpBuff);
            PatBlt(memHDC, 0, 0, wndWidth, wndHeight, WHITENESS);
            DrawScene(memHDC);
            BOOL tt = BitBlt(hdc, rectClient.left, rectClient.top,
                wndWidth, wndHeight, memHDC, rectClient.left, rectClient.top, SRCCOPY);
            SelectObject(memHDC, pOldBMP);
            DeleteObject(bmpBuff);
            DeleteObject(memHDC);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        KillTimer(hWnd, 1);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// “关于”框的消息处理程序。
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
