#ifndef _H_BRANDSWITHDRAWER
#define _H_BRANDSWITHDRAWER

#include "brands.h"

namespace brands {

class BrandsWithdrawer {
	public:
	BrandsParams Settings;
	mpz_class u1, I; // key pair of withdrawer, where I is identity of the user
	mpz_class z; // compute by issuer

	BrandsWithdrawer(BrandsParams bp) {	
		Settings = bp;
        // u1 = random mod Q		
		this->u1 = GenRandom(bp.Q);
		this->u1 = this->u1 % bp.Q;
		// I = G1^u1 mod P
		mpz_powm_sec(this->I.get_mpz_t(), bp.G1.get_mpz_t(), this->u1.get_mpz_t(), bp.P.get_mpz_t());
     }

	// Protocol: account setup
	void SetupReceive(const mpz_class _z) { this->z = _z; }

	// Protocol: withdrawal challenge
	// send ww.Challenge
	void WithdrawalChallenge(const mpz_class A, const mpz_class B, WithdrawalByWithdrawer* ww) {
		mpz_class tmp, tmp2;
		PartialCoin pcoin(
			GenRandom(this->Settings.Q), // s
			GenRandom(this->Settings.Q), // x1
			GenRandom(this->Settings.Q), // x2
			GenRandom(this->Settings.Q), // u
			GenRandom(this->Settings.Q) // v
		);
		ww->A_by_issuer = A;
		ww->B_by_issuer = B;
		ww->partialCoin = pcoin;

		// A = (I * G2)^s
		tmp = this->I * this->Settings.G2;
		mpz_powm_sec(ww->BigA.get_mpz_t(), tmp.get_mpz_t(), pcoin.s.get_mpz_t(), this->Settings.P.get_mpz_t());

		// B = g1^x1 * g2^x2
		mpz_powm_sec(tmp.get_mpz_t(), this->Settings.G1.get_mpz_t(), pcoin.x1.get_mpz_t(), this->Settings.P.get_mpz_t());
		mpz_powm_sec(tmp2.get_mpz_t(), this->Settings.G2.get_mpz_t(), pcoin.x2.get_mpz_t(), this->Settings.P.get_mpz_t());
		ww->BigB = tmp * tmp2;

		// zd = z^s
		mpz_powm_sec(ww->ZD.get_mpz_t(), this->z.get_mpz_t(), pcoin.s.get_mpz_t(), this->Settings.P.get_mpz_t());

		// ad = a^u * g^v
		mpz_powm_sec(tmp.get_mpz_t(), A.get_mpz_t(), pcoin.u.get_mpz_t(), this->Settings.P.get_mpz_t());
		mpz_powm_sec(tmp2.get_mpz_t(), this->Settings.G.get_mpz_t(), pcoin.v.get_mpz_t(), this->Settings.P.get_mpz_t());
		ww->AD = tmp * tmp2;

		// bd = b^(s * u) * A^v
		tmp2 = pcoin.s * pcoin.u;
		mpz_powm_sec(tmp.get_mpz_t(), B.get_mpz_t(), tmp2.get_mpz_t(), this->Settings.P.get_mpz_t());
		mpz_powm_sec(tmp2.get_mpz_t(), ww->BigA.get_mpz_t(), pcoin.v.get_mpz_t(), this->Settings.P.get_mpz_t());
		ww->BD = tmp * tmp2;

		// cd = H(A,B,zd,ad,bd) (hash value) TODO - choose hash/sign function your own
		ww->ChallengeD = GenHMACSHA256_MPZ("brandskey",ww->BigA.get_str(), ww->BigB.get_str(), ww->ZD.get_str(), ww->AD.get_str(), ww->BD.get_str());

		// c = cd/u mod q
		mpz_invert(tmp.get_mpz_t(),pcoin.u.get_mpz_t(),this->Settings.Q.get_mpz_t());
		ww->Challenge = (ww->ChallengeD * tmp) % this->Settings.Q;
	}

	// Protocol: withdrawal verify (optional)
	bool WithdrawalVerify(const mpz_class h, const mpz_class r, const WithdrawalByWithdrawer ww) {
		mpz_class tmp, tmp2;

		// (I * g2) ^ r == z^c * b
		tmp2 = this-> I * this->Settings.G2;
		mpz_powm_sec(tmp.get_mpz_t(), tmp2.get_mpz_t(), r.get_mpz_t(), this->Settings.P.get_mpz_t());
		mpz_powm_sec(tmp2.get_mpz_t(), this->z.get_mpz_t(), ww.Challenge.get_mpz_t(), this->Settings.P.get_mpz_t());
		tmp2 = (tmp2 * ww.B_by_issuer) % this->Settings.P;

		if (tmp != tmp2) {
			//std::cout << "(I * g2) ^ r != z^c * b" << std::endl;
			return false;
		}

		// g ^ r == h^c * a
		mpz_powm_sec(tmp.get_mpz_t(), this->Settings.G.get_mpz_t(), r.get_mpz_t(), this->Settings.P.get_mpz_t());
		mpz_powm_sec(tmp2.get_mpz_t(), h.get_mpz_t(), ww.Challenge.get_mpz_t(), this->Settings.P.get_mpz_t());
		tmp2 = (tmp2 * ww.A_by_issuer) % this->Settings.P;

		if (tmp != tmp2) {
			//std::cout << "g ^ r != h^c * a" << std::endl;
			return false;
		}
		
		return true;
	}

	void ObtainCoin(const mpz_class r, WithdrawalByWithdrawer ww, Coin* coin) {
		
		//coin->partial = ww.partialCoin;
		coin->C1 = ww.BigA;
		coin->C2 = ww.BigB;
		coin->C3 = ww.ZD;
		coin->C4 = ww.AD;
		coin->C5 = ww.BD;
		// rd = ru + v mod Q
		coin->C6 = (r * ww.partialCoin.u + ww.partialCoin.v) % this->Settings.Q; 

		coin->CD = ww.ChallengeD;
	}

	void CoinResponse(Coin c, mpz_class d, WithdrawalByWithdrawer ww, SpentCoin* pcoin) {
		mpz_class tmp, tmp2;

		pcoin->c = c;
		
		//r1 = d(u1)s + x1 mod q
		pcoin->r1 = (d * this->u1 * ww.partialCoin.s + ww.partialCoin.x1) % this->Settings.Q;
		
		//r2 = ds + x2 mod q
		pcoin->r2 = (d * ww.partialCoin.s + ww.partialCoin.x2) % this->Settings.Q;
	}
};

} // namespace

#endif // _H_BRANDSWITHDRAWER