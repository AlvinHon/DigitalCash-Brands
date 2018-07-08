package brands

import (
	"crypto/rand"
	"fmt"
	"math/big"
	//"time"
)

// GenCycGroup - generation of cyclic group
func GenCycGroup(n int) (*big.Int, *big.Int, *big.Int, *big.Int, *big.Int) {
	for {
		q, err := rand.Prime(rand.Reader, n-1)
		if err != nil {
			panic(err.Error())
		}
		n := new(big.Int).Mul(q, big.NewInt(2))
		p := new(big.Int).Add(n, big.NewInt(1))
		// p = 2q + 1, order(p)=n=2q
		if p.ProbablyPrime(40) {
			a := FindGen(p, q)
			g1 := FindGen(p, q)
			g2 := FindGen(p, q)
			return p, q, a, g1, g2
		}
	}
}

// FindGen -
func FindGen(p, q *big.Int) *big.Int {
	for {
		a, err := rand.Int(rand.Reader, p)
		if err != nil {
			panic(err.Error())
		}
		if b := new(big.Int).Exp(a, big.NewInt(2), p); b.Cmp(big.NewInt(1)) == 0 {
			continue
		}
		if b := new(big.Int).Exp(a, q, p); b.Cmp(big.NewInt(1)) == 0 {
			return a
		}
	}
}

func randPrime(n *big.Int) *big.Int {
	a, _ := rand.Int(rand.Reader, n)
	for !a.ProbablyPrime(40) {
		a, _ = rand.Int(rand.Reader, n)
	}
	return a
}

// TestCycgroup -
func TestCycgroup() {
	p, q, a, g1, g2 := GenCycGroup(1024)
	fmt.Println(p)
	fmt.Println(q)
	fmt.Println(a)
	fmt.Println(g1)
	fmt.Println(g2)
}
