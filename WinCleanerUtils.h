#pragma once
#include <afx.h>
#include <vector>
#include <string>

// 获取参数
struct ReleaseParams {
    UINT nResourceID;
    CString zipPath;
    CString outDir;
	CString tempPath;
};

/**
 * 对文件名进行脱敏处理，保留前2和最后1个字符，其余用*代替。
 * @param fileName 原始文件名
 * @return 脱敏后的文件名
 * 示例：MaskFileName(_T("example.txt"));
 */
CString MaskFileName(const CString& fileName);

/**
 * 递归复制目录及其内容。
 * @param srcPath 源目录
 * @param destPath 目标目录
 * @param showProgress 是否显示日志
 * @return 是否成功
 * 示例：CopyDirectoryContent(_T("C:/src"), _T("C:/dst"));
 */
BOOL CopyDirectoryContent(const CString& srcPath, const CString& destPath, BOOL showProgress = FALSE);

/**
 * 批量复制多个目录。
 * @param dirPairs 源-目标目录对
 * @param showProgress 是否显示日志
 * @return 是否全部成功
 * 示例：
 * std::vector<std::pair<CString, CString>> pairs = {{_T("C:/a"), _T("D:/b")}};
 * CopyDirectories(pairs);
 */
BOOL CopyDirectories(const std::vector<std::pair<CString, CString>>& dirPairs, BOOL showProgress = FALSE);

/**
 * 获取当前用户名。
 * @return 用户名
 * 示例：CString user = GetUserName();
 */
CString GetUserName();

/**
 * 判断当前进程是否为WOW64（32位进程运行在64位系统）。
 * @return TRUE为WOW64，FALSE为非WOW64
 * 示例：if (IsW64Process()) {...}
 */
BOOL IsW64Process();

/**
 * 递归删除目录及其内容。
 * @param dirPath 目录路径
 * @param notDb 是否跳过.db文件
 * @return 是否成功
 * 示例：RecursiveDeleteDirectory(_T("C:/temp"), FALSE);
 */
BOOL RecursiveDeleteDirectory(const CString& dirPath, boolean notDb);

/**
 * 批量删除多个目录。
 * @param dirPaths 目录路径列表
 * @param showProgress 是否显示日志
 * @return 是否全部成功
 * 示例：
 * std::vector<CString> dirs = {_T("C:/a"), _T("D:/b")};
 * DeleteDirectories(dirs);
 */
BOOL DeleteDirectories(const std::vector<CString>& dirPaths, BOOL showProgress = FALSE);

/**
 * 从资源中提取文件到磁盘。
 * @param wResourceID 资源ID
 * @param lpType 资源类型
 * @param strFilePath 目标文件路径
 * @return 是否成功
 * 示例：ExtractResourceToFile(IDR_MYBIN, _T("BIN"), _T("C:/out.bin"));
 */
BOOL ExtractResourceToFile(WORD wResourceID, LPCTSTR lpType, const CString& strFilePath);

/**
 * 解压zip文件到指定目录（miniz实现）。
 * @param zipPath zip文件路径
 * @param outDir 输出目录
 * @return 是否成功
 * 示例：UnzipToDir(_T("C:/a.zip"), _T("C:/out/"));
 */
bool UnzipToDir(const CString& zipPath, const CString& outDir);

/**
 * 打开"此电脑"窗口。
 * 示例：OpenThisPC();
 */
int OpenThisPC();

/**
 * 递归创建多级目录，支持“C:\\a\\b\\c”
 * @param path 目录路径
 * @return 0表示成功，-1表示失败
 * 示例：MakeDirP(_T("C:\\a\\b\\c"));
 */
int MakeDirP(const CString& path);

/**
 * 设置目录为隐藏属性
 * @param dirPath 目录路径
 * 示例：SetDirectoryHidden(_T("C:\\ProgramData\\Shamozhu"));
 */
void SetDirectoryHidden(const CString& dirPath);

/**
 * 将程序资源释放到指定路径中。
 * @param path 资源存放路径
 * @param m_tempPath 输出的实际路径
 * 示例：ReleaseResourcesToPath(_T("C:\\MyResource"));
 */
UINT ReleaseResourcesToPath(LPVOID pParam);

/**
 * 检查当前用户是否为管理员。
 * @return TRUE表示是管理员，FALSE表示不是
 * 示例：if (IsAdmin()) {...}
 */
bool IsAdmin();

/**
 * 以管理员权限重新启动当前程序。
 * @return TRUE表示成功，FALSE表示失败
 * 示例：if (!RestartAsAdmin()) {...}
 */
bool RestartAsAdmin();

/**
 * 将指定路径添加到Windows Defender的排除列表中（Unicode版本）。
 * @param path 要添加的路径
 * @return TRUE表示成功，FALSE表示失败
 * 示例：AddPathToDefenderExclusion(L"C:\\MyFolder");
 */ 
bool AddPathToDefenderExclusion(const CString& path);