#include "FolderPicker.h"

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <shobjidl.h>
#include <filesystem>
#endif

namespace WorldMaker::ui {

bool PickExportFolder(GLFWwindow* owner, std::string& folder, std::string& error) {
#ifdef _WIN32
    const HRESULT initialized = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (FAILED(initialized)) {
        error = "Could not initialize the folder picker.";
        return false;
    }

    IFileOpenDialog* dialog = nullptr;
    HRESULT result = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER,
                                     IID_IFileOpenDialog, reinterpret_cast<void**>(&dialog));
    if (SUCCEEDED(result)) {
        FILEOPENDIALOGOPTIONS options;
        result = dialog->GetOptions(&options);
        if (SUCCEEDED(result)) result = dialog->SetOptions(options | FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM | FOS_PATHMUSTEXIST | FOS_NOCHANGEDIR);
        if (SUCCEEDED(result)) result = dialog->SetTitle(L"Choose export folder");

        // Open at the currently entered destination when it is a valid folder.
        std::error_code path_error;
        const auto initial = std::filesystem::absolute(std::filesystem::u8path(folder), path_error);
        if (!path_error) {
            IShellItem* initial_item = nullptr;
            if (SUCCEEDED(SHCreateItemFromParsingName(initial.c_str(), nullptr, IID_IShellItem,
                                                     reinterpret_cast<void**>(&initial_item)))) {
                dialog->SetFolder(initial_item);
                initial_item->Release();
            }
        }

        if (SUCCEEDED(result)) result = dialog->Show(owner ? glfwGetWin32Window(owner) : nullptr);
        if (SUCCEEDED(result)) {
            IShellItem* item = nullptr;
            result = dialog->GetResult(&item);
            if (SUCCEEDED(result)) {
                PWSTR path = nullptr;
                result = item->GetDisplayName(SIGDN_FILESYSPATH, &path);
                if (SUCCEEDED(result)) {
                    const auto utf8 = std::filesystem::path(path).u8string();
                    folder.assign(utf8.begin(), utf8.end());
                    CoTaskMemFree(path);
                }
                item->Release();
            }
        }
        dialog->Release();
    }
    CoUninitialize();
    if (FAILED(result) && result != HRESULT_FROM_WIN32(ERROR_CANCELLED))
        error = "Could not open or read the selected folder.";
    return SUCCEEDED(result);
#else
    error = "The native folder picker is currently available on Windows. Enter the folder path manually.";
    return false;
#endif
}

}
