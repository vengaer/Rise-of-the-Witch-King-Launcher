#include "config.h"
#include "command.h"
#include "fsys.h"
#include "win_gui.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined __CYGWIN__ || defined _WIN32
#include <windows.h>

char const g_szClassName[] = "Launcher";

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSEX wc;
    HWND hwnd;
    MSG Msg;

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if(!RegisterClassEx(&wc)) {
        MessageBox(NULL, "Window registration failed", "Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        g_szClassName,
        "Rise of the Witch-King Unofficial Launcher",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 600, 400,
        NULL, NULL, hInstance, NULL);
    if(hwnd == NULL) {
        MessageBox(NULL, "Window creation failed", "Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    
    while(GetMessage(&Msg, NULL, 0, 0) > 0) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    return Msg.wParam;
}

#else
int main(int argc, char** argv) {
    if(argc < 2) {
        #if defined __CYGWIN__ || defined _WIN32
        if(!file_exists(CONFIG_FILE)) {
            /* gui setup */
        }

        return 0;
        #elif defined __linux__
        fprintf(stderr, "GUI not available on Linux. Try rotwkl --help\n");
        return 1;
        #endif
    }
    
    if(argv[1][0] != '-') {
        fprintf(stderr, "First argument must be one or more options. Try rotwkl --help\n");
        return 1;
    }
        
    if(strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
        fprintf(stderr, "Usage: rotwkl [OPTION] [VERSION]\n\n");
        fprintf(stderr, "    -r, --run           Run the given configuration\n");
        fprintf(stderr, "    -s, --set           Specify what configuration to enable\n");
        fprintf(stderr, "    -u, --update        Update config file for specified configuration\n");
        fprintf(stderr, "    -h, --help          Display this help message\n");
        fprintf(stderr, "Available configurations are:\n");
        fprintf(stderr, "    rotwk\n    edain(*)\n    botta(*)\n");
        fprintf(stderr, "* The mod to enable must be installed correctly\n");

        return 1;
    }
    if(strcmp(argv[1], "--run") == 0 || strcmp(argv[1], "-r") == 0) {
        if(argc < 3) {
            fprintf(stderr, "%s must be followed by a configuration\n", argv[1]);
            return 1;
        }
        if(strcmp(argv[2], "rotwk") == 0)
            active_configuration("toml/rotwk.toml");
        else if(strcmp(argv[2], "edain") == 0) 
            active_configuration("toml/edain.toml");
        else if(strcmp(argv[2], "botta") == 0) 
            active_configuration("toml/botta.toml");
        else {
            fprintf(stderr, "Unknown configuration %s\n", argv[2]);
            return 1;
        }
        /* mount */
        launch_game();

        while(game_running())
            sleep_for(SLEEP_TIME);
        
        /* umount */

    }
    else if(strcmp(argv[1], "--set") == 0 || strcmp(argv[1], "-s") == 0) {
        if(argc < 3) {
            fprintf(stderr, "%s must be followed by a configuration\n", argv[1]);
            return 1;
        }
        if(strcmp(argv[2], "rotwk") == 0) 
            active_configuration("toml/rotwk.toml");
        else if(strcmp(argv[2], "edain") == 0) 
            active_configuration("toml/edain.toml");
        else if(strcmp(argv[2], "botta") == 0) 
            active_configuration("toml/botta.toml");
        else {
            fprintf(stderr, "Unknown configuration %s\n", argv[2]);
            return 1;
        }
    }
    else if(strcmp(argv[1], "--update") == 0 || strcmp(argv[1], "-u") == 0) {
        if(argc < 3) {
            fprintf(stderr, "%s must be followed by a configuration\n", argv[1]);
            return 1;
        }
        if(strcmp(argv[2], "rotwk") == 0) {
            update_config_file("toml/rotwk.toml");
        }
        else if(strcmp(argv[2], "edain") == 0) {
            update_config_file("toml/edain.toml");
        }
        else if(strcmp(argv[2], "botta") == 0) {
            update_config_file("toml/botta.toml");
        }
        else {
            fprintf(stderr, "Unknown configuration %s\n", argv[2]);
            return 1;
        }
    }
    else {
        fprintf(stderr, "Missing options. Try rotwkl --help\n");
        return 1;
    }


    return 0;
}
#endif
