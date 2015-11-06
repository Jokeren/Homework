#ifndef SMM_CONCEPT_H
#define SMM_CONCEPT_H
#include <iostream>

//check first is less than second
#define SMM_CHECK_LESS(first, second) \
	do \
{ if (first >= second) { \
	std::cerr<< "index out of range error!" << std::endl;\
	return ;\
					   }\
} while (0)\

//check first is greater or equal to second
#define SMM_CHECK_GREATER_EQUAL(first, second) \
	do \
{ if (first < second) { \
	std::cerr << "index less than require error!" << std::endl;\
	return ;\
					  }\
} while (0)\

#endif
