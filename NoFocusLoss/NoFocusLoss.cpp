#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#define NOMINMAX
#include <Windows.h>
#include "MinHook.h"
#include <TlHelp32.h>

// Thanks to gaspardpetit (https://gist.github.com/gaspardpetit/3b1a10f6a2a9866b2bba8eb5d8e4ef36)
// Imagine wanting to use native API to get the main window handle. Thanks for nothing, Microsoft.
// Will most likely be removed in the future once I use the injector itself to set the window handle
#pragma region GetMainWindow

struct EnumWindowsCallbackArgs
{
	HWND hwnd = nullptr;
	int area = -1;
};

static BOOL CALLBACK EnumWindowsCallback(HWND hnd, LPARAM lParam)
{
	EnumWindowsCallbackArgs* args = (EnumWindowsCallbackArgs*)lParam;

	RECT rect = { 0 };
	::GetWindowRect(hnd, &rect);
	int area = (rect.right - rect.left) * (rect.bottom - rect.top);
	if (area > args->area)
	{
		args->area = area;
		args->hwnd = hnd;
	}

	return true;
}

template<class _CB>
bool VisitProcessThreads(_CB visitor)
{
	HANDLE hThreadSnap = INVALID_HANDLE_VALUE;
	THREADENTRY32 te32;

	hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hThreadSnap == INVALID_HANDLE_VALUE)
		return false;

	te32.dwSize = sizeof(THREADENTRY32);

	if (!Thread32First(hThreadSnap, &te32))
	{
		CloseHandle(hThreadSnap);
		return false;
	}

	do
	{
		visitor(te32);
	} while (Thread32Next(hThreadSnap, &te32));

	CloseHandle(hThreadSnap);
	return true;
}


HWND GetMainWindow()
{
	DWORD currentProcessId = ::GetCurrentProcessId();
	EnumWindowsCallbackArgs args{};

	VisitProcessThreads([&](THREADENTRY32 threadEntry) {
		if (threadEntry.th32OwnerProcessID != currentProcessId)
			return;

		EnumThreadWindows(threadEntry.th32ThreadID, &EnumWindowsCallback, (LPARAM)&args);
		});

	return args.hwnd;
}

#pragma endregion 


// wrapper for easier setting up hooks for MinHook
template <typename T>
inline MH_STATUS MH_CreateHookEx(LPVOID pTarget, LPVOID pDetour, T** ppOriginal)
{
	return MH_CreateHook(pTarget, pDetour, reinterpret_cast<LPVOID*>(ppOriginal));
}

HWND hwnd; // Main window handle
BOOL unfocused = FALSE;
WNDPROC OldWndProc; // Window procedures are funny

static decltype(GetForegroundWindow)* real_GetForegroundWindow = GetForegroundWindow;
static decltype(SetCursorPos)* real_SetCursorPos = SetCursorPos;

HWND WINAPI DetourGetForegroundWindow()
{
	return hwnd; // Some games seem to mute audio if they don't get their main window handle here. Not pause though, that's handled separately because uhh
}

BOOL WINAPI DetourSetCursorPos(int X, int Y)
{
	if (unfocused) {
		return TRUE; // Fuck you Alan
	}
	return real_SetCursorPos(X, Y);
}


LRESULT CALLBACK NewWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_NCACTIVATE && wParam == TRUE) {
		unfocused = FALSE;
	}
	else if (message == WM_NCACTIVATE && wParam == FALSE) {
		unfocused = TRUE;
		return 0; // This is techincally all that's needed, since it seems no more deactivate messages are sent if this one doesn't get a response.. Might want to handle it properly though
	}
	else if (message == WM_ACTIVATE && wParam == WA_INACTIVE) {
		return 0;
	}
	else if (message == WM_ACTIVATEAPP && wParam == FALSE) {
		return 0;
	}
	else if (message == WM_KILLFOCUS) {
		return 0;
	}
	else if (message == WM_IME_SETCONTEXT && wParam == FALSE) {
		return 0;
	}

	// Call the original window procedure
	return CallWindowProc(OldWndProc, hwnd, message, wParam, lParam);
}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD  fdwReason, LPVOID lpReserved)
{
	switch (fdwReason) {
		case DLL_PROCESS_ATTACH:
		{
			MH_Initialize();

			hwnd = GetMainWindow();

			MH_CreateHookEx(GetForegroundWindow, DetourGetForegroundWindow, &real_GetForegroundWindow);
			MH_CreateHookEx(SetCursorPos, DetourSetCursorPos, &real_SetCursorPos);

			if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK) {
				return FALSE;
			}

			OldWndProc = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)NewWndProc);

			break;
		}

		case DLL_PROCESS_DETACH:
		{
			SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)OldWndProc); // Very important, otherwise I imagine you'd have quite the fun time

			MH_DisableHook(MH_ALL_HOOKS);
			MH_Uninitialize();
			break;
		}
	}
	return TRUE;
}