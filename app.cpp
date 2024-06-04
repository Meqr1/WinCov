#include <iostream>
#include <vector>
#include <windows.h>

// Function to adjust window properties
void AdjustWindowProperties() {
  std::vector<std::string> Titles;
  EnumWindows(
      [](HWND hwnd, LPARAM lParam) -> BOOL {
        std::vector<std::string> &titles =
            *reinterpret_cast<std::vector<std::string> *>(lParam);

        char windowTitle[256];
        GetWindowTextA(hwnd, windowTitle, sizeof(windowTitle));

        if (IsWindowVisible(hwnd) && windowTitle[0] != '\0' &&
            strcmp(windowTitle, "ToastWindow") != 0 &&
            strcmp(windowTitle, "Program Manager") != 0 &&
            strncmp(windowTitle, "Window", 6) != 0 &&
            strncmp(windowTitle, "FancyWM", 7) != 0 &&
            strncmp(windowTitle, "Flow", 4) != 0) {
          titles.push_back(windowTitle);
        }

        return TRUE; // Continue enumeration
      },
      reinterpret_cast<LPARAM>(&Titles));

  for (const auto &title : Titles) {
    HWND hwnd = FindWindow(NULL, title.c_str());

    if (hwnd) {
      SetWindowLong(hwnd, GWL_EXSTYLE,
                    GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
      SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 240, LWA_ALPHA);

      ShowWindow(hwnd, SW_SHOW);
      UpdateWindow(hwnd);
    } else {
      std::cerr << "Failed to find window with title: " << title << std::endl;
    }
  }
}

// Callback function for WinEvent hook
void CALLBACK WinEventProc(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd,
                           LONG idObject, LONG idChild, DWORD dwEventThread,
                           DWORD dwmsEventTime) {
  if (event == EVENT_OBJECT_CREATE || event == EVENT_OBJECT_DESTROY) {
    AdjustWindowProperties(); // Run the logic to adjust window properties
  }
}

int main() {
  // Set up the WinEvent hook
  HWINEVENTHOOK hWinEventHook =
      SetWinEventHook(EVENT_OBJECT_CREATE, EVENT_OBJECT_DESTROY, NULL,
                      WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);

  if (hWinEventHook == NULL) {
    std::cerr << "Failed to set WinEvent hook" << std::endl;
    return 1;
  }

  // Message loop
  MSG msg;
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  // Unhook the WinEvent hook
  UnhookWinEvent(hWinEventHook);
  return 0;
}
