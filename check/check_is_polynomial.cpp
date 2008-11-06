/*
 * Check for inconsistency in power::is_polynomial
 */
#include "ginac.h"
#include <iostream>
#include <stdexcept>
using namespace std;
using namespace GiNaC;

static void do_test(const ex& e, const ex& s)
{
	if (e.is_polynomial(s))
		return;
	cerr << "*** Error: is_polynomial() says \"" << e << "\""
		<< "is not a polynomial in \"" << s << "\"" << endl;
	throw std::logic_error("bug in is_polynomial()");
}

int main(int argc, char** argv)
{
	cout << "checking for bugs in is_polynomial()... " << flush;
	symbol x("x"), s("s");
	ex e = sin(x) + 2*s;
	ex g = pow(2, x) + 2*s;
	do_test(e, s);
	do_test(g, s);
	cout << " OK, not found." << endl;
	return 0;
}
	
