/*
 * Computacao Grafica
 * Codigo Exemplo: Rasterizacao de Segmentos de Reta com GLUT/OpenGL
 * Autor: Prof. Laurindo de Sousa Britto Neto
 */

// Bibliotecas utilizadas pelo OpenGL
#ifdef __APPLE__
    #define GL_SILENCE_DEPRECATION
    #include <GLUT/glut.h>
    #include <OpenGL/gl.h>
    #include <OpenGL/glu.h>
#else
    #include <GL/glut.h>
    #include <GL/gl.h>
    #include <GL/glu.h>
#endif

#include <cmath>
#include <cstdio>
#include <vector>
#include <cstdlib>
#include <forward_list>
#include "glut_text.h"

using namespace std;

// Variaveis Globais
#define ESC 27
#define PI 3.14159265358979323846

// Enumeracao com os tipos de formas geometricas
enum tipo_forma{LIN = 1, RET = 2, TRI = 3, POL = 4, CIR = 5}; // Linha, Triangulo, Retangulo, Poligono e Circulo
enum tipo_transf{TRA = 6, ROT = 7, ESCA = 8, CIS = 9, REF = 10}; // Translacao, Rotacao, Escala, Cisalhamento e Reflexao

// Verificacoes booleanas
bool click1 = false, click2 = false; 	// clique do mouse
bool poligonoIniciado = false;			// "o desenho do poligono ja foi iniciado?"
int n = 2;								// quantidade vertices atuais poligono

// Coordenadas do mouse
int m_x, m_y; 							// posicao atual do mouse
int x_1, y_1, x_2, y_2, x_3, y_3;		// posicao de cada clique

// Indica o tipo de forma geometrica e tipo de operacao ativa para desenhar
int modo = LIN;
int operacao = TRA;

// Largura e altura da janela
int width = 512, height = 512;

// Definicao de vertice
struct vertice{
    int x;
    int y;
};

// Definicao das formas geometricas
struct forma{
    int tipo;
    forward_list<vertice> v; //lista encadeada de vertices
};


bool colorir = false;

// Estrutura para cor
struct Color{
    GLfloat R;
    GLfloat G;
    GLfloat B;
};

// Obter cor do pixel
Color getPixelColor(GLint x, GLint y){
    Color color;
    glReadPixels(x, y, 1, 1, GL_RGB, GL_FLOAT, &color);
    return color;
}

// Mudar cor do pixel
void setPixelColor(GLint x, GLint y, Color color){
    glColor3f(color.R, color.G, color.B);
    glBegin(GL_POINTS);
    glVertex2i(x, y);
    glEnd();
    glutSwapBuffers();
}

// Retorna true ou false dependendo se as cores sao iguais ou nao
bool saoIguais(Color a, Color b){
    return a.R == b.R && a.G == b.G && a.B == b.B;
}

void floodFill(GLint x, GLint y, Color corAntiga, Color novaCor){
    Color color;
    color = getPixelColor(x, y);

    if (saoIguais(color, corAntiga)){
    	printf("Tentando pintar\n");
        setPixelColor(x, y, novaCor);
        floodFill(x + 1, y, corAntiga, novaCor);
        floodFill(x, y + 1, corAntiga, novaCor);
        floodFill(x - 1, y, corAntiga, novaCor);
        floodFill(x, y - 1, corAntiga, novaCor);
    }
}

// Lista encadeada de formas geometricas
forward_list<forma> formas;

// Funcao para armazenar uma forma geometrica na lista de formas
void pushForma(int tipo){
    forma f;
    f.tipo = tipo;
    formas.push_front(f);	// Armazena sempre no inicio da lista
}

// Funcao para armazenar um vertice na forma do inicio da lista de formas geometricas
void pushVertice(int x, int y){
    vertice v;
    v.x = x;
    v.y = y;
    formas.front().v.push_front(v);		// Armazena sempre no inicio da lista
}

// Funcao para armazenar uma Linha na lista de formas geometricas
void pushLinha(int x1, int y1, int x2, int y2){
    pushForma(LIN);
    pushVertice(x1, y1);
    pushVertice(x2, y2);
}

void pushRetangulo(int x1, int y1, int x2, int y2){
	pushForma(RET);
	pushVertice(x1, y1); // vertice 1
    pushVertice(x2, y2); // vertice 2
    pushVertice(x2, y1); // vertice 3
    pushVertice(x1, y2); // vertice 4
    
    /*
    1 --------- 3
    |			|
    |			|
    |			|
    4 --------- 2
    
    */
}

void pushTriangulo(int x1, int y1, int x2, int y2, int x3, int y3){
	pushForma(TRI);
	pushVertice(x1, y1);
    pushVertice(x2, y2);
    pushVertice(x3, y3);
}

// Declaracoes antecipadas (forward) das funcoes (assinaturas das funcoes)
void init(void);
void reshape(int w, int h);
void display(void);
void menu_popup(int value);
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
void mousePassiveMotion(int x, int y);
void drawPixel(int x, int y);
void drawFormas();						// Funcao que percorre a lista de formas geometricas, desenhando-as na tela
void translacao(int dx, int dy);		// Declara translacao
void rotacao(float angulo);				// Declara rotacao
void escala(float Sx, float Sy);		// Declara escala
void cisalhamento(float Cx, float Cy);	// Declara cisalhamento
void reflexao(int Rx, int Ry);			// Declara reflexao		
void algoritmoBresenham (double x1,double y1,double x2,double y2);
void rasterizaCircunferencia (int xc, int yc, double raio);


// Funcao principal
int main(int argc, char** argv){
    glutInit(&argc, argv); 									// Passagens de parametro C para o glut
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB); 			// Selecao do Modo do Display e do Sistema de cor
    glutInitWindowSize (width, height);  					// Tamanho da janela do OpenGL
    glutInitWindowPosition (100, 100); 						// Posicao inicial da janela do OpenGL
    glutCreateWindow ("Computacao Grafica: Paint"); 		// Da nome para uma janela OpenGL
    init(); 												// Chama funcao init();
    glutReshapeFunc(reshape); 								// funcao callback para redesenhar a tela
    glutKeyboardFunc(keyboard); 							// funcao callback do teclado
    glutMouseFunc(mouse); 									// funcao callback do mouse
    glutPassiveMotionFunc(mousePassiveMotion); 				// funcao callback do movimento passivo do mouse
    glutDisplayFunc(display); 								// funcao callback de desenho
    
    // Sub menus
    int menu_desenhar = glutCreateMenu(menu_popup);
    glutAddMenuEntry("Linha", LIN);
	glutAddMenuEntry("Retangulo", RET);
	glutAddMenuEntry("Triangulo", TRI);
	glutAddMenuEntry("Poligono", POL);
	glutAddMenuEntry("Circunferencia", CIR);
	
	int menu_transformacao = glutCreateMenu(menu_popup);
	glutAddMenuEntry("Translacao", TRA);
	glutAddMenuEntry("Rotacao", ROT);
	glutAddMenuEntry("Escala", ESCA);
	glutAddMenuEntry("Cisalhamento", CIS);
	glutAddMenuEntry("Reflexao", REF);
	
	// Menu principal
	glutCreateMenu(menu_popup);
	glutAddSubMenu("Desenhar", menu_desenhar);
	glutAddSubMenu("Transformar", menu_transformacao);
	glutAddMenuEntry("Sair", 0);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	
	printf("=================================================================\n");
	printf("Por padrao, LINHA e TRANSLACAO estao previamente selecionados\n\n");
	printf("Teclas disponiveis:\n");
	printf("  'C' - Limpar a tela\n");
	printf("  'Z' - Apagar a ultima forma desenhada\n\n");
	printf("Transformacoes (apos selecionar no menu):\n");
	printf("  'WASD' - Direcao da translacao\n");
	printf("  'R' - Rotacao\n");
	printf("  'WS' - Aumentar/diminuir a escala\n");
	printf("  'XY' - Cisalhamento no eixo X ou Y\n");
	printf("  'XYO' - Reflexao sobre o eixo X, o eixo Y ou a origem do sistema\n\n");
	printf("OBS: Para fechar o poligono, desenhar sobre a previa que conecta\n     ao primeiro vertice\n");
	printf("=================================================================\n");
	printf("\n");

    glutMainLoop(); 			// executa o loop do OpenGL
    return EXIT_SUCCESS; 		// retorna 0 para o tipo inteiro da funcao main();
}

// Inicializa alguns parametros do GLUT
void init(void){
    glClearColor(1.0, 1.0, 1.0, 1.0); //Limpa a tela com a cor branca;
}

// Ajusta a projecao para o redesenho da janela
void reshape(int w, int h){
	// Muda para o modo de projecao e reinicializa o sistema de coordenadas
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Definindo o Viewport para o tamanho da janela
	glViewport(0, 0, w, h);
	
	width = w;
	height = h;
    glOrtho (0, w, 0, h, -1 ,1);  

   // muda para o modo de desenho
	glMatrixMode(GL_MODELVIEW);
 	glLoadIdentity();

}

// Controla os desenhos na tela
void display(void){
    glClear(GL_COLOR_BUFFER_BIT); 				//Limpa o buffer de cores e reinicia a matriz
    glColor3f (0.0, 0.0, 0.0); 					// Seleciona a cor default como preto
    drawFormas(); 								// Desenha as formas geometricas da lista
    //Desenha texto com as coordenadas da posicao do mouse
    draw_text_stroke(0, 0, "(" + to_string(m_x) + "," + to_string(m_y) + ")", 0.2);
    glutSwapBuffers(); 							// manda o OpenGl renderizar as primitivas
}

// Controla o menu pop-up
void menu_popup(int value){
    if (value == 0) exit(EXIT_SUCCESS);
    
    if (value >= LIN && value <= CIR) {
		modo = value;
	} else if(value > CIR){
		operacao = value;
	}
}

int dx = 30, dy = 30;							// Deslocamento (em pixels) para translacao						
int angulo = 15.0; 								// angulo (em graus) de rotacao
float anguloEmRad = angulo * (PI / 180.0);
float fatorEscala = 2;							// Fator de escala
float Cx = 2, Cy = 2;							// Fator de cisalhamento nos eixos X e Y

// Controle das teclas comuns do teclado
void keyboard(unsigned char key, int x, int y){
    switch (key) { // key - variavel que possui valor ASCII da tecla pressionada
        case ESC: 
            exit(EXIT_SUCCESS); 
            break;
        case 'c':								// Limpar a tela
        case 'C':
            formas.clear();
            printf("Tela limpada\n\n");
            glutPostRedisplay();
            break;
        case 'z':								// Desfazer desenho
	    case 'Z':			
	    	if(!formas.empty()){
			  formas.pop_front();
	    	  printf("Ultima forma desfeita\n\n");
			  glutPostRedisplay();	
			} 
            break;
        case 'p':
	    case 'P':
	    	colorir = !colorir;
	    	if(colorir) printf("Modo colorir ATIVADO. Proibido desenhar.\n");
	    	else printf("Modo colorir DESATIVADO. Livre para desenhar.\n");
	    	printf("\n");
	    	break;
        case 'W':
        case 'w': // Transformacões geométricas
		    switch (operacao) {
		        case TRA:
		            if(!formas.empty()){
						printf("Translacao de %d pixels para cima\n", dy);
		            	translacao(0, dy);
					} 
		            break;
		        case ESCA:
		        	if(!formas.empty()){
		        		printf("Formas aumentadas em %.1fx\n", fatorEscala);
		        		escala(fatorEscala,fatorEscala);
					
					}
		    }
		    break;
		case 'S':
        case 's':
        	switch (operacao) {
		        case TRA:
		            if(!formas.empty()){
						printf("Translacao de %d pixels para baixo\n", dy);
		            	translacao(0, -dy);
					} 
		            break;
		        case ESCA:
		        	if(!formas.empty()){
		        		printf("Formas diminuidas em 1/%.1fx\n", fatorEscala);
		        		escala(1/fatorEscala, 1/fatorEscala);
					}
					break;
		    }
		    break;
        case 'A':
        case 'a':
        	switch (operacao) {
		        case TRA:
		            if(!formas.empty()){
						printf("Translacao de %d pixels para esquerda\n", dx);
		            	translacao(-dx, 0);
					} 
		            break;
		    }
		    break;
        case 'D':
        case 'd':
        	switch (operacao) {
		        case TRA:
		            if(!formas.empty()){
						printf("Translacao de %d pixels para direita\n", dx);
		            	translacao(dx, 0);
					} 
		            break;
		    }
		    break;
		case 'R':
        case 'r':
        	switch(operacao){
				case ROT:
		        	if(!formas.empty()){
						printf("Rotacao de %d graus\n", angulo);
						rotacao(anguloEmRad);
					}
		            break;
			}
			break;
		case 'X':
        case 'x':
        	switch (operacao){
				case CIS:
					if(!formas.empty()){
						printf("Cisalhamento de %.1f no eixo X\n", Cx);
		            	cisalhamento(Cx, 0);
					} 
					break;
				case REF:
					if(!formas.empty()){
						printf("Reflexao sobre o eixo X\n");
						reflexao(1, -1);
					}
			}
			break;
	    case 'Y':
	    case 'y':
	    	switch (operacao){
				case CIS:
					if(!formas.empty()){
						printf("Cisalhamento de %.1f no eixo Y\n", Cy);
		            	cisalhamento(0, Cy);
					} 
					break;
				case REF:
					if(!formas.empty()){
						printf("Reflexao sobre o eixo Y\n");
						reflexao(-1, 1);
					}
			}
			break;
		case 'O':
        case 'o':
        	switch(operacao){
				case REF:
					if(!formas.empty()){
						printf("Reflexao sobre a origem\n");
						reflexao(-1, -1);
					}
			}
			break;
    }
}

// Controle dos botoes do mouse
void mouse(int button, int state, int x, int y){
    switch (button) {
        case GLUT_LEFT_BUTTON:
        	if(colorir){
        		if (state == GLUT_DOWN) {		
		    		// Cor do fundo branco.
					Color oldColor = {1.0f, 1.0f, 1.0f};
					// Nova cor vermelha.
					Color newColor = {1.0f, 0.0f, 0.0f};
					// Coordenada de um ponto no fundo branco (ajuste conforme necessário).
					floodFill(x, height - y - 1, oldColor, newColor);
					
					
					Color colorAtPoint = getPixelColor(x, height - y - 1); // Ponto dentro do polígono.
				    printf("Color at (150, 150): r = %f, g = %f, b = %f\n", colorAtPoint.R, colorAtPoint.G, colorAtPoint.B);
				}
			}
			else{
				switch(modo){
	                case LIN:
	                    if (state == GLUT_DOWN) {
	                        if(click1){
	                            x_2 = x;
	                            y_2 = height - y - 1;
	                            printf("Vertice 2 (%d, %d)\n\n",x_2,y_2);
	                            pushLinha(x_1, y_1, x_2, y_2);
	                            click1 = false;
	                            glutPostRedisplay();
	                        }else{
	                            click1 = true;
	                            x_1 = x;
	                            y_1 = height - y - 1;
	                            printf("Linha\n");
	                            printf("Vertice 1 (%d, %d)\n",x_1,y_1);
	                        }
	                    }
	                	break;
	            	case RET:
	            		if (state == GLUT_DOWN) {
					        if (click1) {
					            x_2 = x;
					            y_2 = height - y - 1; 
					            printf("Vertice 2 (%d, %d)\n\n", x_2, y_2);
					            pushRetangulo(x_1, y_1, x_2, y_2);
					            click1 = false;
					            glutPostRedisplay();
					        } else {
					        	click1 = true;
					            x_1 = x;
					            y_1 = height - y - 1; 
					            printf("Retangulo\nVertice 1 (%d, %d)\n", x_1, y_1);
					        }
				    	}
	   					break;
	   				case TRI:
	   					if (state == GLUT_DOWN){
							if(!click1){
								click1 = true;
								x_1 = x;
					            y_1 = height - y - 1;
					            printf("Triangulo\nVertice 1 (%d, %d)\n", x_1, y_1);
							} else if(!click2){
								click2 = true;
								x_2 = x;
					            y_2 = height - y - 1; 
					            printf("Vertice 2 (%d, %d)\n", x_2, y_2);
							} else{
								click1 = click2 = false;
								x_3 = x;
					            y_3 = height - y - 1;
					            printf("Vertice 3 (%d, %d)\n\n", x_3, y_3);
					            pushTriangulo(x_1, y_1, x_2, y_2, x_3, y_3);
							}
						} 
						break;
					case POL:
					    if (state == GLUT_DOWN) {
			                if (!poligonoIniciado) {
			                	x_1 = x;
		   	   	   	   	    	y_1 = height - y - 1;
			                    click1 = true;
			                    poligonoIniciado = true;
			                    pushForma(POL);
			                    pushVertice(x_1, y_1);
			                    printf("Poligono\nVertice 1 (%d, %d)\n", x_1, y_1);
			                } else {
			                	x_2 = x;
			                	y_2 = height - y - 1; 
			                	click1 = false;
			                	
			                    int distX = abs(x_1 - x_2);
			                    int distY = abs(y_1 - y_2);
			                    int tolerancia = 10;
			
			                    if (distX < tolerancia && distY < tolerancia) {
			                        printf("Poligono fechado\n\n");
			                        poligonoIniciado = false; 		// Reset para o próximo
			                        n = 2;							// Reset para o próximo
			                        click1 = false;
			                    } else {
			                        pushVertice(x_2, y_2);
			                        printf("Vertice %d (%d, %d)\n", n++, x_2, y_2);
			                        
			                    }
			                }
			            }
			            break;
			        case CIR:
			        	if (state == GLUT_DOWN){
							if(!click1){
								click1 = true;
								x_1 = x;
	                            y_1 = height - y - 1;
	                            printf("Circunferencia\n");
	                            printf("Centro (%d, %d)\n",x_1,y_1);
							}
							else{
								x_2 = x;
			                	y_2 = height - y - 1; 
								click1 = false;
								double raio = sqrt(pow(x_2 - x_1, 2) + pow(y_2 - y_1, 2)); // distancia euclidiana
								printf("Raio: %.1f\n", raio);
								printf("Diametro: %.1f\n\n", raio*2);
								rasterizaCircunferencia(x_1, y_1, raio);
							}
			        	break;
	    			}
				}
			}	
	}
}

vertice calcularCentroide(forma& f) {
	int somaX = 0, somaY = 0, n = 0, i = 0;
	
	for(forward_list<vertice>::iterator v = f.v.begin(); v != f.v.end(); v++, i++){
        somaX += v->x;
        somaY += v->y;
        ++n;
    }
 
	if (n == 0) return vertice{0, 0}; // Retorna {0, 0} se nao houver vértices
    return vertice{somaX / n, somaY / n}; // Retorna o centroide calculado	
}

void multiplicarMatrizes(double a[3][3], double b[3][3], double resultado[3][3]) {
	// inicializa a matriz resultado
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            resultado[i][j] = 0;
        }
    }

	// multiplicacao das matrizes
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 3; k++) {
                resultado[i][j] += a[i][k] * b[k][j]; // multiplicacao das matra
            }
        }
    }
}

void imprimirMatriz(double matriz[3][3]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            printf("%.1f ", matriz[i][j]);
        }
        printf("\n");
    }
}

void multiplicarVetorPorMatriz(const double vetor[3], const double matriz[3][3], double resultado[3]) {
    for (int i = 0; i < 3; ++i) {
        resultado[i] = 0;
        for (int j = 0; j < 3; j++) {
            resultado[i] += vetor[j] * matriz[j][i];
        }
    }
}

void calcularMatrizTransformacao(vertice centroide, double operacao[3][3], double matrizTransformacao[3][3]) {
	double transladaParaOrigem[3][3] = {
        {1.0, 0.0, 0.0}, 
        {0.0, 1.0, 0.0},
        {-(double)centroide.x, -(double)centroide.y, 1.0}
    };
    
    double transladaDeVolta[3][3] = {
        {1.0, 0.0, 0.0},
        {0.0, 1.0, 0.0},
        {(double)centroide.x, (double)centroide.y, 1.0}
    };
    
	double operacaoNaOrigem[3][3];
	
	// translada para a origem e realiza a operacao passada como parametro, salvando na matriz operacaoNaOrigem
    multiplicarMatrizes(transladaParaOrigem, operacao, operacaoNaOrigem);
    
    // multiplica o resultado obtido anteriormente pela translacao de volta, para a forma retornar ao ponto inicial ja transformada
    multiplicarMatrizes(operacaoNaOrigem, transladaDeVolta, matrizTransformacao);
}

// funcao que evita a repeticao desses comandos padroes nas funcoes de transformacao
// recebe a forma como parametro e percorre seus vertices, calculando o valor do novo vertice de acordo com a matriz de transformacao
void aplicarTransformacaoVertices(forma& f, double matrizTransformacao[3][3]){
	int i = 0;
	for(forward_list<vertice>::iterator v = f.v.begin(); v != f.v.end(); v++, i++){
		double coordHomogenea[3] = {(double)v->x, (double)v->y, 1.0}; // converte para coordenada homogênea
		double verticeResultante[3];
		
		multiplicarVetorPorMatriz(coordHomogenea, matrizTransformacao, verticeResultante);
		
		// exibe o valor antigo e novo da coordenada, a fim de comparar os dois
		printf("(%d, %d) -> (%d, %d)\n", v->x, v->y, (int)round(verticeResultante[0]), (int)round(verticeResultante[1]));
			
        // atualiza as coordenadas do vértice
        v->x = verticeResultante[0];
        v->y = verticeResultante[1];
	}
	printf("\n");
}

void reflexao(int Rx, int Ry){
	double matrizReflexao[3][3] = {
			{(double)Rx, 0.0, 0.0}, 
		    {0.0, (double)Ry, 0.0},
		    {0.0, 0.0, 1.0} 	
	};
	
	// Para cada forma, obter a matriz de transformacao final após calcular o centróide da forma
	for(forward_list<forma>::iterator f = formas.begin(); f != formas.end(); f++){
		vertice centroide = calcularCentroide(*f); 		// Chama a funcao e obtém um vertice
		
		double transformacaoReflexao[3][3];
		calcularMatrizTransformacao(centroide, matrizReflexao, transformacaoReflexao);
		aplicarTransformacaoVertices(*f, transformacaoReflexao);
	}
}

void cisalhamento(float Cx, float Cy){
	double matrizCisalhamento[3][3] = {
			{1.0, Cy, 0.0}, 
		    {Cx, 1.0, 0.0},
		    {0.0, 0.0, 1.0} 
	};
	
	// Para cada forma, obter a matriz de transformacao final após calcular o centróide da forma
	for(forward_list<forma>::iterator f = formas.begin(); f != formas.end(); f++){
		vertice centroide = calcularCentroide(*f); 		// Chama a funcao e obtém um vertice
		
		double transformacaoCisalhamento[3][3];
		calcularMatrizTransformacao(centroide, matrizCisalhamento, transformacaoCisalhamento);
		aplicarTransformacaoVertices(*f, transformacaoCisalhamento);
	}
}

void escala(float Sx, float Sy){
	double matrizEscala[3][3] = {
			{Sx, 0.0, 0.0}, 
		    {0.0, Sy, 0.0},
		    {0.0, 0.0, 1.0}
	};
	
	// Para cada forma, obter a matriz de transformacao final após calcular o centróide da forma
	for(forward_list<forma>::iterator f = formas.begin(); f != formas.end(); f++){
		vertice centroide = calcularCentroide(*f); 		// Chama a funcao e obtém um vertice
				
		double transformacaoEscala[3][3];
		calcularMatrizTransformacao(centroide, matrizEscala, transformacaoEscala);
		aplicarTransformacaoVertices(*f, transformacaoEscala);
	}
}

void rotacao(float angulo){
	double matrizRotacao[3][3] = {
			{std::cos(angulo), std::sin(angulo), 0.0},
			{-std::sin(angulo), std::cos(angulo), 0.0},
			{0.0, 0.0, 1.0}
	};
	
	// Para cada forma, obter a matriz de transformacao final após calcular o centróide da forma
	for(forward_list<forma>::iterator f = formas.begin(); f != formas.end(); f++){
		vertice centroide = calcularCentroide(*f); 		// Chama a funcao e obtém um vertice
		
		double transformacaoRotacao[3][3];
		calcularMatrizTransformacao(centroide, matrizRotacao, transformacaoRotacao);
		aplicarTransformacaoVertices(*f, transformacaoRotacao);
	}
}

// unica transformacao que nao e feita com coordenadas homogeneas nesse codigo
void translacao(int dx, int dy){
	int i = 0;
	
	for(forward_list<forma>::iterator f = formas.begin(); f != formas.end(); f++){
		for(forward_list<vertice>::iterator v = f->v.begin(); v != f->v.end(); v++, i++){
			printf("(%d, %d) -> (%d, %d)\n", v->x, v->y, v->x+dx, v->y+dy);
			v->x += dx;  // Move o vértice na direcao x
		    v->y += dy;	 // Move o vértice na direcao y
		}
		printf("\n");
	}
}	

// Controle da posicao do cursor do mouse
void mousePassiveMotion(int x, int y){
    m_x = x; m_y = height - y - 1;
    glutPostRedisplay();
}

// Funcao para desenhar apenas um pixel na tela
void drawPixel(int x, int y){
	glPointSize(2);
    glBegin(GL_POINTS); 	// Seleciona a primitiva GL_POINTS para desenhar
        glVertex2i(x, y);
    glEnd();  				// indica o fim do ponto
}

// Funcao que desenha a lista de formas geometricas
void drawFormas(){
	// Visualizacao previa
	if (modo == LIN){
    	if(click1) algoritmoBresenham(x_1, y_1, m_x, m_y);
	}
	if (modo == RET) {
        if (click1) {
            algoritmoBresenham(x_1, y_1, m_x, y_1); // Superior
            algoritmoBresenham(m_x, y_1, m_x, m_y); // Direita
            algoritmoBresenham(m_x, m_y, x_1, m_y); // Inferior
            algoritmoBresenham(x_1, m_y, x_1, y_1); // Esquerda
        }
    }
    if (modo == TRI){
		if(click1) algoritmoBresenham(x_1, y_1, m_x, m_y);
		if(click2){
			algoritmoBresenham(x_2, y_2, m_x, m_y);
			algoritmoBresenham(x_1, y_1, x_2, y_2);
		}
	}
	if (modo == POL){
		if (poligonoIniciado){
			if (click1) algoritmoBresenham(x_1, y_1, m_x, m_y);
			else algoritmoBresenham(x_2, y_2, m_x, m_y);
		}	
	}
	if (modo == CIR){
		if (click1){
		    int dx = m_x - x_1, dy = m_y - y_1;					// deslocamento em x e y do centro ate a posicao do mouse
			algoritmoBresenham(x_1, y_1, m_x, m_y);				// raio na direcao do mouse
			algoritmoBresenham(x_1, y_1, x_1 - dx, y_1 - dy);	// raio na direcao oposta (centro - deslocamento realizado)
		}
	}
    
    //Percorre a lista de formas geometricas para desenhar
    for(forward_list<forma>::iterator f = formas.begin(); f != formas.end(); f++){
    	int i = 0;
    	vector<double> x, y;
        switch (f->tipo) {
            case LIN:
                //Percorre a lista de vertices da forma linha para desenhar
                for(forward_list<vertice>::iterator v = f->v.begin(); v != f->v.end(); v++, i++){
                    x.push_back(v->x);
                    y.push_back(v->y);
                }
                //Desenha o segmento de reta apos dois cliques
                algoritmoBresenham(x[0], y[0], x[1], y[1]);
                
            	break;
			case RET:
				for(forward_list<vertice>::iterator v = f->v.begin(); v != f->v.end(); v++, i++){
                    x.push_back(v->x);
                    y.push_back(v->y);
                }
                
            	// Desenha retangulo
                algoritmoBresenham(x[0], y[0], x[2], y[2]); // superior
				algoritmoBresenham(x[2], y[2], x[1], y[1]); // direita
				algoritmoBresenham(x[1], y[1], x[3], y[3]); // inferior
				algoritmoBresenham(x[3], y[3], x[0], y[0]); // esquerda
                break;
            case TRI:
            	for(forward_list<vertice>::iterator v = f->v.begin(); v != f->v.end(); v++, i++){
                    x.push_back(v->x);
                    y.push_back(v->y);
                }
                
                algoritmoBresenham(x[0], y[0], x[1], y[1]);
				algoritmoBresenham(x[1], y[1], x[2], y[2]);
        		algoritmoBresenham(x[0], y[0], x[2], y[2]);
        		break;
        	case CIR:
                for (forward_list<vertice>::iterator v = f->v.begin(); v != f->v.end(); v++) {
                    drawPixel(v->x, v->y);  // desenha cada ponto armazenado em rasterizaCircunferencia
                }
                break;
        	case POL:
			    for (forward_list<vertice>::iterator v = f->v.begin(); v != f->v.end(); v++, i++) {
			        x.push_back(v->x);
			        y.push_back(v->y);
			    }
			    size_t n = x.size();
			    for (size_t i = 0; i < n; i++) {
			    	// desenhar linha entre o vertice atual e o seguinte
					// a operacao modulo garante que seja desenhado entre o ultimo e primeiro vertice
			        algoritmoBresenham(x[i], y[i], x[(i + 1) % n], y[(i + 1) % n]);
			    }
			    break;	
        }
    }
}

void algoritmoBresenham(double x1, double y1, double x2, double y2) {
    // passo 1: definir booleanos declive e simetrico
    bool declive = false, simetrico = false;	
    
    // passo 2: calcular deltas
    double delta_x = x2 - x1;
    double delta_y = y2 - y1;
    
    // passo 3: testar o produto entre os deltas
    if(delta_x * delta_y < 0){
		// substituir os valores de y e delta_y pelo seu simétrico
		delta_y *= -1;
		y1 *= -1;
		y2 *= -1;
		simetrico = true; // registrar que fez a troca pelo simétrico
	}
	
	// passo 4: testar se o valor absoluto de delta x é menor que o valor absoluto de delta y
	if(abs(delta_x) < abs(delta_y)){
        swap(x1, y1);			// trocar coordenadas x e y dos pontos
        swap(x2, y2);
        swap(delta_x, delta_y);	// trocar os valores de delta x e delta y
        declive = true; 		// atribuir verdadeiro ao declive
	}
	
	// passo 5: verificar se o valor da coordenada x do primeiro extremo é superior ao valor da mesma coordenada do segundo extremo
	if (x1 > x2) {
	    swap(x1, x2);	// inverter os extremos
	    swap(y1, y2);
	    delta_x *= -1;	// substituir delta x e delta y pelos respectivos valores simétricos
	    delta_y *= -1;
    }
	
	// variaveis para o algoritmo de Bresenham
	double x, y, decisao, incE, incNE;
	
	// ponto inicial
    x = x1;
    y = y1;

    decisao = 2 * abs(delta_y) - abs(delta_x);
    incE = 2 * abs(delta_y);
    incNE = 2 * (abs(delta_y) - abs(delta_x));

	// rasterizacao ponto a ponto
	for (; x <= x2; ++x) {
		int x_transformado = (int)x;
		int y_transformado = (int)y;
		
		if(declive) swap(x_transformado, y_transformado);
		if(simetrico) y_transformado *= -1;
		
		drawPixel(x_transformado, y_transformado);
	
		if(decisao <= 0) decisao += incE;
		else{
			decisao += incNE;
			y += (delta_y > 0) ? 1 : -1;
		}
	}
}

void rasterizaCircunferencia(int xc, int yc, double raio) {
    // calcular variaveis necessarias
    int d = 1 - raio, dE = 3, dSE = -2 * raio + 5;
    pushForma(CIR);
    
    int xi = 0, yi = (int)raio;
    
    while (yi > xi) {
        if (d < 0) {
            d += dE;
            dE += 2;
            dSE += 2;
        } else {
            d += dSE;
            dE += 2;
            dSE += 4;
            yi--;
        }
        xi++;
               
  		// 8 pontos simetricos
        pushVertice(xc + xi, yc + yi); 
        pushVertice(xc - xi, yc + yi); 
        pushVertice(xc + xi, yc - yi); 
        pushVertice(xc - xi, yc - yi); 
        pushVertice(xc + yi, yc + xi); 
        pushVertice(xc - yi, yc + xi); 
        pushVertice(xc + yi, yc - xi); 
        pushVertice(xc - yi, yc - xi); 
    }
}