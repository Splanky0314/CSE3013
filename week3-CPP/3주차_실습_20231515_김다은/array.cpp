#include "Array.h"
#include <cstdlib>
#include<iostream>
using namespace std;
Array::Array(int size)
{
	//사이즈를 확인하고 양수이면 new를 사용하여 배열 data를 할당, len값 초기화
	if(size<0)
	{
		cout << "Size must be equal to or greater than 0" << endl;
	}
	else
	{
		// data에 size만큼 메모리 동적 할당
		data = new int[size];
		// len에 size 저장(배열 크기)
		len = size;
	}
}
Array::~Array()
{
	// 소멸자; 할당된 메모리 해제
	delete[] data;
}
int Array::length() const
{
	// 배열의 크기 리턴
	return len;
}

// 배열에 원소를 대입하거나 값을 반환하는 부분으로 []연산자의 오버로딩이다
int& Array::operator[](int i) // 배열에 원소 삽입
{
	static int tmp;

	// 배열의 인덱스가 범위 내에 있으면 값 리턴, 그렇지 않으면 에러메세지 출력하고 tmp리턴
	if(i >= 0 && i < len)
	{
		// 배열의 인덱스 범위 내에 있으므로 배열 값 리턴
		return data[i];
	}
	else
	{
		// 배열의 인덱스 범위 밖에 있는 경우 에러 출력
		cout << "Array bound error!" << endl;
		// tmp 리턴
		return tmp;
	}
}

int Array::operator[](int i) const // 배열의 원소값 반환
{
	//배열의 인덱스가 범위 내에 잇으면 값을 리턴, 그렇지 않으면 에러메세지 출력하고 0을 리턴
	if(i >= 0 && i<len)
	{	
		// 배열의 인덱스 범위 내에 있으므로 배열 값 리턴
		return data[i];
	}
	else
	{
		// 배열의 인덱스 범위 밖에 있으므로 에러 출력
		cout << "Array bound error!" << endl;
		// 0 리턴
		return 0;
	}
}
void Array::print() //배열의 모든 내용을 출력해주는 함수
{
	int i;
	cout<<"[";
	for(i=0; i<len; i++) {
		cout << data[i] << " ";
	}	
	cout << "]" << endl;
}
