#include <iostream>
#include <windows.h>

#include "MainWindow.hpp"

#pragma comment( compiler )
#pragma comment( user, "Compiled on " __DATE__ " at " __TIME__ )

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
#if 0
    key* key = dump_pack(strlen("key"), (unsigned char*)"key", 1);
    value* value1 = dump_pack(strlen("value"), (unsigned char*)"value", 1);
    mdlIBTP.fileMappingRealization.main.set(&kv, key, value1);

    value* val = mdlIBTP.fileMappingRealization.main.get(&kv, key);
    printf("%.*s\n", (unsigned int)val->size, val->dump);
    dump_delete_fr(val);
    dump_delete_fr(key);
    dump_delete_fr(value1);
#endif

    auto& window = MainWindow::getMainWindow(hInstance, L"IBTPBrowserClass", L"Immutable BTree+ browser", 700, 400);

    return window.run(nCmdShow);
}