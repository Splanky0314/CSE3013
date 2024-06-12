#include "Str.h"
#include <string.h>

Str::Str(int leng) {
    // len에 leng(글자 수)를 저장
    len = leng;
    // len+1(NULL문자 고려)만큼 str에 메모리 동적 할당
    str = new char(len+1);
}

Str::Str(char *neyong) {
    // 입력받은 문자열의 길이를 len에 저장
    len = strlen(neyong);
    // len+1(NULL문자 고려)만큼 str에 메모리 동적 할당
    str = new char[len+1];
    // str에 neyong 저장
    strcpy(str, neyong);
}

Str::~Str() { 
    // str에 동적 할당된 메모리 해제
    delete[] str;
}

int Str::length(void) { 
    // 문자열의 길이 반환
    return len;
}

char* Str::contents(void) { 
    // 문자열의 내용 반환
    return str;
}

int Str::compare(class Str& a) { 
    // str과 a.str을 비교한 결과 반환
    return strcmp(str, a.contents());
}
int Str::compare(char *a) { // a의 내용과 strcmp.
    // str과 a라는 문자열을 비교한 결과 반환
    return strcmp(str, a);
}
void Str::operator=(char *a) { // string의 값을 대입.
    // a 문자열의 길이 저장
    len = strlen(a);
    // str에 동적 할당된 메모리 해제
    delete[] str;
    // a의 길이(len)에 맞추어 메모리 동적 할당
    str = new char[len+1];
    // str에 a의 내용 복사
    strcpy(str, a);
}
void Str::operator=(class Str& a) { // Str의 내용을 대입.
    // Str a의 len을 저장
    len = a.length();
    // str에 동적 할당된 메모리 해제
    delete[] str;
    // len(a.len)에 맞추어 메모리 동적 할당
    str = new char[len+1];
    // str에 a.str의 내용 복사
    strcpy(str, a.contents());
}