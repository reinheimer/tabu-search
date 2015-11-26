# Minimização de árvores de Steiner
# Linguagem: GMPL

# Conjuntos
set S;  # Nodos opcionais
set R;  # Nodos terminais
set E within (S union R) cross (S union R);  # Arestas possíveis

set Tv within S union R; # Nodos selecionados
set Ta within E; # Arestas selecionadas

# set sTv within Tv;
# set step{s in 1..card(Tv)} :=

# set sTv{k in 1 .. card(Tv)} :=  setof{(i, j) in Tv cross : i }(i, k) union

# Parâmetros
param c {(v, u) in E};  # Custo da aresta (v, u)

# Variáveis
var x {(v, u) in E}, >= 0, binary; # Participação da aresta (v, u) na solução

# Função objetivo ##
minimize CUSTO: sum{v in Tv, u in Tv}c[v, u]*x[v, u];

# Restrições

# Nodos obrigatórios tem grau pelo menos 1
s.t. TERMINAIS{r in R}: sum{(i, j) in E: j == r}x[i, r] >= 1;

s.t. ARESTAS{k in 2..}: sum{(i, j) in E}x[i, j] = card(Tv) - 1;

# Grafo é conexo (como representar o conjunto das partes de Tv?)

s.t. CONEXO{}: sum{i in V, j in V}x[i, j] = |V| - 1;

solve;

printf{(i, j) in E} "%g\n", x[i, j];


# Dados (vêm dos conjuntos e parâmetros)
data;

# Conjuntos

set R := 1
         2
         3
         4
;

set S := 5
         6
         7
;

# Tupla com + pertence ao conjunto
set E : 1 2 3 4 5 6 7 :=
      1 - + - + + - -
      2 - - + - + - -
      3 - - - + - + +
      4 - - - - - + +
      5 - - - - - + -
      6 - - - - - - -
      7 - - - - - - -
;

set Tv; # Nodos selecionados

# Parâmetros
param c default 100:
          1  2  3  4  5  6  7 :=
        1 . 10  . 12  3  .  .
        2 .  . 12  .  3  .  .
        3 .  .  . 10  .  3  3
        4 .  .  .  .  .  3  3
        5 .  .  .  .  .  2  .
        6 .  .  .  .  .  .  .
        7 .  .  .  .  .  .  .
;

end;
