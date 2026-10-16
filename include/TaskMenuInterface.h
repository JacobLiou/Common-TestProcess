#ifndef _TASK_MENU_INTERFACE_H
#define _TASK_MENU_INTERFACE_H

#pragma warning(disable: 4786)
#include <map>
#include <vector>

class CValue  
{
public:
	CValue();
	CValue(const char* value);
	virtual ~CValue();
	const char* toString(void) const;
	const CString& toCString(void) const;
	int toInt(void) const;
	double toDouble(void) const;
	static CString null;
private:
	CString m_strValue;
};

class CItems  
{
public:
	CItems();
	virtual ~CItems();
	int count(void);
	void clear(void);
	void set(const char* name, const char* value);
	const CValue& get(const char* name);
	std::map<CString, CValue>& get(void);
	const CValue& operator[] (const char* name);
private:
	std::map<CString, CValue> m_items;
	CValue m_null;
};

class CTasks  
{
public:
	CTasks();
	virtual ~CTasks();
	CItems& set(const char* name);
	CItems& get(const char* name);
	CItems& get(int id);
	std::map<int, CItems>& get(void);
	int count(void);
	void clear(void);
	const CString& name(int id);
	int id(const char* name);
	CItems& operator[] (const char* name);
	CItems& operator[] (int id);
private:
	std::map<int, CItems> m_tasks_id;
	std::vector<CString> m_tasks_name;
	CItems m_null;
};

class CRange
{
public:
	CRange(void);
	CRange(int range, const CString& key, const CString& name);
	CRange(int range, const char* key, const char* name);
	int get_range(void);
	void set_range(int range);
	const CString& get_key(void);
	void set_key(const CString& key);
	void set_key(const char* key);
	const CString& get_name(void);
	void set_name(const CString& name);
	void set_name(const char* name);
private:
	int m_range;
	CString m_key, m_name;
};

class CTaskMenuInterface
{
public:
	virtual const char* GetLastError(void) = 0;
	virtual BOOL Read(LPCTSTR path) = 0;
	virtual int GetRangeInformation(int index, std::vector<CString>& info) = 0;
	virtual int count(void) = 0;
	virtual const CString& title(void) = 0;
	virtual const CString& name(int id) = 0;
	virtual const char* param_to_string(int id, const char* name) = 0;
	virtual const CString& param_to_cstring(int id, const char* name) = 0;
	virtual int param_to_int(int id, const char* name) = 0;
	virtual double param_to_double(int id, const char* name) = 0;
	virtual void clear(void) = 0;
	virtual int range_count(void) = 0;
	virtual const CString& range_key(int index) = 0;
	virtual const CString& range_name(int index) = 0;
	virtual int range_value(int index) = 0;
};

class CTaskMenuPro : public CTaskMenuInterface
{
public:
	CTaskMenuPro();
	virtual ~CTaskMenuPro();
	const char* GetLastError(void);
	BOOL Read(LPCTSTR path);
	int GetRangeInformation(int index, std::vector<CString>& info);
	int count(void);
	const CString& title(void);
	const CString& name(int id);
	const char* param_to_string(int id, const char* name);
	const CString& param_to_cstring(int id, const char* name);
	int param_to_int(int id, const char* name);
	double param_to_double(int id, const char* name);
	void clear(void);
	int range_count(void);
	const CString& range_key(int index);
	const CString& range_name(int index);
	int range_value(int index);
	//以下接口非同一DLL不能使用
	CTasks& GetTaskObject(void);
	CItems& operator[] (int id);
	CItems& operator[] (const char* name);
	CItems& operator[] (const CString& name);
private:
	CTasks m_tasks;
	std::vector<CRange> m_ranges;
	//错误信息
	CString m_strErr;
	CString m_strTitle;
};

#endif