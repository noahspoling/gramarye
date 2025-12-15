static char rcsid[] = "$Id$";
#include "except.h"
#include "assert.h"
const Except_T Assert_Failed = { "Assertion failed" };
void (assert)(int e) {
	assert(e);
}