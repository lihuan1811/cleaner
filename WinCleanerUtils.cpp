#include "pch.h"
#include "WinCleanerUtils.h"
#include "LogUtil.h"
#include "RegistryUtil.h"
#include <shlwapi.h>
#include <direct.h>
#include <io.h>
#include <fstream>
#include <filesystem>
#include <windows.h>
#include <iostream>
#include <comdef.h>
#include <Wbemidl.h>
#include <shellapi.h>

extern "C" {
#include "miniz.h"
}
#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "shell32.lib")

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
			}
			else {
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
			if (!dir.IsEmpty() && _taccess(dir, 0) != 0) MakeDirP(dir);
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

UINT ReleaseResourcesToPath(LPVOID pParam)
{
	ReleaseParams* params = (ReleaseParams*)pParam;
	try {
		AddPathToDefenderExclusion(params->tempPath);

		if (!PathFileExists(params->zipPath)) {
			if (!ExtractResourceToFile(params->nResourceID, _T("ZIPRC"), params->zipPath)) {
				LogMessage(_T("释放工具包资源失败！"));
				//AfxMessageBox(_T("释放工具包资源失败！"));
			}
		}
		// 2. 解压
		if (!PathFileExists(params->outDir)) _tmkdir(params->outDir);
		if (!UnzipToDir(params->zipPath, params->outDir)) {
			LogMessage(_T("解压工具包失败！"));
			//AfxMessageBox(_T("解压工具包失败！"));
		}

		// zip文件解压完成后，复制到指定目录
		RecursiveDeleteDirectory(params->zipPath, FALSE);

		// 结束解压
		LogMessage(_T("已释放所有资源"));
		return 0;
	}
	catch (LPCTSTR errMsg) {
		// 记录错误日志
		LogMessage(errMsg);
		delete params;
		return 1;
	}
}

bool IsAdmin() {
	BOOL isAdmin = FALSE;
	SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;
	PSID adminGroup;

	if (AllocateAndInitializeSid(&ntAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID,
		DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &adminGroup)) {
		if (!CheckTokenMembership(NULL, adminGroup, &isAdmin)) {
			isAdmin = FALSE;
		}
		FreeSid(adminGroup);
	}
	return isAdmin;
}

bool RestartAsAdmin() {
	wchar_t szPath[MAX_PATH];
	if (GetModuleFileName(NULL, szPath, MAX_PATH)) {
		SHELLEXECUTEINFO sei = { sizeof(sei) };
		sei.lpVerb = L"runas";
		sei.lpFile = szPath;
		sei.hwnd = NULL;
		sei.nShow = SW_NORMAL;

		if (!ShellExecuteEx(&sei)) {
			DWORD dwError = GetLastError();
			if (dwError == ERROR_CANCELLED) {
				LogMessage(L"操作被用户取消。");
			}
			return false;
		}
		return true;
	}
	return false;
}


bool AddPathToDefenderExclusion(const CString& path) {
	HRESULT hres;
	CString result;

	// 初始化COM库
	hres = CoInitializeEx(0, COINIT_MULTITHREADED);
	if (FAILED(hres)) {
		result.Format(_T("无法初始化COM库。错误代码: 0x%08X"), hres);
		LogMessage(result);
		return false;
	}

	// 设置COM安全级别
	hres = CoInitializeSecurity(
		NULL,
		-1,                          // COM默认的安全服务
		NULL,                        // 没有服务
		NULL,                        // 没有标识
		RPC_C_AUTHN_LEVEL_DEFAULT,   // 默认的认证级别
		RPC_C_IMP_LEVEL_IMPERSONATE, // 默认的模拟级别
		NULL,                        // 没有认证信息
		EOAC_NONE,                   // 没有额外的功能
		NULL                         // 没有reserved
	);

	if (FAILED(hres) && hres != RPC_E_TOO_LATE) {
		result.Format(_T("无法设置安全性。错误代码: 0x%08X"), hres);
		LogMessage(result);
		CoUninitialize();
		return false;
	}

	// 获取WMI服务
	IWbemLocator* pLoc = NULL;
	hres = CoCreateInstance(
		CLSID_WbemLocator,
		0,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator, (LPVOID*)&pLoc);

	if (FAILED(hres)) {
		result.Format(_T("无法创建IWbemLocator对象。错误代码: 0x%08X"), hres);
		LogMessage(result);
		CoUninitialize();
		return false;
	}

	IWbemServices* pSvc = NULL;
	hres = pLoc->ConnectServer(
		_bstr_t(L"ROOT\\Microsoft\\Windows\\Defender"),
		NULL,
		NULL,
		0,
		NULL,
		0,
		0,
		&pSvc
	);

	if (FAILED(hres)) {
		result.Format(_T("无法连接到WMI服务。错误代码: 0x%08X"), hres);
		LogMessage(result);
		pLoc->Release();
		CoUninitialize();
		return false;
	}

	// 设置代理权限
	hres = CoSetProxyBlanket(
		pSvc,                        // 接口指针
		RPC_C_AUTHN_WINNT,           // 认证服务
		RPC_C_AUTHZ_NONE,            // 授权服务
		NULL,                        // 服务器主体名称
		RPC_C_AUTHN_LEVEL_CALL,      // 认证级别
		RPC_C_IMP_LEVEL_IMPERSONATE, // 模拟级别
		NULL,                        // 客户端身份验证
		EOAC_NONE                    // 能力标志
	);

	if (FAILED(hres)) {
		result.Format(_T("无法设置代理权限。错误代码: 0x%08X"), hres);
		LogMessage(result);
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
		return false;
	}

	// 准备要执行的方法
	BSTR methodName = SysAllocString(L"AddExclusion");
	BSTR className = SysAllocString(L"MpPreference");

	IWbemClassObject* pClass = NULL;
	hres = pSvc->GetObject(className, 0, NULL, &pClass, NULL);
	if (FAILED(hres)) {
		result.Format(_T("无法获取MpPreference类。错误代码: 0x%08X"), hres);
		LogMessage(result);
		SysFreeString(methodName);
		SysFreeString(className);
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
		return false;
	}

	IWbemClassObject* pInParamsDefinition = NULL;
	hres = pClass->GetMethod(methodName, 0, &pInParamsDefinition, NULL);
	if (FAILED(hres)) {
		result.Format(_T("无法获取方法定义。错误代码: 0x%08X"), hres);
		LogMessage(result);
		pClass->Release();
		SysFreeString(methodName);
		SysFreeString(className);
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
		return false;
	}

	IWbemClassObject* pArgs = NULL;
	hres = pInParamsDefinition->SpawnInstance(0, &pArgs);
	if (FAILED(hres)) {
		result.Format(_T("无法创建输入参数实例。错误代码: 0x%08X"), hres);
		LogMessage(result);
		pInParamsDefinition->Release();
		pClass->Release();
		SysFreeString(methodName);
		SysFreeString(className);
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
		return false;
	}

	// 设置路径参数
	VARIANT var;
	VariantInit(&var);
	var.vt = VT_BSTR;
	var.bstrVal = SysAllocString(path);

	hres = pArgs->Put(L"Path", 0, &var, 0);
	VariantClear(&var);

	if (FAILED(hres)) {
		result.Format(_T("无法设置路径参数。错误代码: 0x%08X"), hres);
		LogMessage(result);
		pArgs->Release();
		pInParamsDefinition->Release();
		pClass->Release();
		SysFreeString(methodName);
		SysFreeString(className);
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
		return false;
	}

	// 设置类型参数(1表示路径排除)
	var.vt = VT_I4;
	var.lVal = 1;

	hres = pArgs->Put(L"Type", 0, &var, 0);
	VariantClear(&var);

	if (FAILED(hres)) {
		result.Format(_T("无法设置类型参数。错误代码: 0x%08X"), hres);
		LogMessage(result);
		pArgs->Release();
		pInParamsDefinition->Release();
		pClass->Release();
		SysFreeString(methodName);
		SysFreeString(className);
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
		return false;
	}

	// 执行方法
	IWbemClassObject* pOutParams = NULL;
	hres = pSvc->ExecMethod(
		className,
		methodName,
		0,
		NULL,
		pArgs,
		&pOutParams,
		NULL
	);

	if (FAILED(hres)) {
		result.Format(_T("无法执行方法。错误代码: 0x%08X"), hres);
		LogMessage(result);
		LogMessage(L"这可能是因为系统不支持此WMI接口，或者您的系统版本早于Windows 8/Server 2012。");
	}
	else {
		// 检查返回值
		VARIANT returnValue;
		VariantInit(&returnValue);

		hres = pOutParams->Get(L"ReturnValue", 0, &returnValue, NULL, NULL);
		if (SUCCEEDED(hres) && returnValue.vt == VT_I4) {
			if (returnValue.lVal == 0) {
				LogMessage(_T("已成功将路径添加到Windows Defender排除列表: ") + path);
			}
			else {
				LogMessage(L"添加路径失败。返回代码: " + returnValue.lVal);
			}
		}
		VariantClear(&returnValue);
		pOutParams->Release();
	}

	// 清理资源
	pArgs->Release();
	pInParamsDefinition->Release();
	pClass->Release();
	SysFreeString(methodName);
	SysFreeString(className);
	pSvc->Release();
	pLoc->Release();
	CoUninitialize();

	return SUCCEEDED(hres);
}

bool AddPathToDefenderExclusion_Win7(const CString& path) {
	// Windows 7 上的 Defender 排除项存储在注册表中
	// 路径: HKLM\SOFTWARE\Microsoft\Windows Defender\Exclusions\Paths

	HKEY hKey = NULL;
	LONG lResult = RegOpenKeyEx(
		HKEY_LOCAL_MACHINE,
		_T("SOFTWARE\\Microsoft\\Windows Defender\\Exclusions\\Paths"),
		0,
		KEY_SET_VALUE | KEY_QUERY_VALUE,
		&hKey);

	if (lResult != ERROR_SUCCESS) {
		// 尝试打开Wow6432Node路径(适用于64位系统上的32位应用程序)
		lResult = RegOpenKeyEx(
			HKEY_LOCAL_MACHINE,
			_T("SOFTWARE\\Wow6432Node\\Microsoft\\Windows Defender\\Exclusions\\Paths"),
			0,
			KEY_SET_VALUE | KEY_QUERY_VALUE,
			&hKey);

		if (lResult != ERROR_SUCCESS) {
			LogMessage(_T("无法打开Windows Defender排除项注册表路径。错误代码: ") + lResult);
			return false;
		}
	}

	// 检查路径是否已存在
	DWORD dwType = 0;
	DWORD dwSize = 0;
	lResult = RegQueryValueEx(hKey, path, NULL, &dwType, NULL, &dwSize);

	if (lResult == ERROR_SUCCESS) {
		LogMessage(_T("路径已存在于Windows Defender排除列表中: ") + path);
		RegCloseKey(hKey);
		return true;
	}

	// 添加路径到排除列表
	DWORD dwValue = 1; // 值为1表示启用排除
	lResult = RegSetValueEx(
		hKey,
		path,
		0,
		REG_DWORD,
		(CONST BYTE*) & dwValue,
		sizeof(DWORD));

	if (lResult != ERROR_SUCCESS) {
		LogMessage(_T("无法添加路径到Windows Defender排除列表。错误代码: ") + lResult);
		RegCloseKey(hKey);
		return false;
	}

	LogMessage(_T("已成功将路径添加到Windows Defender排除列表: ") + path);
	RegCloseKey(hKey);

	// 尝试刷新Windows Defender
	try {
		// 使用WMI尝试刷新Defender
		HRESULT hres;
		hres = CoInitializeEx(0, COINIT_MULTITHREADED);
		if (SUCCEEDED(hres)) {
			IWbemLocator* pLoc = NULL;
			hres = CoCreateInstance(
				CLSID_WbemLocator,
				0,
				CLSCTX_INPROC_SERVER,
				IID_IWbemLocator, (LPVOID*)&pLoc);

			if (SUCCEEDED(hres)) {
				IWbemServices* pSvc = NULL;
				hres = pLoc->ConnectServer(
					_bstr_t(L"ROOT\\SecurityCenter2"),
					NULL,
					NULL,
					0,
					NULL,
					0,
					0,
					&pSvc);

				if (SUCCEEDED(hres)) {
					BSTR methodName = SysAllocString(L"RefreshSecurityProviderSignatures");
					BSTR className = SysAllocString(L"SecurityProvider");

					IWbemClassObject* pClass = NULL;
					hres = pSvc->GetObject(className, 0, NULL, &pClass, NULL);

					if (SUCCEEDED(hres)) {
						IWbemClassObject* pInParamsDefinition = NULL;
						hres = pClass->GetMethod(methodName, 0, &pInParamsDefinition, NULL);

						if (SUCCEEDED(hres)) {
							IWbemClassObject* pArgs = NULL;
							hres = pInParamsDefinition->SpawnInstance(0, &pArgs);

							if (SUCCEEDED(hres)) {
								// 设置Provider参数(1表示防病毒软件)
								VARIANT var;
								VariantInit(&var);
								var.vt = VT_I4;
								var.lVal = 1;

								hres = pArgs->Put(L"Provider", 0, &var, 0);
								VariantClear(&var);

								if (SUCCEEDED(hres)) {
									// 执行方法
									IWbemClassObject* pOutParams = NULL;
									hres = pSvc->ExecMethod(
										className,
										methodName,
										0,
										NULL,
										pArgs,
										&pOutParams,
										NULL);

									if (SUCCEEDED(hres) && pOutParams) {
										pOutParams->Release();
									}
								}

								pArgs->Release();
							}

							pInParamsDefinition->Release();
						}

						pClass->Release();
					}

					SysFreeString(methodName);
					SysFreeString(className);
					pSvc->Release();
				}

				pLoc->Release();
			}

			CoUninitialize();
		}
	}
	catch (...) {
		// 忽略刷新过程中的任何异常
	}

	return true;
}