<div align="center">
  <h3 align="center">Snake Game</h3>
</div>

## Sobre
Trabalho da disciplina de Programação I, UFES. Objetivo: Usar conhecimentos básicos de programação para implementar um jogo da cobrinha.

### Pré-requisitos

- Linux
- [GNU Compiler Collection](https://gcc.gnu.org/)

### Instalando

1. Clone o repositório

   ```sh
   git clone https://github.com/tucozz/Snake_Prog.git
   ```

2. Dirija-se ao seu diretório

   ```sh
   cd Snake_Prog
   ```

3. Compile o código-fonte

   ```sh
   gcc -o main Jogo_Da_Cobrinha.c
   ```

## Inicialização

Intruções de uso o programa.

### Entradas
O usuário do programa deverá digitar como entrada:

```sh
./main <path>
```

Onde,

- `path` - é o caminho que contém os arquivos a serem processados.

### Mapa
O mapa será armazenado em um arquivo .txt.

- A primeira linha possui dois inteiros, definindo quantidade de linhas e colunas (maximo: 100x100).
- Após isso, cada célula será representada por um caracter, em uma matriz.

- `#` - Parede: Encerra o jogo se a cobra colidir com ela.
- `*` - Comida: Aumenta o corpo da cobra e ganha +1 ponto.
- `$` - Dinheiro: Aumenta a pontuação em +10
- `>` - Posição Inicial da Cobra: É indicada já no mapa, sempre para a direita.
- `o` - Corpo da cobra: Não é indicada no mapa, mas encerra o jogo ao entrar em colisão.

Um exemplo desse arquivo de definição do mapa é mostrado a seguir.

```sh
6 6
######
#    #
#> * #
#*  *#
#   $#
######
```

O exemplo acima define um mapa com 6 linhas e 6 colunas, paredes (#) em suas bordas, um
dinheiro ($) e três comidas (*) espalhadas pelo mapa. Além disso, a posição inicial da cobra
está na terceira linha e segunda coluna (posição do “>”).

## Jogando

 O jogo terminará quando não houver mais comidas pelo mapa ou a cobra
morrer (i.e., quando ela colidir com alguma parede ou em si mesma). As jogadas deverão ser
lidas da entrada padrão de dados, permitindo dessa forma, um redirecionamento a partir de
arquivo. O fluxo da jogada segue:

- A jogada será fornecida pelo usuário no formato “@”, em que @ é um caractere que
define o movimento: “h” para movimento horário, “a” para movimento anti-horário e “c”
para continuar (i.e., não alterar a direção).

## Logs

Ao fim do Jogo, são salvos alguns arquivos secundários para o registro da partida

- `resumo.txt` - Diz passo a passo o que ocorreu durante a partida.
- `ranking.txt` - Ordena quais foram as posições do mapa acessadas com mais frequência.
- `estatisticas.txt` - Contabiliza número de movimentos, número de movimentos sem pontuar, número de movimentos para
baixo, número de movimentos para cima, número de movimentos para a esquerda e número
de movimentos para a direita.
- `heatmap.txt` - Mostra com um mapa o quanto cada posição foi percorrida.

## Melhorias
Esse trabalho foi realizado no primeiro período, então é um registro histórico das minhas habilidades com programação nessa época.
Quaisquer sugestões de melhorias são bem vindas, e um dia eu ainda considero retornar a esse código com novos conhecimentos em C
