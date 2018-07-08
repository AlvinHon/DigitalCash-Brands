#ifndef _H_BRANDSWITHDRAWAL
#define _H_BRANDSWITHDRAWAL

#include "brands.h"

namespace brands {


// WithdrawalByIssuer - parameters known by issuer during withdrawal
class WithdrawalByIssuer {
	public:
	mpz_class A, B, w, R;
};

// WithdrawalByUser - parameters known provided by withdrawer
class WithdrawalByWithdrawer {
	public:
	mpz_class A_by_issuer, B_by_issuer;
	mpz_class Challenge, ChallengeD, BigA, BigB, ZD, AD, BD;
	PartialCoin partialCoin;
};

}

#endif