#pragma once
#include<Windows.h>

//用来确定链表头尾
#define REFLECT_DECLARE() \
static ClassPropertyMeta* lpFirstProperty; static ClassPropertyMeta* lpLastProperty;

//初始化链表头尾：
#define REFLECT_IMPLEMENT(C) ClassPropertyMeta* C::lpFirstProperty=nullptr; ClassPropertyMeta* C::lpLastProperty=nullptr;

//定义属性元数据和属性：
#define PROPERTY_DECLARE(C,T,P) T P; \
static struct ClassPropertyInfo<C,T> Prop_##P; void set_##P(T); T get_##P();

//初始化属性元数据和属性访问方法：
#define PROPERTY_IMPLEMENT(C,T,P)  struct ClassPropertyInfo<C,T> C::Prop_##P(#P,#T,&C::set_##P,&C::get_##P); \
inline void C::set_##P(T _value) { P = _value; } \
inline T C::get_##P() { return P; }

//访问属性元数据：
#define PROPERTY(C,P) &(C::Prop_##P)
//修改属性：
#define SET_PROP(O,P,V) (O->*P->Setter)(V)
//读取属性：
#define GET_PROP(O,P) (O->*P->Getter)()

struct ClassPropertyMeta {
	LPCSTR propertyName;
	LPCSTR propertyType;
	ClassPropertyMeta* lpNext;
};

template<class C, class T>
struct ClassPropertyInfo
{
	LPCSTR propertyName;
	LPCSTR propertyType;
	ClassPropertyMeta* lpNext;
	void (C::* Setter)(T);
	T(C::* Getter)();

	ClassPropertyInfo(LPCSTR _propertyName, LPCSTR _propertyType, void (C::* Setter)(T), T(C::* Getter)())
	{
		this->propertyName = _propertyName;
		this->propertyType = _propertyType;
		this->Setter = Setter;
		this->Getter = Getter;
		this->lpNext = nullptr;
		//初始化含有属性类的链表头节点
		if (C::lpFirstProperty == NULL)
		{
			C::lpFirstProperty = (ClassPropertyMeta*)this;
		}
		//将当前属性信息添加到链表结尾
		if (C::lpLastProperty != NULL)
		{
			C::lpLastProperty->lpNext = (ClassPropertyMeta*)this;
		}
		C::lpLastProperty = (ClassPropertyMeta*)this;
	};
};

//根据属性名查找属性：
template<class C, class T>
ClassPropertyInfo<C, T>* FindProperty(LPCSTR szName)
{
	ClassPropertyMeta* item = C::lpFirstProperty;
	while (item)
	{
		if (!strcmp(item->propertyName, szName)) {
			return reinterpret_cast<ClassPropertyInfo<C, T>*>(item);
		}
		item = item->lpNext;
	}
	return nullptr;
}

//示例程序
/*
#include<iostream>
#include<string>
using namespace std;

class TestClass
{
public:
	REFLECT_DECLARE();
	PROPERTY_DECLARE(TestClass, int, ID);
	PROPERTY_DECLARE(TestClass,string , Name);
};

REFLECT_IMPLEMENT(TestClass);
PROPERTY_IMPLEMENT(TestClass, int, ID);
PROPERTY_IMPLEMENT(TestClass, string, Name);

void main()
{
	TestClass testObject;

	//ClassPropertyInfo<TestClass, string>* NameInfo = PROPERTY(TestClass, Name);

	//获得属性元信息
	auto nameInfo = PROPERTY(TestClass, Name);

	//修改属性
	SET_PROP(&testObject, nameInfo, (LPSTR)"Wang");

	//读取属性
	cout <<  GET_PROP(&testObject, nameInfo) << endl;

	//搜索属性
	auto res = FindProperty<TestClass, string>("Age");
	if (!res) {
		cout << "无Age变量" << endl;
	}

	//枚举所有属性，和属性类型
	ClassPropertyMeta* item;
	for (item = TestClass::lpFirstProperty; item; item = item->lpNext)
	{
		cout << item->propertyType << " " << item->propertyName<< endl;
	}
}
*/