# Minimizacao de arvores de Steiner

# Conjuntos
set S;  # Nodos opcionais
set R;  # Nodos terminais
set V := S union R;
set E within (S union R) cross (S union R);  # Arestas possiveis

# set sTv within Tv;
# set step{s in 1..card(Tv)} :=

# set sTv{k in 1 .. card(Tv)} :=  setof{(i, j) in Tv cross : i }(i, k) union

# Parametros
param c {(v, u) in E};  # Custo da aresta (v, u)

# Variaveis
var x {(v, u) in E}, >= 0, binary; # Participacao da aresta (v, u) na solucao
var y {v in V}, >= 0, binary; # Nodos selecionados (relacionar com x)

# baseado no forum do glpk #

# meus "servers" estao em V

# os tamanho de subconjuntos vao de 3 a |V| - 1
param p := 3;
param m := card(V) - 1;

# Number of servers available
param n := card(V); 

# Set of subset sizes geq p and leq m
set I := {1 .. m};

# Set of indices for a subset of size i of set V
set J{i in I} := { 1 .. round((prod{a in {1..n}}a) / 
  (prod{a in {1..i}}a) / (prod{a in {1..n-i}}a)) };

# Set of indices for V
set K := {1 .. n};

# Set containing the kth member of V
set L{k in K} := setof{v in V : k == sum{t in V : t <= v} 1} v;

# Set of integers in [i, n]
set M{i in I} := {i .. n};

# Number of subsets of size i of a set of size j - 1
param ji{i in I, j in M[i]} := if i == j then 0 
  else round((prod{a in {1..j-1}}a) / 
  (prod{a in {1..i}}a) / (prod{a in {1..j-1-i}}a)) ;
  
# Subsets j of size i
set N{i in I, j in J[i]} :=
  if i == 1 
  then L[j]
  else N[i-1,j - ji[i,max{k in M[i] : ji[i,k] < j} k]] 
  union L[max{k in M[i] : ji[i,k] < j} k];

## Funcao objetivo ##
minimize CUSTO: sum{(i, j) in E}c[i, j]*x[i, j];

# Restricoes

# Como relacionar a variavel de arestas com a de nodos?
s.t. CONSISTENCIA{(i, j) in E}: x[i, j] = (y[i] + y[j])/2;

# Nodos obrigatorios tem grau pelo menos 1
s.t. TERMINAIS{r in R}: sum{(i, j) in E: j == r}x[i, r] >= 1;

# Limitar o grau?
# s.t. GRAU{n in V}: sum{(i, j) in E: i == n}x[i, j] <= sum{k in V}Tv[k] - 1;

# Arestas necessarias no grafo inteiro = numero de nodos - 1
s.t. ARESTAS: sum{(i, j) in E}x[i, j] = sum{k in V}y[k] - 1;

# isso deveria ser verdade para subconjuntos tambem?
#s.t. ARESTAS{k in 2..}: sum{(i, j) in E}x[i, j] = card(y) - 1;

# Grafo conexo (como representar o conjunto das partes de Tv?)
# s.t. CONEXO{V in sTv, }: sum{i in V, j in V}x[i, j] = |V| - 1;
# http://claudiaboeres.pbworks.com/f/ListaResultados-Prova1-2010-1.pdf
# 7. Se um grafo (conexo ou desconexo) tem exatamente dois vértices de grau ímpar, então existe um caminho que liga esses dois vértices

# s.t. CONEXO{v in V, u in V: v != u}: sum{(i, j) in E: }
# s.t. CONEXO{P within y: card(P) >= 3}: sum{k in P, v in V}x[k, v] < sum{k in V}y[k] - 1;

# http://www.inf.ufpr.br/andre/Disciplinas/BSc/CI065/michel/Intro/intro.html#Inc     (Teorema 1-2)
## duvida reside aqui ##
# s.t. CONEXO: sum{v in V}(sum{(v, j) in E, k in 0..card(V)}x[v, j] * (-1)^k) mod 2 = 0;

# quero os subconjuntos do que?
# de nodos selecionados

#set L{k in K} := setof{s in S : k == sum{t in S : t <= s} 1} s;

# quantidade de nodos selecionados
# param n := sum{v in V: y[v] >0}1;

#set Tv{} := 
#param K := sum{v in V}y[v];
#var L{k in K} := setof{v in V : k == sum{u in V : u <= v} 1} v;

s.t. CONEXO{i in I, j in J[i]}: if (i >= p) && (sum{v in N[i, j]}y[v] = i) then sum{u in N[i, j], t in N[i, j]}x[u, t] < i;

solve;

printf "Subconjuntos, finalmente. [tamanho, índice] do subconjunto.\n";
for {i in I, j in J[i]} {
	printf "N[%d, %d] = ", i, j;
	printf {tralala in N[i, j]} "%s ", tralala;
	printf "\n";
}
printf "\n";



# printf{(i, j) in E} "%g\n", x[i, j];


# Dados (vem dos conjuntos e parametros)
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

#set Tv# := 1
        # 2
       #  3
        # 4
#; # Nodos selecionados

# Parametros
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
