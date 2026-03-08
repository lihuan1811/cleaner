#pragma once
#include <afx.h>

/**
 * 注册表操作工具类，提供常用的读写删方法。
 */
class RegistryUtil
{
public:
    /**
     * 读取注册表字符串值。
     * @param hRoot 根键（如HKEY_LOCAL_MACHINE）
     * @param subKey 子键路径
     * @param valueName 值名称
     * @param outValue 输出读取到的字符串
     * @return 是否成功
     * 示例：
     * CString value;
     * RegistryUtil::ReadString(HKEY_CURRENT_USER, _T("Software\\MyApp"), _T("Name"), value);
     */
    static BOOL ReadString(HKEY hRoot, const CString& subKey, const CString& valueName, CString& outValue);

    /**
     * 写入注册表字符串值。
     * @param hRoot 根键
     * @param subKey 子键路径
     * @param valueName 值名称
     * @param value 要写入的字符串
     * @return 是否成功
     * 示例：
     * RegistryUtil::WriteString(HKEY_CURRENT_USER, _T("Software\\MyApp"), _T("Name"), _T("Test"));
     */
    static BOOL WriteString(HKEY hRoot, const CString& subKey, const CString& valueName, const CString& value);

    /**
     * 删除注册表值。
     * @param hRoot 根键
     * @param subKey 子键路径
     * @param valueName 值名称
     * @return 是否成功
     * 示例：
     * RegistryUtil::DeleteValue(HKEY_CURRENT_USER, _T("Software\\MyApp"), _T("Name"));
     */
    static BOOL DeleteValue(HKEY hRoot, const CString& subKey, const CString& valueName);

    /**
     * 删除注册表子键（及其所有值）。
     * @param hRoot 根键
     * @param subKey 子键路径
     * @return 是否成功
     * 示例：
     * RegistryUtil::DeleteKey(HKEY_CURRENT_USER, _T("Software\\MyApp"));
     */
    static BOOL DeleteKey(HKEY hRoot, const CString& subKey);
};