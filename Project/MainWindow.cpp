#include "MainWindow.hpp"

#include <utility>
#include <iterator>
#include <regex>
#include <ShellScalingApi.h>
#include <wingdi.h>
#include <random>

#pragma comment(lib, "Shcore.lib")
#pragma comment(lib, "Msimg32.lib")

extern "C" {
#include "../Sub.IBTP/src/ibtp.h"
}

//!< Storage.
static struct KV kv;
/*
* btree set of callback proxy functions
*/
btnode btree_page_get_proxy(btptr ptr) {
    return mdlIBTP.fileMappingRealization.treeCallbacks.page_get(&kv, ptr);
}
btptr btree_page_new_proxy(btnode node) {
    return mdlIBTP.fileMappingRealization.treeCallbacks.page_new(&kv, node);
}
void btree_page_del_proxy(btptr ptr) {
    mdlIBTP.fileMappingRealization.treeCallbacks.page_del(&kv, ptr);
}
/*
* free list set of callback proxy functions
*/
btnode free_list_page_get_proxy(btptr ptr) {
    return mdlIBTP.fileMappingRealization.freeListCallbacks.page_get(&kv, ptr);
}
btptr free_list_page_append_proxy(btnode node) {
    return mdlIBTP.fileMappingRealization.freeListCallbacks.page_append(&kv, node);
}
void free_list_page_use_proxy(btptr ptr, btnode node) {
    mdlIBTP.fileMappingRealization.freeListCallbacks.page_use(&kv, ptr, node);
}

WNDCLASSEX MainWindow::mainWindowClass;
HWND MainWindow::mainWindowHandler;
std::unordered_map<int, std::function<void()>> MainWindow::subWindowsHandlerFunctions;
std::unordered_map<int, HWND> MainWindow::subWindowsHandlers;

void MainWindow::mainWindowClassInit(HINSTANCE hInstance, LPCWSTR className) {
    mainWindowClass.cbSize = sizeof(WNDCLASSEX);
    mainWindowClass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    mainWindowClass.lpfnWndProc = MainWindow::mainWindowProc;  //!< controller
    mainWindowClass.cbClsExtra = 0;
    mainWindowClass.cbWndExtra = 0;
    mainWindowClass.hInstance = hInstance; //!< parameter
    mainWindowClass.hIcon = LoadIcon(mainWindowClass.hInstance, IDI_APPLICATION);
    mainWindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    mainWindowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    mainWindowClass.lpszMenuName = NULL;
    mainWindowClass.lpszClassName = className;
    mainWindowClass.hIconSm = mainWindowClass.hIcon;
}
void MainWindow::mainWindowClassRegister() {
    if (!RegisterClassEx(&mainWindowClass))
    {
        MessageBox(NULL,
            TEXT("Call to RegisterClassEx failed!"),
            TEXT("Windows Desktop Guided Tour"),
            NULL);
        exit(1);
    }
}
void MainWindow::mainWindowCreate(HINSTANCE hInstance, LPCWSTR className, LPCWSTR mainWindowTitle, int mainWindowWidth, int mainWindowHeight) {
    mainWindowHandler = CreateWindowEx(
        WS_EX_WINDOWEDGE | WS_EX_ACCEPTFILES,
        className,
        mainWindowTitle,
        WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, mainWindowWidth, mainWindowHeight,
        NULL,
        NULL,
        hInstance,
        NULL
    );
    if (!mainWindowHandler)
    {
        MessageBox(NULL,
            TEXT("Call to CreateWindowEx failed!"),
            TEXT("Windows Desktop Guided Tour"),
            NULL);
        exit(1);
    }
}
MainWindow::MainWindow(HINSTANCE hInstance, LPCWSTR className, LPCWSTR mainWindowTitle, int mainWindowWidth, int mainWindowHeight) {
    SYSTEM_INFO sys_info = { 0 };
    GetSystemInfo(&sys_info);
    DWORD page_size = sys_info.dwPageSize;

    int status = ImmutableBTreePlus_initialize(page_size);
    if (status) return;

    status = mdlIBTP.fileMappingRealization.kvstore_open(&kv, "Nice try", L"default.db.txt", btree_page_get_proxy, btree_page_new_proxy, btree_page_del_proxy,
        free_list_page_get_proxy, free_list_page_append_proxy, free_list_page_use_proxy);
    if (status) {
        MessageBox(mainWindowHandler, L"Database Wrong Signature", L"Exit", NULL);
        exit(status);
    }
    mainWindowClassInit(hInstance, className);
    mainWindowClassRegister();
    mainWindowCreate(hInstance, className, mainWindowTitle, mainWindowWidth, mainWindowHeight);
    subWindowsHandlersInit();
    subWindowsHandlersFunctionInit();
    mainWindowAddMenu();
}

void MainWindow::mainWindowAddMenu() {
    HMENU RootMenu = CreateMenu();
    HMENU SubMenu = CreateMenu();
    AppendMenu(RootMenu, MF_POPUP, (UINT_PTR)SubMenu, L"File");
    AppendMenu(SubMenu, MF_STRING, ID_MENU_OPEN_FILE, L"Open file");
    AppendMenu(SubMenu, MF_STRING, ID_MENU_SAVE_TO_FILE, L"Save to file");
    AppendMenu(SubMenu, MF_SEPARATOR, NULL, NULL);
    AppendMenu(SubMenu, MF_STRING, ID_MENU_EXIT, L"Exit");
    SetMenu(mainWindowHandler, RootMenu);
}
void MainWindow::subWindowsHandlersInit() {
    RECT windowFrame;
    GetWindowRect(mainWindowHandler, &windowFrame);
    LONG windowFrameWidth = windowFrame.right - windowFrame.left;
    LONG windowFrameHeight = windowFrame.bottom - windowFrame.top;
    HWND hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER | WS_HSCROLL | ES_AUTOHSCROLL | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_NOHIDESEL,
        150, 10,
        windowFrameWidth - 150 - 10 - 10 - 2 * SM_CXBORDER,
        windowFrameHeight - 10 - 10 - 2 * SM_CYBORDER - GetSystemMetrics(SM_CYCAPTION) - GetSystemMetrics(SM_CYMENU),
        mainWindowHandler, (HMENU)ID_KEY_EDIT, (HINSTANCE)GetWindowLongPtr(mainWindowHandler, GWLP_HINSTANCE), NULL);
    SetWindowLong(hEdit,
        GWL_EXSTYLE,
        GetWindowLong(hEdit, GWL_EXSTYLE) | WS_EX_LAYERED);
    SetLayeredWindowAttributes(hEdit, RGB(255, 255, 255), 225, LWA_ALPHA);
    subWindowsHandlers.insert(std::pair<int, HWND>
        (ID_GENERATION_COUNT_EDIT, CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", NULL, WS_VISIBLE | WS_CHILD | WS_BORDER | ES_LEFT | ES_NOHIDESEL, 
            10, 250, 140, 30, mainWindowHandler, (HMENU)ID_GENERATION_COUNT_EDIT, (HINSTANCE)GetWindowLongPtr(mainWindowHandler, GWLP_HINSTANCE), NULL)));
    subWindowsHandlers.insert(std::pair<int, HWND>
        (ID_KEY_EDIT, hEdit));
    subWindowsHandlers.insert(std::pair<int, HWND>
        (ID_INSERT_BUTTON, CreateWindowEx(0, L"BUTTON", L"Insert key-value", WS_CHILD | WS_VISIBLE, 10, 10, 140, 40, mainWindowHandler, (HMENU)ID_INSERT_BUTTON, NULL, NULL)));
    subWindowsHandlers.insert(std::pair<int, HWND>
        (ID_DELETE_BUTTON, CreateWindowEx(0, L"BUTTON", L"Delete key", WS_CHILD | WS_VISIBLE, 10, 50, 140, 40, mainWindowHandler, (HMENU)ID_DELETE_BUTTON, NULL, NULL)));
    subWindowsHandlers.insert(std::pair<int, HWND>
        (ID_GET_BUTTON, CreateWindowEx(0, L"BUTTON", L"Get key value", WS_CHILD | WS_VISIBLE, 10, 90, 140, 40, mainWindowHandler, (HMENU)ID_GET_BUTTON, NULL, NULL)));
    subWindowsHandlers.insert(std::pair<int, HWND>
        (ID_DELETE_BUTTON, CreateWindowEx(0, L"BUTTON", L"Pack keys", WS_CHILD | WS_VISIBLE, 10, 130, 140, 40, mainWindowHandler, (HMENU)ID_PACK_KEYS_BUTTON, NULL, NULL)));
    subWindowsHandlers.insert(std::pair<int, HWND>
        (ID_DELETE_BUTTON, CreateWindowEx(0, L"BUTTON", L"Generate keys", WS_CHILD | WS_VISIBLE, 10, 170, 140, 40, mainWindowHandler, (HMENU)ID_GENERATE_KEYS_BUTTON, NULL, NULL)));
    subWindowsHandlers.insert(std::pair<int, HWND>
        (ID_DELETE_BUTTON, CreateWindowEx(0, L"BUTTON", L"Snapshot", WS_CHILD | WS_VISIBLE, 10, 210, 140, 40, mainWindowHandler, (HMENU)ID_SNAPSHOT_BUTTON, NULL, NULL)));
}

std::string random_string(std::size_t length)
{
    const std::string CHARACTERS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    std::random_device random_device;
    std::mt19937 generator(random_device());
    std::uniform_int_distribution<> distribution(0, CHARACTERS.size() - 1);

    std::string random_string;

    for (std::size_t i = 0; i < length; ++i)
    {
        random_string += CHARACTERS[distribution(generator)];
    }

    return random_string;
}

void MainWindow::subWindowsHandlersFunctionInit() {
    subWindowsHandlerFunctions.insert(std::pair<int, std::function<void()>>(ID_MENU_OPEN_FILE, [&] {
        OPENFILENAMEA ofn;
        /*open file*/ {
            char file_name[200];
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(OPENFILENAME);
            ofn.hwndOwner = mainWindowHandler;
            ofn.lpstrFile = file_name;
            ofn.lpstrFile[0] = '\0';
            ofn.nMaxFile = sizeof(file_name);
            ofn.lpstrFilter = "All files\0*.*\0Text files\0*.txt\0";
            ofn.nFilterIndex = 1;
            GetOpenFileNameA(&ofn);
        }
        /*display file*/ {
            FILE* file;
            if (fopen_s(&file, ofn.lpstrFile, "rb")) return;
            fseek(file, 0, SEEK_END);
            int _size = ftell(file);
            rewind(file);
            char* _file_data = (char*)malloc(_size + 1);
            if (_file_data == NULL) {
                fclose(file);
                return;
            }
            fread_s(_file_data, _size, _size, 1, file);
            _file_data[_size] = '\0';
            SetWindowTextA(subWindowsHandlers[ID_KEY_EDIT], _file_data);
            fclose(file);
            free(_file_data);
        }
        SetFocus(mainWindowHandler);
        MessageBox(NULL, L"File is opened", L"File opening", MB_ICONINFORMATION | MB_OK);
        }));
    subWindowsHandlerFunctions.insert(std::pair<int, std::function<void()>>(ID_MENU_SAVE_TO_FILE, [&] {
        OPENFILENAMEA ofn;
        /*open file*/ {
            char file_name[200];
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(OPENFILENAME);
            ofn.hwndOwner = mainWindowHandler;
            ofn.lpstrFile = file_name;
            ofn.lpstrFile[0] = '\0';
            ofn.nMaxFile = sizeof(file_name);
            ofn.lpstrFilter = "All files\0*.*\0Text files\0*.txt\0";
            ofn.nFilterIndex = 1;
            GetSaveFileNameA(&ofn);
        }
        /*display file*/ {
            FILE* file;
            if (fopen_s(&file, ofn.lpstrFile, "w")) return;
            int _size = GetWindowTextLengthA(subWindowsHandlers[ID_KEY_EDIT]);
            char* _file_data = (char*)malloc(_size + 1);
            if (_file_data == NULL) {
                fclose(file);
                return;
            }
            _file_data[_size] = '\0';
            GetWindowTextA(subWindowsHandlers[ID_KEY_EDIT], _file_data, _size);
            fwrite(_file_data, _size, 1, file);         
            fclose(file);
            free(_file_data);
        }
        SetFocus(mainWindowHandler);
        MessageBox(NULL, L"Saved to file", L"Saving to file", MB_ICONINFORMATION | MB_OK);
        }));
    subWindowsHandlerFunctions.insert(std::pair<int, std::function<void()>>(ID_MENU_EXIT, [&] {
        SendMessage(mainWindowHandler, WM_DESTROY, 42, 42);
        }));
    subWindowsHandlerFunctions.insert(std::pair<int, std::function<void()>>(ID_INSERT_BUTTON, [&] {
        int message = MessageBox(NULL, L"Are you sure you want to insert this keys?", L"Inserting to database", MB_ICONQUESTION | MB_YESNOCANCEL);
        switch (message) {
        case IDYES: {
            int text_length = GetWindowTextLength(subWindowsHandlers[ID_KEY_EDIT]);
            char* buff = (char*)malloc(sizeof(char) * (text_length + 1)); //!< [ ] endofline
            if (buff) {
                GetWindowTextA(subWindowsHandlers[ID_KEY_EDIT], buff, text_length + 1);
                std::vector<std::pair<std::string, std::string>> keyValuePairs;
                std::string subject(buff);
                const std::regex pattern{R"(\[\r\nkey:.{1,1000}\r\nvalue:.{0,3000}\r\n\])"};
                for (auto i = std::sregex_iterator(subject.begin(), subject.end(), pattern); i != std::sregex_iterator(); ++i) {
                    std::string substr(std::move(i->str()));
                    const std::regex key_pattern(R"(^key:.{1,1000})");
                    const std::regex value_pattern(R"(^value:.{0,3000})");
                    std::string key, value;
                    auto key_it = std::sregex_iterator(substr.begin(), substr.end(), key_pattern, std::regex_constants::match_default);
                    if (key_it != std::sregex_iterator()) {
                        key = key_it->str().substr(sizeof("key"), key_it->str().length() - sizeof("key")); //!< "key".length() + sizeof(':') = sizeof("key")
                    }
                    auto value_it = std::sregex_iterator(substr.begin(), substr.end(), value_pattern);
                    if (value_it != std::sregex_iterator()) {
                        value = value_it->str().substr(sizeof("value"), value_it->str().length() - sizeof("value"));
                    }
                    keyValuePairs.push_back({key, value});
                }
                free(buff);
                for (auto keyValuePair : keyValuePairs) {
                    key* key = dump_pack(keyValuePair.first.size(), (unsigned char*)keyValuePair.first.c_str(), 1);
                    value* value = dump_pack(keyValuePair.second.size(), (unsigned char*)keyValuePair.second.c_str(), 1);
                    mdlIBTP.fileMappingRealization.main.set(&kv, key, value);
                    dump_delete_fr(key);
                    dump_delete_fr(value);
                }
                SetWindowText(subWindowsHandlers[ID_KEY_EDIT], NULL);
                MessageBeep(MB_ICONASTERISK);
                break;
            }
        }
        case IDNO:
            break;
        case IDCANCEL:
            break;
        }
        }));
    subWindowsHandlerFunctions.insert(std::pair<int, std::function<void()>>(ID_DELETE_BUTTON, [&] {
        int textLength = SendMessage(subWindowsHandlers[ID_KEY_EDIT], WM_GETTEXTLENGTH, 0, 0);
        if (textLength == 0) {
            MessageBeep(MB_ICONERROR);
            return;
        }
        int message = MessageBox(NULL, L"Are you sure you want to delete this keys?", L"Deleting from database", MB_ICONQUESTION | MB_YESNOCANCEL);
        switch (message) {
        case IDYES: {
            auto got = subWindowsHandlers.find(ID_KEY_EDIT);
            if (got != subWindowsHandlers.end())
            {
                int textLength = SendMessage(got->second, WM_GETTEXTLENGTH, 0, 0);
                char* buffer = new char[textLength + 1];
                SendMessageA(got->second, WM_GETTEXT, textLength + 1, reinterpret_cast<LPARAM>(buffer));
                std::string key_string(buffer);
                std::vector<std::string> all_keys;
                int pos = 0;
                while (pos <= key_string.size()) {
                    pos = key_string.find("\r\n");
                    all_keys.push_back(key_string.substr(0, pos));
                    key_string.erase(0, pos + 2); // 3 is the length of the delimiter, "%20"
                }
                for (std::string& operating_key : all_keys) {
                    key* key = dump_pack(operating_key.size(), (unsigned char*)operating_key.c_str(), 0);
                    int deleted = mdlIBTP.fileMappingRealization.main.del(&kv, key);
                    if (deleted == 0) {
                        static const char* string_format = "Key deletion failed. Key %.*s doesn't exist.";
                        int length = snprintf(NULL, 0, string_format, key->size, key->dump);
                        char* str = (char*)malloc(length + 1);
                        snprintf(str, length + 1, string_format, key->size, key->dump);
                        MessageBoxA(mainWindowHandler, str, "Key deletion", MB_ICONERROR | MB_OK);
                        free(str);
                    }
                }
                SetWindowText(got->second, NULL);
            }
            MessageBeep(MB_ICONASTERISK);
            break;
        }
        case IDNO:
            break;
        case IDCANCEL:
            break;
        }
        }));
    subWindowsHandlerFunctions.insert(std::pair<int, std::function<void()>>(ID_GET_BUTTON, [&] {
        auto got = subWindowsHandlers.find(ID_KEY_EDIT);
        if (got != subWindowsHandlers.end())
        {
            int textLength = SendMessage(got->second, WM_GETTEXTLENGTH, 0, 0);
            char* buffer = new char[textLength + 1];
            SendMessageA(got->second, WM_GETTEXT, textLength + 1, reinterpret_cast<LPARAM>(buffer));
            std::string key_string(buffer);
            std::vector<std::string> all_keys;
            int pos = 0;
            while (pos <= key_string.size()) {
                pos = key_string.find("\r\n");
                all_keys.push_back(key_string.substr(0, pos));
                key_string.erase(0, pos + 2); // 3 is the length of the delimiter, "%20"
            }
            std::string result_string;
            for (std::string& operating_key : all_keys) {
                if (operating_key.size() != 0) 
                {
                    result_string.append("[\r\n");
                    result_string.append("key:");
                    key* key = dump_pack(operating_key.size(), (unsigned char*)operating_key.c_str(), 0);
                    result_string.append(key->dump, key->dump + key->size);
                    result_string.append("\r\nvalue:");
                    value* val = mdlIBTP.fileMappingRealization.main.get(&kv, key);
                    if (val == (void*)0) 
                    {
                        static const char* string_format = "Key search failed. Key %.*s doesn't exist.";
                        int length = snprintf(NULL, 0, string_format, key->size, key->dump);
                        char* str = (char*)malloc(length + 1);
                        snprintf(str, length + 1, string_format, key->size, key->dump);
                        MessageBoxA(mainWindowHandler, str, "Key search", MB_ICONERROR | MB_OK);
                        result_string.append("!!!not found!!!");
                        free(str);
                    }
                    else {
                        if (val->size != 0)
                            result_string.append(val->dump, val->dump + val->size);
                        dump_delete_fr(val);
                    }
                    result_string.append("\r\n]\r\n");                  
                }
            }
            SetWindowTextA(got->second, result_string.c_str());
            MessageBeep(MB_ICONASTERISK);
        }
        }));
    subWindowsHandlerFunctions.insert(std::pair<int, std::function<void()>>(ID_PACK_KEYS_BUTTON, [&] {
        auto got = subWindowsHandlers.find(ID_KEY_EDIT);
        if (got != subWindowsHandlers.end())
        {
            int textLength = SendMessage(got->second, WM_GETTEXTLENGTH, 0, 0);
            char* buffer = new char[textLength + 1];
            SendMessageA(got->second, WM_GETTEXT, textLength + 1, reinterpret_cast<LPARAM>(buffer));
            std::string key_string(buffer);
            std::vector<std::string> all_keys;
            int pos = 0;
            while (pos <= key_string.size()) {
                pos = key_string.find("\r\n");
                all_keys.push_back(key_string.substr(0, pos));
                key_string.erase(0, pos + 2); // 3 is the length of the delimiter, "%20"
            }
            std::string result_string;
            for (std::string& operating_key : all_keys) {
                if (operating_key.size() != 0)
                {
                    result_string.append("[\r\n");
                    result_string.append("key:");
                    result_string.append(operating_key);
                    result_string.append("\r\nvalue:");
                    result_string.append("\r\n]\r\n");
                }
            }
            SetWindowTextA(got->second, result_string.c_str());
            MessageBeep(MB_ICONASTERISK);
        }       
        }));
    subWindowsHandlerFunctions.insert(std::pair<int, std::function<void()>>(ID_GENERATE_KEYS_BUTTON, [&] {
        auto got = subWindowsHandlers.find(ID_KEY_EDIT);
        auto generationKeysCountEdit = subWindowsHandlers.find(ID_GENERATION_COUNT_EDIT);
        if (got != subWindowsHandlers.end() && generationKeysCountEdit != subWindowsHandlers.end())
        {        
            int _size = GetWindowTextLengthA(generationKeysCountEdit->second);
            if (_size == 0) {
                MessageBox(NULL, L"Keys count and length are not defined(%count%:%length%).", L"Key generation", MB_ICONINFORMATION | MB_OK);
                return;
            }
            char* _data = (char*)malloc(_size + 1);
            if (_data == NULL) {               
                return;
            }
            _data[_size] = '\0';
            GetWindowTextA(generationKeysCountEdit->second, _data, _size+1);
            std::string result_string;
            char* key_length_ptr = strchr(_data, ':');
            *key_length_ptr = '\0';
            int keys_count = atoi(_data);
            int key_length = atoi(++key_length_ptr);
            if (key_length == 0) {
                SetWindowTextA(generationKeysCountEdit->second, NULL);
                MessageBeep(MB_ICONERROR);
                return;
            }
            for (int i = 0; i < keys_count; i++) {             
                result_string.append(random_string(key_length));
                result_string.append("\r\n");
            }         
            SetWindowTextA(got->second, result_string.c_str());
            SetWindowTextA(generationKeysCountEdit->second, NULL);
            free(_data);
            MessageBeep(MB_ICONASTERISK);
        }
        }));
    subWindowsHandlerFunctions.insert(std::pair<int, std::function<void()>>(ID_SNAPSHOT_BUTTON, [&] {
        auto got = subWindowsHandlers.find(ID_KEY_EDIT);
        if (got != subWindowsHandlers.end())
        {
            FILE* file;
            if (fopen_s(&file, "tree_dump.txt", "w+t")) return;
            btnode root_node = kv.tree.get(kv.tree.root);
            mdlIBTP.fileMappingRealization.utils.dump_tree(file, &kv.tree, &root_node, 0);
            fclose(file);
            MessageBeep(MB_ICONASTERISK);
        }
        }));
}

MainWindow& MainWindow::getMainWindow(HINSTANCE hInstance, LPCWSTR className, LPCWSTR mainWindowTitle, int mainWindowWidth, int mainWindowHeight) {
    static MainWindow _mainWindow(hInstance, className, mainWindowTitle, mainWindowWidth, mainWindowHeight);
    return _mainWindow;
}

int MainWindow::run(int nCmdShow) {
    ShowWindow(mainWindowHandler, nCmdShow);
    UpdateWindow(mainWindowHandler);
    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}

LRESULT CALLBACK MainWindow::mainWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static LONG windowFrameWidth;
    static LONG windowFrameHeight;
    static HBITMAP hbmBack;
    static HBRUSH hbrBack;
    switch (uMsg)
    {
    case WM_CREATE: 
    {
        SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
        RECT windowFrame;
        GetWindowRect(hwnd, &windowFrame);
        windowFrameWidth = windowFrame.right - windowFrame.left;
        windowFrameHeight = windowFrame.bottom - windowFrame.top;
        hbmBack = (HBITMAP)LoadImage(NULL, L".\\back1.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
        hbrBack = CreatePatternBrush(hbmBack);
        break;
    }
    case WM_DESTROY:
    {
        PostQuitMessage(4);
        mdlIBTP.fileMappingRealization.kvstore_close(&kv);
        break;
    }
    case WM_LBUTTONDOWN: {
        // Passing the parent window as the parameter.
        SetFocus(hwnd);
        break;
    }
    case WM_CHAR:
    {
#if 0
        enum EDIT_CODS
        {
            CTRL_BASE = 'A' - 1,
            SELECT_ALL = 'A' - CTRL_BASE, // Ctrl+A
            COPY = 'C' - CTRL_BASE, // Ctrl+C
            CUT = 'X' - CTRL_BASE, // Ctrl+X
            PASTE = 'V' - CTRL_BASE, // Ctrl+V
            UNDO = 'Z' - CTRL_BASE, // Ctrl+Z
            REDO = 'Y' - CTRL_BASE, // Ctrl+Y
        };
        switch (wParam)
        {
        case EDIT_CODS::SELECT_ALL:
        {
            auto got = subWindowsHandlers.find(ID_KEY_EDIT);
            if (got != subWindowsHandlers.end())
                SendMessage(got->second, EM_SETSEL, 0, -1); return 1;
            break;
        }
        case EDIT_CODS::COPY:
        {
            DWORD selStart;
            DWORD selEnd;
            auto got = subWindowsHandlers.find(ID_KEY_EDIT);
            if (got != subWindowsHandlers.end())
            {
                SendMessage(got->second, EM_GETSEL, reinterpret_cast<WPARAM>(&selStart), reinterpret_cast<WPARAM>(&selEnd));
                int textLength = SendMessage(got->second, WM_GETTEXTLENGTH, 0, 0);
                TCHAR* buffer = new TCHAR[textLength + 1];
                SendMessage(got->second, WM_GETTEXT, textLength + 1, reinterpret_cast<LPARAM>(buffer));
                std::string text(buffer + selStart, buffer + selEnd);

            }
            break;
        }
        case EDIT_CODS::CUT:
        {
            DWORD selStart;
            DWORD selEnd;
            auto got = subWindowsHandlers.find(ID_KEY_EDIT);
            if (got != subWindowsHandlers.end())
            {
                SendMessage(got->second, EM_GETSEL, reinterpret_cast<WPARAM>(&selStart), reinterpret_cast<WPARAM>(&selEnd));
                int textLength = SendMessage(got->second, WM_GETTEXTLENGTH, 0, 0);
                TCHAR* buffer = new TCHAR[textLength + 1];
                SendMessage(got->second, WM_GETTEXT, textLength + 1, reinterpret_cast<LPARAM>(buffer));
                std::wstring wtext(buffer);
                std::string text(wtext.begin(), wtext.end());
                std::string selected_text(buffer + selStart, buffer + selEnd);
                std::string::size_type i = text.find(selected_text);
                if (i != std::string::npos)
                    wtext.erase(i, selected_text.length());
                SetWindowText(got->second, wtext.c_str());
            }
            break;
        }
        }
#endif //!< deprecated.
        break;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        RECT rect;
        HDC memBufDC /* device context for primary drawing */,
            memDoubleBufDC /* device context for drawing bitmaps (sprutes)*/;
        HBITMAP memBM;

        HDC hdc = BeginPaint(hwnd, &ps);

        /*
        * Initialization
        */
        memBufDC = CreateCompatibleDC(hdc);
        memDoubleBufDC = CreateCompatibleDC(hdc);
        GetClientRect(hwnd, &rect);
        memBM = CreateCompatibleBitmap(hdc, rect.right - rect.left, rect.bottom - rect.top);
        SelectObject(memBufDC, memBM);
        FillRect(memBufDC, &rect, (HBRUSH)(COLOR_WINDOW + 1));
        /* end initialization */

        FillRect(hdc, &rect, hbrBack);
        static std::pair<int, int> buttonBoxSize = std::make_pair<int, int>(140, windowFrameHeight - 10 - 2 * SM_CYBORDER - GetSystemMetrics(SM_CYCAPTION) - GetSystemMetrics(SM_CYMENU) - 10);
        Rectangle(memBufDC, 0, 0, buttonBoxSize.first, buttonBoxSize.second);
        BLENDFUNCTION buttomBoxBlend;
        buttomBoxBlend.AlphaFormat = AC_SRC_ALPHA;
        buttomBoxBlend.BlendFlags = 0;
        buttomBoxBlend.BlendOp = AC_SRC_OVER;
        buttomBoxBlend.SourceConstantAlpha = 50;
        AlphaBlend(hdc, 10, 10, buttonBoxSize.first, buttonBoxSize.second,
            memBufDC, 0, 0, buttonBoxSize.first, buttonBoxSize.second, buttomBoxBlend);

        /*
        * Free allocated stuff for drawing
        */
        DeleteObject(memBM);
        DeleteDC(memBufDC);
        DeleteDC(memDoubleBufDC);
        /* end releasing*/

        EndPaint(hwnd, &ps);

        break;
    }
    case WM_COMMAND:
    {
        auto got = subWindowsHandlerFunctions.find(LOWORD(wParam));
        if (got != subWindowsHandlerFunctions.end()) {
            got->second();
            SetFocus(hwnd);
        }
        break;
    }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
