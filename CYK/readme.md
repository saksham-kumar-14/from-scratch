# CYK Algorithm for Context Free Grammar
CYK is a parsing algorithm for context free grammar. In order to apply CYK algorithm to a grammar, it must be in [Chomsky Normal Form](./CFG_to_CNF.md)

## Algorithm
[Explanation](https://www.youtube.com/watch?v=VTH1k-xiswM)

Let w be the n length string to be parsed. And G represent the set of rules in our grammar with start state S.

1. Construct DP of size n x n 
2. if w == ε and S -> ε, is a rule in G then we accept the string else we reject 
```
for(1..n)
    for(A)
        if (A -> b and b = w_i for some i)
            dp[i][i] = A
```

```
for l in (2..n)
    for i in (i..n - l + 1)
        j = i + l - 1
        for k in (i..j - 1)
            for each rule A -> BC
                if (i, k) cell contains B and (k + 1, j) cell contains C
                    dp[i][j] = A
```

So, if S is in (1, n), then accept else reject

**Time Complexity:** *O(n^3)|G|*
    where |G| is the number of rules in grammar
**Space Complexity:** *O(n^2)*
