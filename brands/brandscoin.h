#ifndef _H_BRANDSCOIN
#define _H_BRANDSCOIN

#include <gmpxx.h> 
#include <string.h>
#include "brandsutil.h"

namespace brands {
    
class PartialCoin{
	public:
	mpz_class s;
	mpz_class x1;
	mpz_class x2;
	mpz_class u; // randomness u and v are kept by user
	mpz_class v; // randomness u and v are kept by user

	PartialCoin() { }
	PartialCoin(std::string _s, std::string _x1, std::string _x2, std::string _u, std::string _v) { 
		this->s = _s;
		this->x1 = _x1;
		this->x2 = _x2;
		this->u = _u;
		this->v = _v;
	}


	PartialCoin & operator=(PartialCoin value) {
		this->s = value.s;
		this->x1 = value.x1;
		this->x2 = value.x2;
		this->u = value.u;
		this->v = value.v;

		return *this;
	}

};

class Coin {
	public:
	mpz_class C1, C2, C3, C4, C5, C6;
	//PartialCoin partial;
	mpz_class CD;

	Coin & operator=(Coin value) {
		this->C1 = value.C1;
		this->C2 = value.C2;
		this->C3 = value.C3; // (C3,C4,C5,C6) = signature of (C1,C2)
		this->C4 = value.C4;
		this->C5 = value.C5;
		this->C6 = value.C6;
		this->CD = value.CD; // hash of (C1-C5) computed by command function to coin receivers
		
		return *this;
	}

	bool Verify(const mpz_class h, const mpz_class G, const mpz_class P) {
		mpz_class tmp, tmp2;

		if (this->C1 == 1) {
			return false;
		}

		// TODO should let user choose hash/signature algorithm
		mpz_class verCD = GenHMACSHA256_MPZ("brandskey", this->C1.get_str(), this->C2.get_str(), this->C3.get_str(), this->C4.get_str(), this->C5.get_str());
		verCD = verCD % P;

		if (this->CD != verCD) {
			return false;
		}

		// C4 * h^CD = g^C6
		mpz_powm_sec(tmp.get_mpz_t(), h.get_mpz_t(), this->CD.get_mpz_t(), P.get_mpz_t());
		tmp = (this->C4 * tmp) % P;
		mpz_powm_sec(tmp2.get_mpz_t(), G.get_mpz_t(), this->C6.get_mpz_t(), P.get_mpz_t());
		if (tmp != tmp2) {
			//std::cout << "C4 * h^CD != g^C6" << std::endl;
			return false;
		}

		// C5 * C3^CD = C1^C6
		mpz_powm_sec(tmp.get_mpz_t(), this->C3.get_mpz_t(), this->CD.get_mpz_t(), P.get_mpz_t());
		tmp = (tmp * this->C5) % P;
		mpz_powm_sec(tmp2.get_mpz_t(), this->C1.get_mpz_t(), this->C6.get_mpz_t(), P.get_mpz_t());
		if (tmp != tmp2) {
			//std::cout << "C5 * C3^CD = C1^C6" << std::endl;
			return false;
		}

		return true;
	}

};


class SpentCoin {
	public:
	Coin c;
	mpz_class r1;
	mpz_class r2;

	bool Verify(mpz_class d, mpz_class G1, mpz_class G2, mpz_class P) {
		mpz_class tmp, tmp2, tmp3;
		
		// C1^d * C2 == g1^r1 g2^r2

		mpz_powm_sec(tmp.get_mpz_t(), this->c.C1.get_mpz_t(), d.get_mpz_t(), P.get_mpz_t());
		tmp = (tmp * this->c.C2) % P;

		mpz_powm_sec(tmp2.get_mpz_t(), G1.get_mpz_t(), r1.get_mpz_t(), P.get_mpz_t());
		mpz_powm_sec(tmp3.get_mpz_t(), G2.get_mpz_t(), r2.get_mpz_t(), P.get_mpz_t());
		tmp2 = (tmp2 * tmp3) % P;

		return tmp == tmp2;
	}
};	

} // namespace

#endif // _H_BRANDSCOIN