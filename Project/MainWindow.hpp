#pragma once

#include <functional>
#include <memory>
#include <unordered_map>
#include <Windows.h>

//#pragma comment(lib, "Sub.IBTP.lib")
extern "C" {
#include "../Sub.IBTP/src/ibtp.h"
}

class MainWindow {
public:
    MainWindow(MainWindow& other) = delete;
    void operator=(const MainWindow&) = delete;
	static MainWindow& getMainWindow(HINSTANCE hInstance, LPCWSTR className, LPCWSTR mainWindowTitle, int mainWindowWidth, int mainWindowHeight);
	int run(int nCmdShow);
protected:
	static const int ID_INSERT_BUTTON = 21;
	static const int ID_GET_BUTTON = 22;
	static const int ID_DELETE_BUTTON = 23;
	static const int ID_KEY_EDIT = 24;
	static const int ID_PACK_KEYS_BUTTON = 26;
	static const int ID_GENERATE_KEYS_BUTTON = 27;
	static const int ID_MENU_OPEN_FILE = 28;
	static const int ID_MENU_SAVE_TO_FILE = 29;
	static const int ID_MENU_EXIT = 30;
	static const int ID_SNAPSHOT_BUTTON = 31;
	static const int ID_GENERATION_COUNT_EDIT = 32;
protected:
	void mainWindowAddMenu();
	void subWindowsHandlersInit();
	void subWindowsHandlersFunctionInit();
	void mainWindowClassInit(HINSTANCE hInstance, LPCWSTR className);
	void mainWindowClassRegister();
	void mainWindowCreate(HINSTANCE hInstance, LPCWSTR className, LPCWSTR mainWindowTitle, int mainWindowWidth, int mainWindowHeight);
	MainWindow(HINSTANCE hInstance, LPCWSTR className, LPCWSTR mainWindowTitle, int mainWindowWidth, int mainWindowHeight);
protected:
	static WNDCLASSEX mainWindowClass;
	static HWND mainWindowHandler;
	static std::unordered_map<int, std::function<void()>> subWindowsHandlerFunctions;
	static std::unordered_map<int, HWND> subWindowsHandlers;
protected:
	static LRESULT CALLBACK mainWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};