#include "pch.h"
#include "WinCleanerUtils.h"
#include "LogUtil.h"
#include "RegistryUtil.h"
#include <shlwapi.h>
#include <direct.h>
#include <io.h>
#include <fstream>
#include <filesystem>
extern "C" {
#include "miniz.h"
}

CString MaskFileName(const CString& fileName)
{
    try {
        if (fileName.GetLength() <= 3) {
            return fileName;
        }
        int dotPos = fileName.ReverseFind(_T('.'));
        if (dotPos == -1) {
            dotPos = fileName.GetLength();
        }
        CString nameWithoutExt = fileName.Left(dotPos);
        CString extension = (dotPos < fileName.GetLength()) ? fileName.Mid(dotPos) : _T("");
        if (nameWithoutExt.GetLength() <= 3) {
            return fileName;
        }
        CString maskedName = nameWithoutExt.Left(2);
        int starCount = nameWithoutExt.GetLength() - 3;
        maskedName += CString(_T('*'), starCount);
        maskedName += nameWithoutExt.Right(1);
        maskedName += extension;
        return maskedName;
    }
    catch (...) {
        return fileName;
    }
}

BOOL CopyDirectoryContent(const CString& srcPath, const CString& destPath, BOOL showProgress)
{
    try {
        if (!PathFileExists(srcPath)) {
            LogMessage(_T("源目录不存在：") + srcPath);
            return FALSE;
        }
        if (!PathFileExists(destPath)) {
            if (!CreateDirectory(destPath, NULL)) {
                LogMessage(_T("无法创建目标目录：") + destPath);
                return FALSE;
            }
        }
        CString searchPath = srcPath;
        if (searchPath.Right(1) != _T("\\")) {
            searchPath += _T("\\");
        }
        searchPath += _T("*");
        WIN32_FIND_DATA findData;
        HANDLE hFind = FindFirstFile(searchPath, &findData);
        if (hFind == INVALID_HANDLE_VALUE) {
            LogMessage(_T("无法访问目录：") + srcPath);
            return FALSE;
        }
        BOOL success = TRUE;
        do {
            if (_tcscmp(findData.cFileName, _T(".")) == 0 ||
                _tcscmp(findData.cFileName, _T("..")) == 0) {
                continue;
            }
            CString srcFile = srcPath;
            CString destFile = destPath;
            if (srcFile.Right(1) != _T("\\")) srcFile += _T("\\");
            if (destFile.Right(1) != _T("\\")) destFile += _T("\\");
            srcFile += findData.cFileName;
            destFile += findData.cFileName;
            if (showProgress) {
                LogMessage(_T("正在复制：") + srcFile);
            }
            if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                if (!CopyDirectoryContent(srcFile, destFile, showProgress)) {
                    success = FALSE;
                }
            }
            else {
                if (!CopyFile(srcFile, destFile, FALSE)) {
                    LogMessage(_T("复制文件失败：") + srcFile);
                    success = FALSE;
                }
            }
        } while (FindNextFile(hFind, &findData));
        FindClose(hFind);
        return success;
    }
    catch (const std::exception& e) {
        LogMessage(_T("复制目录时发生异常：") + srcPath + _T(" 错误：") + CString(e.what()));
        return FALSE;
    }
}

BOOL CopyDirectories(const std::vector<std::pair<CString, CString>>& dirPairs, BOOL showProgress)
{
    BOOL result = TRUE;
    int total = dirPairs.size();
    int current = 0;
    for (const auto& pair : dirPairs) {
        try {
            current++;
            const CString& srcPath = pair.first;
            const CString& destPath = pair.second;
            if (srcPath.IsEmpty() || destPath.IsEmpty()) continue;
            CString progressMsg;
            progressMsg.Format(_T("正在处理 %d/%d: %s -> %s"),
                current, total, srcPath, destPath);
            if (showProgress) {
                LogMessage(progressMsg);
            }
            if (!PathFileExists(srcPath)) {
                LogMessage(_T("源目录不存在: ") + srcPath);
                result = FALSE;
                continue;
            }
            if (!CopyDirectoryContent(srcPath, destPath, showProgress)) {
                LogMessage(_T("复制目录失败: ") + srcPath + _T(" -> ") + destPath);
                result = FALSE;
            }
            else {
                LogMessage(_T("成功复制目录: ") + srcPath + _T(" -> ") + destPath);
            }
        }
        catch (const std::exception& e) {
            LogMessage(_T("复制目录时发生异常: ") + pair.first + _T(" 错误: ") + CString(e.what()));
            result = FALSE;
        }
    }
    return result;
}

CString GetUserName()
{
    TCHAR userName[MAX_PATH];
    DWORD size = MAX_PATH;
    if (GetUserName(userName, &size))
    {
        return CString(userName);
    }
    return _T("");
}

BOOL IsW64Process()
{
    typedef BOOL(WINAPI* LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
    LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)
        GetProcAddress(GetModuleHandle(_T("kernel32")), "IsWow64Process");
    BOOL bIsWow64 = FALSE;
    if (fnIsWow64Process != NULL)
    {
        if (!fnIsWow64Process(GetCurrentProcess(), &bIsWow64))
        {
            return FALSE;
        }
    }
    return bIsWow64;
}

BOOL RecursiveDeleteDirectory(const CString& dirPath, boolean notDb)
{
    try {
        if (dirPath.IsEmpty())
        {
            return FALSE;
        }
        HANDLE hFind;
        WIN32_FIND_DATA findData;
        CString wstrTempDir = dirPath + _T("\\*");;
        hFind = FindFirstFile(wstrTempDir, &findData);
        if (hFind == INVALID_HANDLE_VALUE)
        {
            return FALSE;
        }
        do
        {
            if (wcscmp(findData.cFileName, L".") == 0 || wcscmp(findData.cFileName, L"..") == 0)
            {
                continue;
            }
            if (notDb) {
                CString fileName = findData.cFileName;
                if (fileName.GetLength() > 3 && fileName.Right(3).CompareNoCase(_T(".db")) == 0) {
                    LogMessage(_T("跳过数据库文件：") + fileName);
                    continue;
                }
            }
            std::wstring wstrFileName;
            wstrFileName.assign(dirPath);
            wstrFileName.append(L"\\");
            wstrFileName.append(findData.cFileName);
            if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                RecursiveDeleteDirectory(wstrFileName.c_str(), FALSE);
            }
            else
            {
                DeleteFile(wstrFileName.c_str());
            }
        } while (FindNextFile(hFind, &findData));
        FindClose(hFind);
        RemoveDirectory(dirPath);
        return TRUE;
    }
    catch (const std::exception& e) {
        LogMessage(_T("删除目录时发生异常：") + dirPath + _T(" 异常：") + CString(e.what()));
        return FALSE;
    }
}

BOOL DeleteDirectories(const std::vector<CString>& dirPaths, BOOL showProgress)
{
    BOOL result = TRUE;
    int total = dirPaths.size();
    int current = 0;
    for (const auto& path : dirPaths) {
        if (path.IsEmpty()) continue;
        try {
            current++;
            CString progressMsg;
            progressMsg.Format(_T("正在处理 %d/%d: %s"), current, total, path);
            if (showProgress) {
                LogMessage(progressMsg);
            }
            if (!PathFileExists(path)) {
                LogMessage(_T("目录不存在: ") + path);
                continue;
            }
            if (RecursiveDeleteDirectory(path, TRUE)) {
                LogMessage(_T("成功删除目录: ") + path);
            } else {
                LogMessage(_T("删除目录失败: ") + path);
                result = FALSE;
            }
        }
        catch (const std::exception& e) {
            LogMessage(_T("删除目录时发生异常: ") + path + _T(" 错误: ") + CString(e.what()));
            result = FALSE;
        }
    }
    return result;
}

BOOL ExtractResourceToFile(WORD wResourceID, LPCTSTR lpType, const CString& strFilePath)
{
    HRSRC hRes = FindResource(AfxGetResourceHandle(), MAKEINTRESOURCE(wResourceID), lpType);
    if (!hRes) { LogMessage(_T("FindResource failed.")); return FALSE; }
    HGLOBAL hData = LoadResource(AfxGetResourceHandle(), hRes);
    if (!hData) { LogMessage(_T("LoadResource failed.")); return FALSE; }
    DWORD dwSize = SizeofResource(AfxGetResourceHandle(), hRes);
    LPVOID lpBytes = LockResource(hData);
    if (!lpBytes) { LogMessage(_T("LockResource failed.")); return FALSE; }
    CFile file;
    if (!file.Open(strFilePath, CFile::modeCreate | CFile::modeWrite | CFile::shareDenyWrite)) {
        LogMessage(_T("Open file failed: ") + strFilePath);
        return FALSE;
    }
    file.Write(lpBytes, dwSize);
    file.Close();
    LogMessage(_T("Extracted resource to ") + strFilePath);
    return TRUE;
}

bool UnzipToDir(const CString& zipPath, const CString& outDir)
{
    mz_zip_archive zip;
    memset(&zip, 0, sizeof(zip));
    if (!mz_zip_reader_init_file(&zip, CT2A(zipPath), 0))
        return false;
    mz_uint fileCount = mz_zip_reader_get_num_files(&zip);
    for (mz_uint i = 0; i < fileCount; ++i) {
        mz_zip_archive_file_stat stat;
        if (!mz_zip_reader_file_stat(&zip, i, &stat)) continue;
        int wlen = MultiByteToWideChar(CP_UTF8, 0, stat.m_filename, -1, NULL, 0);
        CString fileName;
        if (wlen > 0) {
            wchar_t* wbuf = fileName.GetBuffer(wlen);
            MultiByteToWideChar(CP_UTF8, 0, stat.m_filename, -1, wbuf, wlen);
            fileName.ReleaseBuffer();
        }
        else {
            fileName = L"";
        }
        CString outPath = outDir + fileName;
        if (stat.m_is_directory) {
            _tmkdir(outPath);
        }
        else {
            CString dir = outPath.Left(outPath.ReverseFind(_T('\\')));
            if (!dir.IsEmpty() && _taccess(dir, 0) != 0) _tmkdir(dir);
            int mblen = WideCharToMultiByte(CP_UTF8, 0, outPath, -1, NULL, 0, NULL, NULL);
            std::string mbPath(mblen, 0);
            WideCharToMultiByte(CP_UTF8, 0, outPath, -1, &mbPath[0], mblen, NULL, NULL);
            mz_zip_reader_extract_to_file(&zip, i, mbPath.c_str(), 0);
        }
    }
    mz_zip_reader_end(&zip);
    return true;
}

int OpenThisPC()
{
    // 打开“此电脑”，兼容Win7/Win10/Win11
    ShellExecute(NULL, _T("open"), _T("explorer.exe"),
        _T("::{20D04FE0-3AEA-1069-A2D8-08002B30309D}"),
        NULL, SW_SHOWNORMAL);
	return 0;
}

int MakeDirP(const CString& path)
{
    CString dir = path;
    dir.Replace('/', '\\');
    if (dir.IsEmpty()) return -1;

    // 如果已存在则直接返回
    if (PathFileExists(dir)) return 0;

    // 递归创建父目录
    int pos = dir.ReverseFind('\\');
    if (pos > 2) // 跳过“C:\”
    {
        CString parent = dir.Left(pos);
        if (!PathFileExists(parent))
        {
            if (MakeDirP(parent) != 0)
                return -1;
        }
    }
    // 创建当前目录
    if (_tmkdir(dir) != 0 && errno != EEXIST)
        return -1;
    return 0;
}

// 设置目录为隐藏属性
void SetDirectoryHidden(const CString& dirPath)
{
    DWORD attrs = GetFileAttributes(dirPath);
    if (attrs == INVALID_FILE_ATTRIBUTES) return;
    if (!(attrs & FILE_ATTRIBUTE_HIDDEN)) {
        SetFileAttributes(dirPath, attrs | FILE_ATTRIBUTE_HIDDEN);
    }
}
