#include <Windows.h>

// 1-ый параметр - указатель на начало исполняемого модуля, 2-й не используется, 3-й - указатель на строку юникод символов, 4-й - 
// параметр того, как будет показано окно (свёрнуто, на весь экран и тп)
int CALLBACK wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR szCmdLine, int nCmdShow)
{
	MSG msg{};
	HWND hwnd{}; // дискриптор окна, где хранится инфа о созданном окне. Указатель на определённую область памяти в ядре

	WNDCLASSEX wc{ sizeof(WNDCLASSEX) }; // Характеристики окна

	wc.cbClsExtra = 0; 
	wc.cbWndExtra = 0;
	wc.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));  // Дискриптор кисти 
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);  // Подгружаем курсор и иконки окна
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hInstance = hInstance;
	wc.lpfnWndProc = [](HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)->LRESULT  // Процедура за обработку сообщений
	{
		switch (uMsg)
		{
		case WM_DESTROY:
		{
			PostQuitMessage(EXIT_SUCCESS);
		}
		return 0;
		}
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	};
	wc.lpszClassName = L"MyAppClass";  // уникальное имя класса
	wc.lpszMenuName = nullptr;
	wc.style = CS_VREDRAW | CS_HREDRAW;  // стиль окна

	// регистрация в системе класса окна
	if (!RegisterClassEx(&wc))
	{
		return EXIT_FAILURE;
	}

	if (hwnd = CreateWindow(wc.lpszClassName, L"Заголовок!", WS_OVERLAPPEDWINDOW, 0, 0, 600, 600,
		nullptr, nullptr, wc.hInstance, nullptr); hwnd == INVALID_HANDLE_VALUE)
		return EXIT_FAILURE;

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	// Запуск цикла обработки сообщений
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return static_cast<int>(msg.wParam);
}