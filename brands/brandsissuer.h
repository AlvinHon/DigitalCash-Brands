#ifndef _H_BRANDSISSUER
#define _H_BRANDSISSUER

#include "brands.h"

namespace brands {

class BrandsIssuer {

public:
	BrandsParams Settings;
	mpz_class H;
private:
	mpz_class x; // (x,H) key pair by issuer, x is secret key

public:
	BrandsIssuer(BrandsParams bparams){
		this->Settings = bparams;
		this->x = GenRandom(this->Settings.Q);
		// H = g^x
		mpz_powm_sec(this->H.get_mpz_t(), this->Settings.G.get_mpz_t(), this->x.get_mpz_t(), this->Settings.P.get_mpz_t());
	}  
	
	// Protocol: account setup
	mpz_class SetupRegister(const mpz_class I) {
		mpz_class ret;
		mpz_class tmp = I * this->Settings.G2;
		// z = (I*G2)^x
    	mpz_powm_sec(ret.get_mpz_t(),tmp.get_mpz_t(),this->x.get_mpz_t(), this->Settings.P.get_mpz_t());
		return ret;
	}

	// Protocol: withdrawal
	// send wi.A and wi.B
	void WithdrawalInit(const mpz_class I, WithdrawalByIssuer* wi) {
		mpz_class tmp;
		// a = g^w
		wi->w = GenRandom(this->Settings.Q);
		mpz_powm_sec(wi->A.get_mpz_t(), this->Settings.G.get_mpz_t(), wi->w.get_mpz_t(), this->Settings.P.get_mpz_t());
		// b = (I * g2) ^ w
		tmp = I * this->Settings.G2;
		mpz_powm_sec(wi->B.get_mpz_t(), tmp.get_mpz_t(), wi->w.get_mpz_t(), this->Settings.P.get_mpz_t());
	}

	// Protocol: withdrawal response
	// send wi.R, H for verify (optional)
	void WithdrawlResponse(const mpz_class c, WithdrawalByIssuer* wi) {
		//r = w + c*x mod q
		wi->R = ( wi->w + c * this->x ) % this->Settings.Q;
	}


	// in this case, issuer find the same coin is spent before but at different merchant (different r1 & r2)
	// it then returns the I of the withdrawer
	mpz_class CheckDoubleSpender(SpentCoin c1, SpentCoin c2) {
		// g1 ^ ( (r1-r1')/(r2-r2') )
		mpz_class ret, tmp1, tmp2, tmp3;
 
		if (c1.r1 > c2.r1) {
			tmp1 = c1.r1 - c2.r1;
			tmp2 = c1.r2 - c2.r2;
		} else {
			tmp1 = c2.r1 - c1.r1;
			tmp2 = c2.r2 - c1.r2;
		}
		mpz_invert(tmp3.get_mpz_t(), tmp2.get_mpz_t(), this->Settings.Q.get_mpz_t());
		tmp1 = (tmp1 * tmp3) % this->Settings.Q;

		mpz_powm_sec(ret.get_mpz_t(), this->Settings.G1.get_mpz_t(), tmp1.get_mpz_t(), this->Settings.P.get_mpz_t());

		return ret;
	}
};


} // namespace

#endif // _H_BRANDSISSUER