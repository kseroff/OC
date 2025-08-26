#include <Windows.h>
#include <iostream>
using namespace std;

void main(void)
{
	int a = -10, b = -4, c = -3, d = -6, e = -5, y, x, z, p;
	_asm {
		mov eax, b;
		imul c;
		mov x, eax;
		mov eax, x;
		idiv d;
		mov z, eax;
		mov ebx, a;
		sub ebx, z;
		mov p, ebx;
		mov ebx, e;
		add ebx, p;
		mov y, ebx;
	}
	cout << y<<endl;
	system("pause");
}