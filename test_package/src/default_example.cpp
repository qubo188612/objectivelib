#include "example.h"
#include <iostream>
#ifndef CUSTOM_MAIN_SELF_IMPLEMENTATION

int main_self() {
	std::cerr <<  "custom_example.cpp not found. Please copy default_example.cpp to custom_example.cpp and customize your implementation." << '\n';
	return 0;
}
#endif
