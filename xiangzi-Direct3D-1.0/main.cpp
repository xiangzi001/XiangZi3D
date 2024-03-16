
#include "resource.h"
#include "include/xiangzi/XiangZi3D.h"


int win_max_num = 6;

DirectWindow* win = new DirectWindow[win_max_num];

void windowMain_0() {

    //objModel model;
    //model.objLoad("./.images/Lowpoly_tree_sample.obj");
    //model.mtlLoad("./.images/Lowpoly_tree_sample.mtl");
    //model.objLoad("./.images/indoor plant_02.obj");
    //model.mtlLoad("./.images/indoor plant_02.mtl");

    DirectScene dScene;
    Timer timer;
    timer.Fps(30);

    dScene.Blind(&(win[0]));
    dScene.InitGPU_3D();
    dScene.InitBuffer();

    Object obj;
    obj.Init(&(dScene));
    obj.Load(vertices, 8, indices, 36);

    float angle = 0;
    while (win[0].use) {
        timer.Start();

        dScene.BeginPaint();

        obj.SetTranslation(1, -1, 0);
        obj.SetRotation(0, angle, -0.4);
        obj.Draw();
        angle = angle + Pi/30;

        dScene.EndPaint();

        timer.End();
        timer.Wait();
    }
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	for (int i = 0; i < win_max_num; i++)
		if (win[i].hWnd == hwnd) {
			return win[i].DefWinMSG(hwnd, Message, wParam, lParam);
		}

	return DefWindowProc(hwnd, Message, wParam, lParam);
}

/* The 'main' function of Win32 GUI programs: this is where execution starts */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	WindowsInit(WndProc, hInstance, MAKEINTRESOURCE(IDI_ICON1));

	win[0].create("Mine-XiangZi", { 0,0,600,400 });
	//win[0].main(windowMain_0, 10);
    win[0].join(windowMain_0);

	MSG msg;
	while (GetMessageA(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}