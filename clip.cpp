#include <iostream>
#include <windows.h>
#include <shlobj.h>

bool CopyFileToClipboard(const std::string& filePath);

int main(int argc, char* argv[])
{
    std::string filePath = argv[1];
    bool success = CopyFileToClipboard(filePath);
    if (success) {
        std::cout << "File copied to clipboard." << std::endl;
    } else {
        std::cout << "Failed to copy file to clipboard." << std::endl;
    }
    return 0;
}

bool CopyFileToClipboard(const std::string& filePath)
{
    // Register a custom clipboard format for the drop effect (copy/cut).
    UINT uDropEffect = RegisterClipboardFormat("Preferred DropEffect");

    // Allocate memory for the drop effect data and set it to "copy".
    HGLOBAL hGblEffect = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE | GMEM_DDESHARE, sizeof(DWORD));
    LPDWORD lpdDropEffect = (LPDWORD)GlobalLock(hGblEffect);
    *lpdDropEffect = DROPEFFECT_COPY; // use DROPEFFECT_MOVE for cut
    GlobalUnlock(hGblEffect);

    // Allocate memory for the file list and populate it with the file path.
    HGLOBAL hGblFiles = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE | GMEM_DDESHARE,
        sizeof(DROPFILES) + filePath.size() + 2);
    LPSTR lpData = (LPSTR)GlobalLock(hGblFiles);
    DROPFILES* pDropFiles = (DROPFILES*)lpData;
    pDropFiles->pFiles = sizeof(DROPFILES);
    pDropFiles->pt.x = 0;
    pDropFiles->pt.y = 0;
    pDropFiles->fNC = FALSE;
    pDropFiles->fWide = FALSE;
    strcpy(lpData + sizeof(DROPFILES), filePath.c_str());
    GlobalUnlock(hGblFiles);

    // Open the clipboard and clear any existing contents.
    if (!OpenClipboard(NULL)) {
        return false;
    }
    EmptyClipboard();

    // Set the file list and drop effect data on the clipboard.
    if (!SetClipboardData(CF_HDROP, hGblFiles) || !SetClipboardData(uDropEffect, hGblEffect)) {
        CloseClipboard();
        return false;
    }

    // Close the clipboard and return success.
    CloseClipboard();
    return true;
}
