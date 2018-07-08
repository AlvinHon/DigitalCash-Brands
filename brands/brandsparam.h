#ifndef _H_BRANDSPARAM
#define _H_BRANDSPARAM

#include "brands.h"

namespace brands {

/**
 * Common parameters for cash system
 * can be specific to certain user
 */
class BrandsParams {
	public:
	// primes p,q: q|(p-1)
	mpz_class P;
	mpz_class Q;
	// public generators of Gq
	mpz_class G;
	mpz_class G1;
	mpz_class G2;

	BrandsParams & operator=(BrandsParams value)
	{
		this->P = value.P;
		this->Q = value.Q;
		this->G = value.G;
		this->G1 = value.G1;
		this->G2 = value.G2;
		
		return *this;
	}

	BrandsParams(){ }

	BrandsParams(const char * p, const char * q, const char * g, const char * g1, const char * g2)
	{
		this->P = p;
		this->Q = q;
		this->G = g;
		this->G1 = g1;
		this->G2 = g2;
	}
};

} // namespace

#endif // _H_BRANDSPARAM