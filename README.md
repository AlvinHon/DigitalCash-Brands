Digital Cash - Brands
===

The repository contains proof-of-concept program to a digital cash scheme proposed by Stefan Brands.

The C++ implementation is to simply demonstrate the protocol between entites Bank (issuer), User (withdrawer) and Merchant. 
As general digital cash systems, relationship between entities is a trangular cycle such that Bank **issues** a coin to user, User **spends** the coin the Merchant, and finally Merchant stores the coin in bank for **deposit**.

It is similar to a common payment protocols currently avaiable in the Internet. 
But the main difference to isolate digital cash from nowadays onlne payment is that, digital cash provides **Anonymity** and **Offline-capability**.
In other words, Digital Cash is a digitalized banknote.

The topic was in my [final year thesis](https://www.researchgate.net/publication/236336064_Digital_Cash) and the coding has been only started ocasionally since I was graduated. I wish the source code could contribute to ease the efforts of researchers/fellows who are studying the topic.

Brands Scheme - Representation Problem
-----

In Brands Scheme, the security of the scheme is based on a hard problem - Representation Problem, an extention of Discrete Logarithm Problem..

Define unique generators gi ∈ Gq for prime order q and i ∈ [1, k]. A representation of h ∈ Gq
is tuple (a1, ..., ak) for ai ∈ Zq such that h = ∑gi^ai.

Then, given h and (g1, ..., gk), find the representation of h.

Details can be found in papers:

* Stefan A. Brands, An Efficient Off-line Electronic Cash System Based On The Representation Problem., CWI (Centre for Mathematics and Computer Science), Amsterdam, The Netherlands, 1993

* Stefan Brands, Untraceable Off-line Cash in Wallets with Observers (Extended Abstract), Proceedings of the 13th Annual International Cryptology Conference on Advances in Cryptology, p.302-318, August 22-26, 1993


Implementation
-----

libraries requires:
* Openssl
* [GMPlib](https://gmplib.org/)

Please note that the code contains very low level API function as a library. It is not actual impletmention of the Digital Cash's Service (e.g. a HTTP server or Service layer application)

It is to suggest studying more on the Brands sheme if intending to create your own application/service. You must be clear what variables are public in communication and what are secret to Bank/User, etc.

Exemple code is located in main.cpp. 

Bank -> User

```cpp
  brands::BrandsIssuer issuer(bparams);
  brands::WithdrawalByIssuer isser_transaction;
	
  brands::BrandsWithdrawer user(bparams);
  brands::WithdrawalByWithdrawer user_transaction;

  user.SetupReceive(issuer.SetupRegister(user.I));
  
  issuer.WithdrawalInit(user.I,&isser_transaction);
  user.WithdrawalChallenge(isser_transaction.A, isser_transaction.B, &user_transaction);
  issuer.WithdrawlResponse(user_transaction.Challenge,&isser_transaction);
```

User -> Merchant

```cpp
  // obtain a coin
  brands::Coin c;
  user.ObtainCoin(isser_transaction.R,user_transaction,&c);
  
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
```

As you might see, the payment protocol does not need to store the User's Identity for record. So Bank received the spent coin from Merchant knows nothing about the payer.

To avoid Double Spending, Banks should keep record of that anonymous spent coin. Once spent coin is detected, Brands scheme also provide method to recover the identity of the coin user (only when coin is spent twice by the user). 

```cpp
mpz_class I2 = issuer.CheckDoubleSpender(paid, paid2);
```
