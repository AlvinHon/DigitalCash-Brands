#ifndef _H_BRANDSUTIL
#define _H_BRANDSUTIL

#include "brands.h"

#include <openssl/hmac.h>
#include <iostream>
#include <stdlib.h>
namespace brands {

char RandChar(bool nozero = false) {
    const char char_l[] = {'0','1','2','3','4','5','6','7','8','9'};
    if (nozero) {
        return char_l[rand()%(sizeof(char_l)-1) + 1];
    }
    return char_l[rand()%sizeof(char_l)];
}

void SeedRandom() {
    struct timeval time; 
    gettimeofday(&time,NULL);
    std::srand((time.tv_sec * 1000) + (time.tv_usec / 1000));
}

std::string GenRandom(const mpz_class& max) {
    std::string ret = "";
    std::string maxs = max.get_str();
    size_t s = maxs.size();
    s = rand() % s + 1;
    ret += RandChar(true);
    for (int i = 1; i<s; i++){
        ret += RandChar();
    }
    return ret;
}


void hmacUpdate(HMAC_CTX * ctx, std::string d) {
    HMAC_Update(ctx, (unsigned char*)d.c_str(), d.length());
}

template<typename ...String>
void hmacUpdate(HMAC_CTX * ctx, std::string d, String... data) {
    hmacUpdate(ctx, d);
    hmacUpdate(ctx, data...);
}

template<typename ...String>
std::string GenHMACSHA256(std::string key, String... data) {
    std::string ret = "";
	unsigned int len;
	unsigned char out[EVP_MAX_MD_SIZE];
	HMAC_CTX *ctx = HMAC_CTX_new();

	HMAC_Init_ex(ctx, key.c_str(), key.length(), EVP_sha256(), NULL);
    hmacUpdate(ctx, data...);
	HMAC_Final(ctx, out, &len);
	HMAC_CTX_free(ctx);
    
	for (unsigned int i = 0;  i < len;  i++) {
        ret += out[i];
	}
    return ret;
}

template<typename ...String>
mpz_class GenHMACSHA256_MPZ(std::string key, String... data) {
    std::string hm = GenHMACSHA256(key, data...);
    size_t hlen = hm.length();
	const char * cs = hm.c_str();
	mpz_class hint;
	mpz_import(hint.get_mpz_t(), hlen, 1, 1 ,0, 0, cs);
    return hint;
}


} // namespace

#endif // _H_BRANDSUTIL