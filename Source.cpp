#include "Header.h"
int main() {
	std::vector<int> vect(20, 7);
	vect[17] = 35;
	ProxyController obj(vect);
	obj.cout17El();
	return 0;
}