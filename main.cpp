#include "main.h"

int main()
{
    SetConsoleTitleA("FiveM Crosshair");

    std::printf("Looking for FiveM process...\n");

    overlay::game_window = process_search::FindWindowFromProcess(process_search::GetProcessByName("FiveM_GTAProcess.exe"));

    if (!overlay::game_window)
    {
        std::printf("The FiveM window doesn't exist...\n");
        system("pause");
        return 0;
    }

    std::printf("FiveM process found!\n");

    std::this_thread::sleep_for(std::chrono::seconds(1));
    ShowWindow(GetConsoleWindow(), SW_HIDE);

    overlay::init_window_create_device();

    overlay::start_render_loop();
}