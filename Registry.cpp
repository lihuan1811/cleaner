#include "pch.h"
#include "RegistryUtil.h"

BOOL RegistryUtil::ReadString(HKEY hRoot, const CString& subKey, const CString& valueName, CString& outValue)
{
    HKEY hKey;
    if (RegOpenKeyEx(hRoot, subKey, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
        return FALSE;
    TCHAR szBuffer[1024] = {0};
    DWORD dwSize = sizeof(szBuffer);
    LONG lRet = RegQueryValueEx(hKey, valueName, 0, NULL, (LPBYTE)szBuffer, &dwSize);
    RegCloseKey(hKey);
    if (lRet == ERROR_SUCCESS) {
        outValue = szBuffer;
        return TRUE;
    }
    return FALSE;
}

BOOL RegistryUtil::WriteString(HKEY hRoot, const CString& subKey, const CString& valueName, const CString& value)  
{  
    HKEY hKey;  
    if (RegCreateKeyEx(hRoot, subKey, 0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL) != ERROR_SUCCESS)  
        return FALSE;  

    // Fix for lnt-arithmetic-overflow: Ensure proper type casting and avoid overflow  
    size_t valueLength = static_cast<size_t>(value.GetLength() + 1) * sizeof(TCHAR);  

    // Fix for E1087 and C2663: Use GetBuffer() correctly with non-const CString  
    CString tempValue = value;  
    LONG lRet = RegSetValueEx(hKey, valueName, 0, REG_SZ, reinterpret_cast<const BYTE*>(tempValue.GetBuffer()), static_cast<DWORD>(valueLength));  
    tempValue.ReleaseBuffer();  

    RegCloseKey(hKey);  
    return lRet == ERROR_SUCCESS;  
}

BOOL RegistryUtil::DeleteValue(HKEY hRoot, const CString& subKey, const CString& valueName)
{
    HKEY hKey;
    if (RegOpenKeyEx(hRoot, subKey, 0, KEY_SET_VALUE, &hKey) != ERROR_SUCCESS)
        return FALSE;
    LONG lRet = RegDeleteValue(hKey, valueName);
    RegCloseKey(hKey);
    return lRet == ERROR_SUCCESS;
}

BOOL RegistryUtil::DeleteKey(HKEY hRoot, const CString& subKey)
{
    return RegDeleteKey(hRoot, subKey) == ERROR_SUCCESS;
}