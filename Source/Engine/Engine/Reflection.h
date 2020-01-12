#pragma once
#include<Windows.h>

//����ȷ������ͷβ
#define REFLECT_DECLARE() \
static ClassPropertyMeta* lpFirstProperty; static ClassPropertyMeta* lpLastProperty;

//��ʼ������ͷβ��
#define REFLECT_IMPLEMENT(C) ClassPropertyMeta* C::lpFirstProperty=nullptr; ClassPropertyMeta* C::lpLastProperty=nullptr;

//��������Ԫ���ݺ����ԣ�
#define PROPERTY_DECLARE(C,T,P) T P; \
static struct ClassPropertyInfo<C,T> Prop_##P; void set_##P(T); T get_##P();

//��ʼ������Ԫ���ݺ����Է��ʷ�����
#define PROPERTY_IMPLEMENT(C,T,P)  struct ClassPropertyInfo<C,T> C::Prop_##P(#P,#T,&C::set_##P,&C::get_##P); \
inline void C::set_##P(T _value) { P = _value; } \
inline T C::get_##P() { return P; }

//��������Ԫ���ݣ�
#define PROPERTY(C,P) &(C::Prop_##P)
//�޸����ԣ�
#define SET_PROP(O,P,V) (O->*P->Setter)(V)
//��ȡ���ԣ�
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
		//��ʼ�����������������ͷ�ڵ�
		if (C::lpFirstProperty == NULL)
		{
			C::lpFirstProperty = (ClassPropertyMeta*)this;
		}
		//����ǰ������Ϣ��ӵ������β
		if (C::lpLastProperty != NULL)
		{
			C::lpLastProperty->lpNext = (ClassPropertyMeta*)this;
		}
		C::lpLastProperty = (ClassPropertyMeta*)this;
	};
};

//�����������������ԣ�
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

//ʾ������
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

	//�������Ԫ��Ϣ
	auto nameInfo = PROPERTY(TestClass, Name);

	//�޸�����
	SET_PROP(&testObject, nameInfo, (LPSTR)"Wang");

	//��ȡ����
	cout <<  GET_PROP(&testObject, nameInfo) << endl;

	//��������
	auto res = FindProperty<TestClass, string>("Age");
	if (!res) {
		cout << "��Age����" << endl;
	}

	//ö���������ԣ�����������
	ClassPropertyMeta* item;
	for (item = TestClass::lpFirstProperty; item; item = item->lpNext)
	{
		cout << item->propertyType << " " << item->propertyName<< endl;
	}
}
*/