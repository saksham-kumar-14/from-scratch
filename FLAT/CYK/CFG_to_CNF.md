# Converting Context Free Grammar to Context Normal Form

For CNF, it must satify the following production conditions-
1. A non-terminal generating a terminal function (eg; X -> x)
    - They form leaves of your parse tree
2. A non-terminal generating two non-terminal functions (eg; X -> YZ)
    - This is the core constraint. By forcing one symbol to split into exactly two, you guarantee that the structure of the derivation is a binary tree.
3. The start symbol is allowed to generate ε (the empty string) only if the empty string is actually in the language.

- A single CFG can be converted to multiple equivalent CNF forms
- CNF produces the same language as original CFG
- For a string of length n, a CNF derivation requires at most 2n-1 derivation steps.
- Any CFG that does not generate ε has an equivalent CNF.


## Steps to convert CFG to CNF
### 1. Eliminate the start symbol
If start symbol S is at the **RHS** of any production in the grammar, create a new production as: S0→S where S0 is the new start symbol.
### 2. Remove Null, Unit, and Useless Productions
### 3. Replace terminals with mixed Productions
Eliminate terminals from RHS if they exist with other terminals or non-terminals. e.g. , production rule X→ xY can be decomposed as: X→ZY, Z→x.
### 4. Reduce productions with more than two terminals
Eliminate RHS with more than two non-terminals. e.g,; production rule X→XYZ can be decomposed as: X→PZ, P→XY 

#### *For example,*
`P: S -> ASA|aB, A -> B|S, B -> b|ε`
1. `P: S0 -> S, S -> ASA|aB, A -> B|S, B -> b|ε`
2. 
- Remove Null productions
    - After removing `B -> ε`: `P : S0 -> S, S -> ASA|aB|a, A -> B|S|ε, B -> b`
    - After removing `A -> ε`: `P : S0 -> S, S -> ASA|aB|a|AS|SA|S, A -> B|S, B -> b`
3. After removing unit productions: 
    ```
    P : S0 -> ASA|aB|a|AS|SA,
        S  -> ASA|aB|a|AS|SA,
        A  -> b|ASA|aB|a|AS|SA,
        B  -> b
    ```
4. Remove where `> 2` variables are present on RHS.
    ```
    P : S0 -> AX|aB|a|AS|SA,
        S  -> AX|aB|a|AS|SA,
        A  -> b|AX|aB|a|AS|SA,
        B  -> b,
        X  -> SA 
    ```
5. Remove where one terminal and variable are together
    ```
    P : S0 -> AX|YB|a|AS|SA,
        S  -> AX|YB|a|AS|SA,
        A  -> b|AX|YB|a|AS|SA,
        B  -> b,
        X  -> SA,
        Y  -> a
    ```

