#include "pch.h"
#include "LogUtil.h"
#include <shlwapi.h>
#include <direct.h>
#include <ctime>
#include <string>

void LogMessage(const CString& msg)
{
    time_t t = time(nullptr);
    struct tm tmBuf;
#ifdef _WIN32
    localtime_s(&tmBuf, &t);
#else
    tmBuf = *localtime(&t);
#endif
    TCHAR exePath[MAX_PATH] = {0};
    GetModuleFileName(NULL, exePath, MAX_PATH);
    CString exeDir = exePath;
    exeDir = exeDir.Left(exeDir.ReverseFind(_T('\\')) + 1);
    CString logDir = exeDir + _T("logs\\");
    if (!PathFileExists(logDir)) _tmkdir(logDir);
    CString logFile;
    logFile.Format(_T("%s%04d-%02d-%02d.log"), logDir.GetString(),
        tmBuf.tm_year + 1900, tmBuf.tm_mon + 1, tmBuf.tm_mday);
    if (msg.IsEmpty()) return; // 如果消息为空，则不记录

    // 以二进制方式写入UTF-8
    FILE* fp = nullptr;
#ifdef _UNICODE
    _tfopen_s(&fp, logFile, _T("ab+"));
#else
    fopen_s(&fp, logFile, "ab+");
#endif
    if (fp)
    {
        // 检查文件是否为空，若为空则写入UTF-8 BOM
        fseek(fp, 0, SEEK_END);
        long fileSize = ftell(fp);
        if (fileSize == 0)
        {
            const unsigned char bom[] = {0xEF, 0xBB, 0xBF};
            fwrite(bom, 1, 3, fp);
        }

        CString timeStr;
        timeStr.Format(_T("[%02d:%02d:%02d] "), tmBuf.tm_hour, tmBuf.tm_min, tmBuf.tm_sec);
        CString line = timeStr + msg + _T("\r\n");

        // 转为UTF-8
        int utf8Len = WideCharToMultiByte(CP_UTF8, 0, line, -1, NULL, 0, NULL, NULL);
        std::string utf8Str(utf8Len - 1, 0); // 不要写入结尾的\0
        WideCharToMultiByte(CP_UTF8, 0, line, -1, &utf8Str[0], utf8Len, NULL, NULL);

        fwrite(utf8Str.c_str(), 1, utf8Str.size(), fp);
        fclose(fp);
    }
}
