#include <stdio.h>
#include <stdlib.h>

//modularizacao dos simbolos do mapa e da entrada
#define cobradireita '>'
#define cobraesquerda '<'
#define cobracima '^'
#define cobrabaixo 'v'
#define cobracorpo 'o'
#define cobramorta 'X'
#define comida '*'
#define dinheiro '$'
#define parede '#'
#define espaco ' '
#define antihorario 'a'
#define horario 'h'
#define continuar 'c'
#define portal '@'

typedef struct{
	int movimentos;
	int sempontuar;
	int parabaixo;
	int paracima;
	int paradireita;
	int paraesquerda;
}tEstatisticas;

typedef struct{
	int linha;
	int coluna;
}tPortal;

typedef struct{
	int linhas;
	int colunas;
	char mapa[100][100];
	int quantascomidas;
	int linhainicial;
	int colunainicial;
	tPortal portal1;
	tPortal portal2;

}tMapa;

typedef struct{
	int tamanho;
	int coluna[100];
	int linha[100];
	int newcoluna;
	int newlinha;
	char direcao;
	int ultimorabocoluna;
	int ultimorabolinha;
	int colidiu;
	
}tCobra;

typedef struct{
	tMapa mapa;
	tCobra cobra;
	int pontos;
	char caracteratual;
	char caminhoraiz[1000];
	int turnoatual;
	int heatmap[100][100];
	int ranking[10000];
	tEstatisticas estatisticas;
}tJogo;

void AtualizaResumo(tJogo jogo, char evento){
	FILE * arquivo;
	char caminhosaida[1018];

	//esse codigo vai sendo aberto e atualizado durante o jogo, em tempo real.
	sprintf(caminhosaida, "%s/saida/resumo.txt", jogo.caminhoraiz);
	arquivo = fopen(caminhosaida, "a");
	if(evento == dinheiro){
		fprintf(arquivo, "Movimento %d (%c) gerou dinheiro\n", jogo.turnoatual, jogo.caracteratual);
	}
	if(evento == comida){
		if(jogo.mapa.quantascomidas){
			fprintf(arquivo, "Movimento %d (%c) fez a cobra crescer para o tamanho %d\n", jogo.turnoatual, jogo.caracteratual, jogo.cobra.tamanho);
		}
		else{
			fprintf(arquivo, "Movimento %d (%c) fez a cobra crescer para o tamanho %d, terminando o jogo\n", jogo.turnoatual, jogo.caracteratual, jogo.cobra.tamanho);
		}
	}
	if(evento == parede){
		fprintf(arquivo, "Movimento %d (%c) resultou no fim de jogo por conta de colisao\n", jogo.turnoatual, jogo.caracteratual);
	}

	fclose(arquivo);
}

tMapa LeMapa(FILE * arquivo){
	tMapa mapa;
	int i, j;
	mapa.portal1.linha = 0;
	mapa.portal1.coluna = 0;
	mapa.portal2.linha = 0;
	mapa.portal2.coluna = 0;
	int jatemportal1 = 0;
	
	//le quantas linhas e colunas tem o mapa
	fscanf(arquivo, "%d %d[^\n]", &mapa.linhas, &mapa.colunas);
	fscanf(arquivo, "%*c");
	
	//começa a ler caracter por caracter
	for(i=1;i<=mapa.linhas;i++){
		for(j=1;j<=mapa.colunas;j++){
			fscanf(arquivo, "%c", &mapa.mapa[i][j]);
		}
		fscanf(arquivo, "%*c");
	}
	
	//confere onde esta a posicao inicial da cobra, portais e quantas comidas tem
	mapa.quantascomidas = 0;
	for(i=1;i<=mapa.linhas;i++){
		for(j=1;j<=mapa.colunas;j++){
			if(mapa.mapa[i][j] == cobradireita){
				mapa.linhainicial = i;
				mapa.colunainicial = j;
			}
			if(mapa.mapa[i][j] == comida){
				mapa.quantascomidas++;
			}
			if(mapa.mapa[i][j] == portal){
				if(jatemportal1){
					mapa.portal2.linha = i;
					mapa.portal2.coluna = j;
				}
				else{
					mapa.portal1.linha = i;
					mapa.portal1.coluna = j;
					jatemportal1 = 1;
				}
			}
		}
	}

	return mapa;
}

void IniciaMapa(tJogo jogo){
	int i, j;
	FILE * arquivosaida;
	char caminhosaida[1025];
	
	//cria um caminho de saida e abre o arquivo la
	sprintf(caminhosaida, "%s/saida/inicializacao.txt", jogo.caminhoraiz);
	arquivosaida = fopen(caminhosaida, "w");
	
	if(!arquivosaida){
		printf("nao foi possivel abrir o arquivo %s\n", caminhosaida);
	}
	
	//ja criado o arquivo, ele printa o mapa
	for(i=1;i<=jogo.mapa.linhas;i++){
		for(j=1;j<=jogo.mapa.colunas;j++){
			fprintf(arquivosaida, "%c", jogo.mapa.mapa[i][j]);
		}
		fprintf(arquivosaida, "\n");
	}
	
	//e printa tambem o estagio inicial da cobra
	fprintf(arquivosaida, "A cobra comecara o jogo na linha %d e coluna %d\n", jogo.mapa.linhainicial, jogo.mapa.colunainicial);
	
	//por fim, fecha o arquivo
	fclose(arquivosaida);
}

tJogo IniciaJogo(char argv[]){
	tJogo jogo;
	jogo.turnoatual = 0;
	char caminhomapa[1001], caminhoresumo[1000];
	int i, j;
	FILE * arquivomapa;
	
	//salva o caminho raiz para ser usado futuramente
	sprintf(jogo.caminhoraiz, "%s", argv);

	//remove o arquivo resumo pq ele pode dar problema
	sprintf(caminhoresumo, "%s/saida/resumo.txt", argv);
	remove(caminhoresumo);
	
	//encontra o caminho do mapa e abre ele
	sprintf(caminhomapa, "%s/mapa.txt", argv);
	arquivomapa = fopen(caminhomapa, "r");
	if(!arquivomapa){
		printf("não foi possivel encontrar o arquivo %s\n", caminhomapa);
		exit(1);
	}

	//le o mapa e cria um arquivo que contem a inicializacao
	jogo.mapa = LeMapa(arquivomapa);
	IniciaMapa(jogo);
	
	//por fim, fecha o arquivo
	fclose(arquivomapa);

	//inicia o heatmap com tudo como zero e a posicao inicial marcada
	for(i=1;i<=jogo.mapa.linhas;i++){
		for(j=1;j<=jogo.mapa.colunas;j++){
			jogo.heatmap[i][j] = 0;
		}
		jogo.heatmap[jogo.mapa.linhainicial][jogo.mapa.colunainicial] = 1;
	}

	return jogo;
}

int JogoAcabou(tJogo jogo){
	if(jogo.mapa.quantascomidas == 0 || jogo.cobra.colidiu){
		return 1;
	}
	return 0;
}

int Bateu(tJogo jogo){
	//checa se ela bateu em uma parede
	if(jogo.mapa.mapa[jogo.cobra.newlinha][jogo.cobra.newcoluna] == parede){
		return 1;
	}
	//checa se ela bateu no proprio corpo
	if(jogo.mapa.mapa[jogo.cobra.newlinha][jogo.cobra.newcoluna] == cobracorpo){
		//precisa checar se ela ta batendo com o ultimo pedaco do corpo, pq ai o corpo vai se mover, entao nao precisa contar a colisao
		if(jogo.cobra.newlinha == jogo.cobra.ultimorabolinha && jogo.cobra.newcoluna == jogo.cobra.ultimorabocoluna){
			return 0;
		}
		return 1;
	}
	//se nao retornou nada ainda é pq nao bateu em nada
	return 0;
}

int FezPonto(tJogo jogo){
	if(jogo.mapa.mapa[jogo.cobra.newlinha][jogo.cobra.newcoluna] == dinheiro){
		return 1;
	}
	return 0;
}

int Comeu(tJogo jogo){
	if(jogo.mapa.mapa[jogo.cobra.newlinha][jogo.cobra.newcoluna] == comida){
		return 1;
	}
	return 0;
}

tJogo CresceCobra(tJogo jogo){
	int i;

	jogo.cobra.tamanho++;

	//pega as novas coordenadas de cada ponto do corpo da cobra
	for(i=jogo.cobra.tamanho; i>=2; i--){
		jogo.cobra.coluna[i] = jogo.cobra.coluna[i-1];
		jogo.cobra.linha[i] = jogo.cobra.linha[i-1];
	}
	//pega as novas coordenadas da cabeca da cobra separadamente
	jogo.cobra.coluna[1] = jogo.cobra.newcoluna;
	jogo.cobra.linha[1] = jogo.cobra.newlinha;

	//atualiza o mapa com as novas informacoes
	jogo.mapa.mapa[jogo.cobra.linha[1]][jogo.cobra.coluna[1]] = jogo.cobra.direcao;
	for(i=2;i<=jogo.cobra.tamanho;i++){
		jogo.mapa.mapa[jogo.cobra.linha[i]][jogo.cobra.coluna[i]] = cobracorpo;
	}

	//atualiza qual o ultimo pedaco da cobra
	jogo.cobra.ultimorabolinha = jogo.cobra.linha[jogo.cobra.tamanho];
	jogo.cobra.ultimorabocoluna = jogo.cobra.coluna[jogo.cobra.tamanho];

	return jogo;
}

tJogo MoveCobra(tJogo jogo){
	int i;

	//pega as novas coordenadas de cada ponto do corpo da cobra
	for(i=jogo.cobra.tamanho; i>=2; i--){
		jogo.cobra.coluna[i] = jogo.cobra.coluna[i-1];
		jogo.cobra.linha[i] = jogo.cobra.linha[i-1];
	}
	//pega as novas coordenadas da cabeca da cobra separadamente
	jogo.cobra.coluna[1] = jogo.cobra.newcoluna;
	jogo.cobra.linha[1] = jogo.cobra.newlinha;

	//atualiza o mapa com as novas informacoes
	for(i=2;i<=jogo.cobra.tamanho;i++){
		jogo.mapa.mapa[jogo.cobra.linha[i]][jogo.cobra.coluna[i]] = cobracorpo;
	}
	jogo.mapa.mapa[jogo.cobra.ultimorabolinha][jogo.cobra.ultimorabocoluna] = espaco;
	jogo.mapa.mapa[jogo.cobra.linha[1]][jogo.cobra.coluna[1]] = jogo.cobra.direcao;

	//atualiza qual o ultimo pedaco da cobra
	jogo.cobra.ultimorabolinha = jogo.cobra.linha[jogo.cobra.tamanho];
	jogo.cobra.ultimorabocoluna = jogo.cobra.coluna[jogo.cobra.tamanho];

	//lembrar de perguntar o thiago como funciona isso direito
	//caso a cobra morra, o corpo dela todo tem que ser trocado por um X, incluindo o ponto de impacto
	if(jogo.cobra.colidiu){
		for(i=1;i<=jogo.cobra.tamanho;i++){
			jogo.mapa.mapa[jogo.cobra.linha[i]][jogo.cobra.coluna[i]] = cobramorta;
		}
	}

	return jogo;
}

int EntrouNoPortal(tJogo jogo){
	if(jogo.mapa.mapa[jogo.cobra.newlinha][jogo.cobra.newcoluna] == portal){
		return 1;
	}
	return 0;
}

tJogo TeleportaCobra(tJogo jogo){
	//sao muitos os casos de portal, entao eh melhor tratar cada um separadamente

	if(jogo.cobra.newlinha == jogo.mapa.portal1.linha && jogo.cobra.newcoluna == jogo.mapa.portal1.coluna){
		if(jogo.cobra.direcao == cobracima){
			jogo.cobra.newlinha = jogo.mapa.portal2.linha-1;
			jogo.cobra.newcoluna = jogo.mapa.portal2.coluna;
		}
		else if(jogo.cobra.direcao == cobradireita){
			jogo.cobra.newlinha = jogo.mapa.portal2.linha;
			jogo.cobra.newcoluna = jogo.mapa.portal2.coluna+1;
		}
		else if(jogo.cobra.direcao == cobrabaixo){
			jogo.cobra.newlinha = jogo.mapa.portal2.linha+1;
			jogo.cobra.newcoluna = jogo.mapa.portal2.coluna;
		}
		else if(jogo.cobra.direcao == cobraesquerda){
			jogo.cobra.newlinha = jogo.mapa.portal2.linha;
			jogo.cobra.newcoluna = jogo.mapa.portal2.coluna-1;
		}
	}
	else if(jogo.cobra.newlinha == jogo.mapa.portal2.linha && jogo.cobra.newcoluna == jogo.mapa.portal2.coluna){
		if(jogo.cobra.direcao == cobracima){
			jogo.cobra.newlinha = jogo.mapa.portal1.linha-1;
			jogo.cobra.newcoluna = jogo.mapa.portal1.coluna;
		}
		else if(jogo.cobra.direcao == cobradireita){
			jogo.cobra.newlinha = jogo.mapa.portal1.linha;
			jogo.cobra.newcoluna = jogo.mapa.portal1.coluna+1;
		}
		else if(jogo.cobra.direcao == cobrabaixo){
			jogo.cobra.newlinha = jogo.mapa.portal1.linha+1;
			jogo.cobra.newcoluna = jogo.mapa.portal1.coluna;
		}
		else if(jogo.cobra.direcao == cobraesquerda){
			jogo.cobra.newlinha = jogo.mapa.portal1.linha;
			jogo.cobra.newcoluna = jogo.mapa.portal1.coluna-1;
		}
	}

	return jogo;
}

tJogo LeJogada(tJogo jogo){
	char lixo;

	//infelizmente nao consegui pensar em um sistema melhor pra isso: eh um monte de if que converte sentido horario / antihorario em uma posicao nova para a cobra
	scanf("%c", &jogo.caracteratual);
	scanf("%c", &lixo);
	if(jogo.caracteratual == antihorario){
		if(jogo.cobra.direcao == cobradireita){
			jogo.cobra.newlinha = jogo.cobra.linha[1]-1;
			jogo.cobra.newcoluna = jogo.cobra.coluna[1];
			jogo.cobra.direcao = cobracima;
			jogo.estatisticas.paracima++;
		}
		else if(jogo.cobra.direcao == cobraesquerda){
			jogo.cobra.newlinha = jogo.cobra.linha[1]+1;
			jogo.cobra.newcoluna = jogo.cobra.coluna[1];
			jogo.cobra.direcao = cobrabaixo;
			jogo.estatisticas.parabaixo++;
		}
		else if(jogo.cobra.direcao == cobracima){
			jogo.cobra.newlinha = jogo.cobra.linha[1];
			jogo.cobra.newcoluna = jogo.cobra.coluna[1]-1;
			jogo.cobra.direcao = cobraesquerda;
			jogo.estatisticas.paraesquerda++;
		}
		else if(jogo.cobra.direcao == cobrabaixo){
			jogo.cobra.newlinha = jogo.cobra.linha[1];
			jogo.cobra.newcoluna = jogo.cobra.coluna[1]+1;
			jogo.cobra.direcao = cobradireita;
			jogo.estatisticas.paradireita++;
		}
	}
	else if(jogo.caracteratual == horario){
		if(jogo.cobra.direcao == cobradireita){
			jogo.cobra.newlinha = jogo.cobra.linha[1]+1;
			jogo.cobra.newcoluna = jogo.cobra.coluna[1];
			jogo.cobra.direcao = cobrabaixo;
			jogo.estatisticas.parabaixo++;
		}
		else if(jogo.cobra.direcao == cobraesquerda){
			jogo.cobra.newlinha = jogo.cobra.linha[1]-1;
			jogo.cobra.newcoluna = jogo.cobra.coluna[1];
			jogo.cobra.direcao = cobracima;
			jogo.estatisticas.paracima++;
		}
		else if(jogo.cobra.direcao == cobracima){
			jogo.cobra.newlinha = jogo.cobra.linha[1];
			jogo.cobra.newcoluna = jogo.cobra.coluna[1]+1;
			jogo.cobra.direcao = cobradireita;
			jogo.estatisticas.paradireita++;
		}
		else if(jogo.cobra.direcao == cobrabaixo){
			jogo.cobra.newlinha = jogo.cobra.linha[1];
			jogo.cobra.newcoluna = jogo.cobra.coluna[1]-1;
			jogo.cobra.direcao = cobraesquerda;
			jogo.estatisticas.paraesquerda++;
		}
	}
	else if(jogo.caracteratual == continuar){
		if(jogo.cobra.direcao == cobradireita){
			jogo.cobra.newlinha = jogo.cobra.linha[1];
			jogo.cobra.newcoluna = jogo.cobra.coluna[1]+1;
			jogo.estatisticas.paradireita++;
		}
		else if(jogo.cobra.direcao == cobraesquerda){
			jogo.cobra.newlinha = jogo.cobra.linha[1];
			jogo.cobra.newcoluna = jogo.cobra.coluna[1]-1;
			jogo.estatisticas.paraesquerda++;
		}
		else if(jogo.cobra.direcao == cobracima){
			jogo.cobra.newlinha = jogo.cobra.linha[1]-1;
			jogo.cobra.newcoluna = jogo.cobra.coluna[1];
			jogo.estatisticas.paracima++;
		}
		else if(jogo.cobra.direcao == cobrabaixo){
			jogo.cobra.newlinha = jogo.cobra.linha[1]+1;
			jogo.cobra.newcoluna = jogo.cobra.coluna[1];
			jogo.estatisticas.parabaixo++;
		}
	}
	else{
		printf("caracter invalido\n");
		exit(1);
	}

	//confere a funcao bonus
	if(EntrouNoPortal(jogo)){
		jogo = TeleportaCobra(jogo);
	}

	//calcula se a cobra esta fora das bordas do mapa, e se estiver, reseta ela para dar a volta
	if(jogo.cobra.newlinha > jogo.mapa.linhas){
		jogo.cobra.newlinha = 1;
	}
	if(jogo.cobra.newlinha < 1){
		jogo.cobra.newlinha = jogo.mapa.linhas;
	}
	if(jogo.cobra.newcoluna > jogo.mapa.colunas){
		jogo.cobra.newcoluna = 1;
	}
	if(jogo.cobra.newcoluna < 1){
		jogo.cobra.newcoluna = jogo.mapa.colunas;
	}

	//existe o caso de ele girar no mapa e sair em um portal, entao precisa conferir novamente
	if(EntrouNoPortal(jogo)){
		jogo = TeleportaCobra(jogo);
	}

	return jogo;
}

tJogo ProcessaJogada(tJogo jogo){
	jogo.turnoatual++;
	jogo.heatmap[jogo.cobra.newlinha][jogo.cobra.newcoluna]++;

	if(Bateu(jogo)){
		jogo.cobra.colidiu = 1;
		AtualizaResumo(jogo, parede);
		jogo = MoveCobra(jogo);
		jogo.estatisticas.movimentos++;
		jogo.estatisticas.sempontuar++;
	}
	else if(EntrouNoPortal(jogo)){
		jogo = TeleportaCobra(jogo);
	}
	else if(FezPonto(jogo)){
		jogo.pontos+=10;
		jogo.estatisticas.movimentos++;
		AtualizaResumo(jogo, dinheiro);
		jogo = MoveCobra(jogo);
	}
	else if(Comeu(jogo)){
		jogo.mapa.quantascomidas-=1;
		jogo.estatisticas.movimentos++;
		jogo = CresceCobra(jogo);
		AtualizaResumo(jogo, comida);
	}
	else{
		jogo.estatisticas.movimentos++;
		jogo.estatisticas.sempontuar++;
		jogo = MoveCobra(jogo);
	}

	return jogo;
}

void PrintJogo(tJogo jogo){
	int i, j;

	printf("\nEstado do jogo apos o movimento '%c':\n", jogo.caracteratual);

	//printa o mapa
	for(i=1;i<=jogo.mapa.linhas;i++){
		for(j=1;j<=jogo.mapa.colunas;j++){
			printf("%c", jogo.mapa.mapa[i][j]);
		}
		printf("\n");
	}

	printf("Pontuacao: %d\n", jogo.cobra.tamanho-1+jogo.pontos);
}

void FinalizaJogo(tJogo jogo){
	if(jogo.cobra.colidiu){
		printf("Game over!\nPontuacao final: %d\n", jogo.pontos+jogo.cobra.tamanho-1);
	}
	else{
		printf("Voce venceu!\nPontuacao final: %d\n", jogo.pontos+jogo.cobra.tamanho-1);
	}
}

tJogo PreparaCobra(tJogo jogo){
	//inicia cada atributo da cobra
	jogo.cobra.tamanho = 1;
	jogo.cobra.direcao = cobradireita;
	jogo.cobra.coluna[1] = jogo.mapa.colunainicial;
	jogo.cobra.linha[1] = jogo.mapa.linhainicial;
	jogo.cobra.colidiu = 0;
	jogo.cobra.ultimorabocoluna = jogo.cobra.coluna[1];
	jogo.cobra.ultimorabolinha = jogo.cobra.linha[1];

	//aproveita esse espaço para iniciar as estatisticas tambem
	jogo.estatisticas.movimentos = 0;
	jogo.estatisticas.parabaixo = 0;
	jogo.estatisticas.paracima = 0;
	jogo.estatisticas.paradireita = 0;
	jogo.estatisticas.paraesquerda = 0;
	return jogo;
}

tJogo RealizaJogo(tJogo jogo){
	//transfere os dados do mapa para a cobra de verdade
	jogo = PreparaCobra(jogo);

	//enquanto nao acabar o jogo, continua pedindo uma nova jogada e realizando o jogo
	while(!JogoAcabou(jogo)){
		jogo = LeJogada(jogo);
		jogo = ProcessaJogada(jogo);
		PrintJogo(jogo);
	}

	//quando sair, exibe a mensagem final
	FinalizaJogo(jogo);

	return jogo;
}

void GeraEstatisticas(tJogo jogo){
	FILE * arquivo;
	char caminhosaida[1024];

	sprintf(caminhosaida, "%s/saida/estatisticas.txt", jogo.caminhoraiz);
	arquivo = fopen(caminhosaida, "w");
	
	fprintf(arquivo, "Numero de movimentos: %d\n", jogo.estatisticas.movimentos);
	fprintf(arquivo, "Numero de movimentos sem pontuar: %d\n", jogo.estatisticas.sempontuar);
	fprintf(arquivo, "Numero de movimentos para baixo: %d\n", jogo.estatisticas.parabaixo);
	fprintf(arquivo, "Numero de movimentos para cima: %d\n", jogo.estatisticas.paracima);
	fprintf(arquivo, "Numero de movimentos para esquerda: %d\n", jogo.estatisticas.paraesquerda);
	fprintf(arquivo, "Numero de movimentos para direita: %d\n", jogo.estatisticas.paradireita);

	fclose(arquivo);
}

void GeraHeatmap(tJogo jogo){
	FILE * arquivo;
	char caminhosaida[1019];
	int i, j, k = 0;

	sprintf(caminhosaida, "%s/saida/heatmap.txt", jogo.caminhoraiz);
	arquivo = fopen(caminhosaida, "w");
	
	for(i=1;i<=jogo.mapa.linhas;i++){
		for(j=1;j<=jogo.mapa.colunas;j++){
			if(j==jogo.mapa.colunas){
				fprintf(arquivo, "%d", jogo.heatmap[i][j]);
			}
			else{
				fprintf(arquivo, "%d ", jogo.heatmap[i][j]);
			}
		}
		fprintf(arquivo, "\n");
	}

	fclose(arquivo);
}

void GeraRanking(tJogo jogo){
	FILE * arquivo;
	char caminhosaida[1019];
	int i, j, k, l;

	sprintf(caminhosaida, "%s/saida/ranking.txt", jogo.caminhoraiz);
	arquivo = fopen(caminhosaida, "w");
	
	//imprimeseparadamente cada posicao do ranking
	for(i=1024;i>=1;i--){
		for(j=1;j<=jogo.mapa.linhas;j++){
			for(k=1;k<=jogo.mapa.colunas;k++){
				if(jogo.heatmap[j][k] == i){
					fprintf(arquivo, "(%d, %d) - %d\n", j-1, k-1, i);
				}
			}
		}
	}

	fclose(arquivo);
}

int main(int argc, char * argv[]){

	FILE * arquivomapa;
	char caminhomapa[1001];
	tJogo jogo;
	
	if(argc <= 1){
		printf("ERRO: O diretorio de arquivos de configuracao nao foi informado\n");
		return 1;
	}

	//iniciar jogo
	jogo = IniciaJogo(argv[1]);
	
	//realizar jogo
	jogo = RealizaJogo(jogo);

	//gerar resumo de resultados
	// o resumo eh atualizado gradualmente durante o codigo

	//gerar estatisticas
	GeraEstatisticas(jogo);

	//gerar ranking
	GeraRanking(jogo);

	//gerar heatmap
	GeraHeatmap(jogo);

	//funcao bonus
	//feita
	
	return 0;
}

