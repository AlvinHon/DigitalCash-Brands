#include <iostream>

#include <sys/time.h>
#include <stdlib.h>     
#include <time.h>       
#include <string.h>


// for linux/osx: download and unzip gmp-6.1.2, in linux/OSX, ./configure and then make
// library = .libs/libgmp.a
// for windows: mingw-get install gmp
#include <gmpxx.h> 

#include "brands.h" 

int main(){
    brands::SeedRandom();

    brands::BrandsParams bparams (
		"170635838606142236835668582024526088839118584923917947104881361096573663241835425726334688227245750988284470206339098086628427330905070264154820140913414479495481939755079707182465802484020944276739164978360438985178968038653749024959908959885446602817557541340750337331201115159158715982367397805202392369959",
		"85317919303071118417834291012263044419559292461958973552440680548286831620917712863167344113622875494142235103169549043314213665452535132077410070456707239747740969877539853591232901242010472138369582489180219492589484019326874512479954479942723301408778770670375168665600557579579357991183698902601196184979",
		"78905550771707176472046196448658658754654071756606341285020444888851221712001014402581392171061103428557663126791572695604498371123013626618548119268438831780941305546724071040612015830836639524139258909464724634581470073606830394285772846821881118677913790493744652978276338707019197283548145299345563445342",
		"144213202463066458950689095305115948799436864106778035179311009761777898846700415257265179855055640783875383274707858827879036088093691306491953244054442062637113833957623609837630797581860524549453053884680615629934658560796659252072641537163117203253862736053101508959059343335640009185013786003173143740486",
		"103961858063657931242220807914123164620648299315033976046547900569904472805027212131284033634769267152657588195583605493290002050604375954536172541064476442340046198608255280588784539677337268545146088599238052090050779330669947961063002552055764161954608835115838286817546073467543570323501842149742495540876"
	);

	brands::BrandsIssuer issuer(bparams);
	brands::WithdrawalByIssuer isser_transaction;
	
	brands::BrandsWithdrawer user(bparams);
	brands::WithdrawalByWithdrawer user_transaction;

	// set up account
	std::cout<< "Setup account for User - " << std::endl << user.I << std::endl << std::endl;
	user.SetupReceive(issuer.SetupRegister(user.I));
	// withdrawal
	issuer.WithdrawalInit(user.I,&isser_transaction);
	user.WithdrawalChallenge(isser_transaction.A, isser_transaction.B, &user_transaction);
	issuer.WithdrawlResponse(user_transaction.Challenge,&isser_transaction);
	
	if (user.WithdrawalVerify(issuer.H, isser_transaction.R, user_transaction)){ // optional
		std::cout << "withdrawal succeed!!" << std::endl;
	}

	// obtain a coin
	brands::Coin c;
	user.ObtainCoin(isser_transaction.R,user_transaction,&c);

	// payment ... 

	// receiver receive a coin
	if (c.Verify(issuer.H, bparams.G, bparams.P)) {
		std::cout << "Coin valid!!" << std::endl;
	}
	// receiver create challenage (should choose your own hash method)
	mpz_class d = brands::GenHMACSHA256_MPZ("shopA-payment-at-20180708-for-an-apple", c.C1.get_str(), c.C2.get_str());
	brands::SpentCoin paid;
	user.CoinResponse(c,d,user_transaction,&paid);
	// receiver received pain coin:
	if (paid.Verify(d, bparams.G1, bparams.G2, bparams.P)) {
		std::cout << "Coin paid!!" << std::endl;
	}

	// deposit ...

	// Issuer can verify the coin and the spent coin using (c, d, paid) where d included payment details
	// and Issuer can check if the coin c is spent before

	std::cout << "double spending the same coin!!" << std::endl;
	mpz_class d2 = brands::GenHMACSHA256_MPZ("shopB-payment-at-20180709-for-an-apple", c.C1.get_str(), c.C2.get_str());
	brands::SpentCoin paid2;
	user.CoinResponse(c,d2,user_transaction,&paid2);
	// receiver received pain coin:
	if (paid.Verify(d2, bparams.G1, bparams.G2, bparams.P)) {
		std::cout << "Coin paid!!" << std::endl;
	}

	std::cout << "bank detects the same coin!!" << std::endl;
	mpz_class I2 = issuer.CheckDoubleSpender(paid, paid2);
	std::cout << "double spender is " << std::endl << I2 << std::endl;

    return 0;
}