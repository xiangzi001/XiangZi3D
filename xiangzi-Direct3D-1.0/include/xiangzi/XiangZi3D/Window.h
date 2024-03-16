
WNDCLASSEX _WindowsClass;
int _WindowsNum = 0;

void WindowsInit(WNDPROC func, HINSTANCE hInstance, LPCSTR icon) {
	memset(&_WindowsClass, 0, sizeof(WNDCLASSEX));
	_WindowsClass.cbSize = sizeof(WNDCLASSEX);
	_WindowsClass.lpfnWndProc = func;
	_WindowsClass.hInstance = hInstance;
	_WindowsClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	_WindowsClass.hbrBackground = NULL;
	_WindowsClass.lpszClassName = "XiangZi";
	_WindowsClass.hIcon = LoadIcon(hInstance, icon);
	_WindowsClass.hIconSm = LoadIcon(hInstance, icon);
	if (!RegisterClassEx(&_WindowsClass)) {
		MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return;
	}
}

class DirectWindow
{
private:
    XMouse m_test;
    XMouse m_now;
    XMouse m_old;
    XKey k_test;
    XKey k_now;
    XKey k_old;

public:
    bool use;

    HINSTANCE	hInstance;
    HWND		hWnd;

    RECT ClientRect;
    RECT WindowRect;

    POINT mouse;
    char mouse_kind;

    DirectWindow() :
        use(false),
        hInstance(NULL),
        hWnd(NULL),
        ClientRect{},
        WindowRect{},
        mouse{},
        mouse_kind(0),
        m_test{ false,false,false,false,false,false },
        m_now{ false,false,false,false,false,false },
        m_old{ false,false,false,false,false,false }
    {
    }

    void join(void* func) {
        thread t_main(*(void(*)()) func);
        t_main.detach();
    }

    void create(const char*, RECT, HINSTANCE);

    void close();

    LRESULT DefWinMSG(HWND, UINT, WPARAM, LPARAM);

    bool if_mouse_press(UINT msg) {
        return m_now(msg);
    }
    bool if_mouse_pressonce(UINT msg) {
        return (m_now(msg) && (!m_old(msg)));
    }
    bool if_key_press(WPARAM wParam) {
        return k_now(wParam);
    }
    bool if_key_pressonce(WPARAM wParam) {
        return (k_now(wParam) && (!k_old(wParam)));
    }

};

void DirectWindow::close() {
    use = false;
    _WindowsNum--;
    if (_WindowsNum == 0) {
        PostQuitMessage(0);
    }
}

void DirectWindow::create(const char* Name, RECT Rect, HINSTANCE HInstance = _WindowsClass.hInstance) {
    ClientRect = Rect;
    hInstance = HInstance;
    if (use) {
        SendMessageA(hWnd, WM_SYSCOMMAND, SC_CLOSE, NULL);
    }
    hWnd = CreateWindowExA(NULL, _WindowsClass.lpszClassName, Name, WS_VISIBLE | WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        Rect.right, Rect.bottom,
        NULL, NULL, hInstance, NULL);

    if (hWnd == NULL) {
        MessageBoxA(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return;
    }
    _WindowsNum++;

    ShowWindow(hWnd, 10);
    UpdateWindow(hWnd);

    HIMC g_hIMC = ImmAssociateContext(hWnd, NULL);

    use = true;
}

LRESULT DirectWindow::DefWinMSG(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam) {

    GetMessagePos();

    switch (Message) {
    case WM_CREATE: {
        hInstance = ((LPCREATESTRUCT)lParam)->hInstance;
        break; }
    case WM_KEYDOWN: {
        k_test.keypress(wParam);
        break; }
    case WM_KEYUP: {
        k_test.keyup(wParam);
    }
    case WM_MOUSEMOVE: {
        mouse.x = GET_X_LPARAM(lParam) / ClientRect.right;
        mouse.y = GET_Y_LPARAM(lParam) / ClientRect.bottom;

        if (mouse_kind == 0) SetCursor(LoadCursorA(NULL, IDC_ARROW));
        else if (mouse_kind == 1) SetCursor(LoadCursorA(NULL, IDC_HAND));
        break; }
    case WM_MOUSEWHEEL: {
        break; }
    case WM_PAINT: {
        break; }
    case WM_SIZE: {
        GetClientRect(hWnd, &ClientRect);
        break; }
    case WM_SYSCOMMAND: {
        switch (wParam)
        {
        case SC_CLOSE: {
            close();
            break;
        }
        default:
            break;
        }
        return DefWindowProc(hWnd, Message, wParam, lParam);
    }
    default: {
        if (m_test.mouseclick(Message))
            return 1;
        else
            return DefWindowProc(hWnd, Message, wParam, lParam);
    }
    }
    return 1;
}
