#include <Windows.h>
#include <stdint.h>
#include <wchar.h>

typedef uint32_t u32;

// Shows if program is running or not
int running = 1;

// Client window size
int client_width = 640;
int client_heigh = 640;

float player_x;
float player_y;

float d_player_x;
float d_player_y;

// Directions where we will move our object
int moving_left;
int moving_right;
int moving_up;
int moving_down;

// object width and height
int tile_size = 25;

// memory ptr and bitmap for our object
void* memory;
BITMAPINFO bitmap_info;

float targer_seconds_per_frame = 1.0f / 120.0f;

LARGE_INTEGER frequency;

// procedure wich count total fps to make a great movement animation
float get_seconds_per_frame(LARGE_INTEGER start_counter, LARGE_INTEGER end_counter)
{
    return ((float)(end_counter.QuadPart - start_counter.QuadPart) / (float)frequency.QuadPart);
}

// fill the screen with a specific color
void clear_screen(u32 color)
{
    u32* pixel = (u32*)memory;

    for (int pixel_number = 0; pixel_number < client_width * client_heigh; ++pixel_number)
    {
        *pixel++ = color;
    }
}

// draw a rectangle with a specific color
void draw_rectangle(int rec_x, int rec_y, int rec_width, int rec_heigh, u32 color)
{
    u32* pixel = (u32*)memory;

    pixel += rec_y * client_width + rec_x;

    for (int y = 0; y < rec_heigh; ++y)
    {
        for (int x = 0; x < rec_width; ++x)
        {
            *pixel++ = color;
        }

        pixel += client_width - rec_width;
    }
}

// Main window result procedure
// HWND window - Описатель окна, в котором возникло сообщение 
// UINT message - Код сообщения
// WPARAM и LPARAM - Доп информация о сообщении
LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
    LRESULT result{};
    switch (message)
    {
        // Close the application
        case WM_CLOSE:
        {
            running = 0;
        } break;

        // Keyboard mouse up
        case WM_KEYUP:
        {
            switch (w_param)
            {
                // ->
                case VK_RIGHT:
                {
                    moving_right = 0;
                } break;

                // <-
                case VK_LEFT:
                {
                    moving_left = 0;
                } break;

                // ^
                case VK_UP:
                {
                    moving_up = 0;
                } break;

                // down
                case VK_DOWN:
                {
                    moving_down = 0;
                } break;
            }
        } break;

        // Keyboard mouse down
        case WM_KEYDOWN:
        {
            switch (w_param)
            {
                // ->
                case VK_RIGHT:
                {
                    if (!moving_right) moving_right = 1;
                } break;

                // <-
                case VK_LEFT:
                {
                    if (!moving_left) moving_left = 1;
                } break;

                // ^
                case VK_UP:
                {
                    if (!moving_up) moving_up = 1;
                } break;

                // down
                case VK_DOWN:
                {
                    if (!moving_down) moving_down = 1;
                } break;
            }
        } break;

        // Mouse right button down
        case WM_RBUTTONDOWN:
        {
            switch (w_param)
            {
                // Mouse right button
                case MK_RBUTTON:
                {
                    if (!moving_right) moving_right = 1;
                } break;
            }
        } break;

        // Mouse right button up
        case WM_RBUTTONUP:
        {
            moving_right = 0;
        } break;

        // Mouse left button down
        case WM_LBUTTONDOWN:
        {
            switch (w_param)
            {
                // Mouse left button
                case MK_LBUTTON:
                {
                    if (!moving_left) moving_left = 1;
                } break;
            }
        } break;

        // Mouse left button up
        case WM_LBUTTONUP:
        {
            moving_left = 0;
        } break;

        // Other keys
        default:
        {
            result = DefWindowProc(window, message, w_param, l_param);
        } break;
    }
    return result;
}

// program enry point
int WINAPI wWinMain(HINSTANCE instance, HINSTANCE prev_instance, PWSTR cmd_line, int cmd_show)
{
    // creating window class object
    WNDCLASS window_class = { 0 };

    // giving it a name
    wchar_t class_name[] = L"GameWindowClass";

    // attributes for the window object
    window_class.lpfnWndProc = WindowProc;
    window_class.hInstance = instance;
    window_class.lpszClassName = class_name;
    window_class.hCursor = LoadCursor(0, IDC_CROSS);

    // window size
    RECT window_rect;
    window_rect.left = 0;
    window_rect.right = client_width;
    window_rect.top = 0;
    window_rect.bottom = client_heigh;

    // calculates the required size of the window rectangle
    AdjustWindowRectEx(&window_rect, WS_OVERLAPPEDWINDOW, 0, 0);

    // getting real window width and height
    int window_width = window_rect.right - window_rect.left;
    int window_height = window_rect.bottom - window_rect.top;

    // getting screen real width and size
    int screen_width = GetSystemMetrics(SM_CXSCREEN);
    int screen_height = GetSystemMetrics(SM_CYSCREEN);

    // making our screen loaded in the center
    int window_x = (screen_width / 2) - (window_width / 2);
    int window_y = (screen_height / 2) - (window_height / 2);

    // register our window in the core
    RegisterClass(&window_class);

    // create window
    HWND window = CreateWindowEx(0, class_name, L"Game", WS_OVERLAPPEDWINDOW | WS_VISIBLE, window_x,
        window_y, window_width, window_height, 0, 0, instance, 0);

    // set up a memory to store window
    memory = VirtualAlloc(0, client_width * client_heigh * 4, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    // set up object bitmap parameters
    bitmap_info.bmiHeader.biSize = sizeof(bitmap_info.bmiHeader);
    bitmap_info.bmiHeader.biWidth = client_width;
    bitmap_info.bmiHeader.biHeight = client_heigh;
    bitmap_info.bmiHeader.biPlanes = 1;
    bitmap_info.bmiHeader.biBitCount = 32;
    bitmap_info.bmiHeader.biCompression = BI_RGB;

    // get a context to use client window area
    HDC hdc = GetDC(window);

    LARGE_INTEGER start_counter, end_counter, counts, fps, ms;

    QueryPerformanceCounter(&start_counter);

    QueryPerformanceFrequency(&frequency);

    // while program is running
    while (running)
    {
        // take a message from user
        MSG message;

        // processes the message
        while (PeekMessage(&message, window, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }

        QueryPerformanceCounter(&end_counter);

        float seconds_per_frame = get_seconds_per_frame(start_counter, end_counter);

        // giving our object some movement animation
        if (seconds_per_frame < targer_seconds_per_frame)
        {
            DWORD sleep_ms;
            sleep_ms = (DWORD)(1000 * (targer_seconds_per_frame - seconds_per_frame));
            Sleep(sleep_ms);

            while (seconds_per_frame < targer_seconds_per_frame)
            {
                QueryPerformanceCounter(&end_counter);

                seconds_per_frame = get_seconds_per_frame(start_counter, end_counter);
            }
        }

        QueryPerformanceCounter(&end_counter);
        seconds_per_frame = get_seconds_per_frame(start_counter, end_counter);
        start_counter = end_counter;

        // speed for our object
        float dt = seconds_per_frame;
        float speed = 250.0f;

        // variables to control movement
        if (moving_right) d_player_x = 1.0f;
        if (moving_left) d_player_x = -1.0f;
        if (moving_up) d_player_y = 1.0f;
        if (moving_down) d_player_y = -1.0f;

        d_player_x *= speed;
        d_player_y *= speed;

        if (d_player_x != 0.0f && d_player_y != 0.0f)
        {
            d_player_x *= 0.707f;
            d_player_y *= 0.707f;
        }

        player_x += dt * d_player_x;
        player_y += dt * d_player_y;

        // Rebounds from windows border
        // up
        if (player_y + tile_size > client_heigh)
        {
            player_x -= dt * d_player_x;
            player_y -= 7 * dt * d_player_y;
        }
        // down
        if (player_y < 0)
        {
            player_x += dt * d_player_x;
            player_y += 30;
        }
        // left
        if (player_x < 0)
        {
            player_x = 30;
            player_y += dt * d_player_y;
        }
        // right
        if (player_x + tile_size > client_width)
        {
            player_x -= 7 * dt * d_player_x;
            player_y -= dt * d_player_y;
        }

        // set movement variables to the default state
        d_player_x *= 0.0f;
        d_player_y *= 0.0f;

        // set screen to the default color
        clear_screen(0x111111);

        // draw the object with new coordinates
        draw_rectangle(player_x, player_y, tile_size, tile_size, 0xff00f7);

        // some window graphic setup
        StretchDIBits(hdc, 0, 0, client_width, client_heigh, 0, 0, client_width, client_heigh, memory, &bitmap_info, DIB_RGB_COLORS, SRCCOPY);
    }

    return 0;
}