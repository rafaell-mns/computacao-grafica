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
#define PI 3.14159265358979323846 // Definindo o valor de PI manualmente

//Enumeracao com os tipos de formas geometricas
enum tipo_forma{LIN = 1, QUA = 2, TRI = 3, POL = 4, CIR = 5}; // Linha, Triangulo, Retangulo Poligono, Circulo
enum tipo_transf{TRA = 6, ROT = 7, ESCA = 8, CIS = 9, REF = 10};

// Verificacoes booleanas
bool click1 = false, click2 = false; 	// clique do mouse

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

void pushQuadrilatero(int x1, int y1, int x2, int y2){
	pushForma(QUA);
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
void drawFormas();	// Funcao que percorre a lista de formas geometricas, desenhando-as na tela
void translacao(int dx, int dy);		// Declara translacao
void rotacao(float angulo);				// Declara rotacao
void escala(float Sx, float Sy);		// Declara escala
void cisalhamento(float Cx, float Cy);	// Declara cisalhamento
void reflexao(int Rx, int Ry);					// Declara reflexao		
void algoritmoBresenham (double x1,double y1,double x2,double y2);


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
	glutAddMenuEntry("Quadrilatero", QUA);
	glutAddMenuEntry("Triangulo", TRI);
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
	
	printf("Por padrao, LINHA e TRANSLACAO estao previamente selecionados\n\n");
	printf("Teclas:\n");
	printf("  'C' - Limpar a tela\n");
	printf("  'Z' - Apagar a ultima forma desenhada\n\n");
	printf("Transformacoes (apos selecionar no menu):\n");
	printf("  'WASD' - Direcao da translacao\n");
	printf("  'R' - Rotacao\n");
	printf("  'WS' - Aumentar/diminuir a escala\n");
	printf("  'XY' - Cisalhamento no eixo X ou Y\n");
	printf("  'XYO' - Reflexao sobre o eixo X, o eixo Y ou a origem do sistema\n");
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

int deslocamento = 30;
int angulo = 45.0; // ângulo em graus
float anguloEmRadianos = angulo * (PI / 180.0);
float fatorEscala = 2;
float Cx = 3.5;
float Cy = 1.5;

// Controle das teclas comuns do teclado
void keyboard(unsigned char key, int x, int y){
    switch (key) { // key - variável que possui valor ASCII da tecla pressionada
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
        case 'W':
        case 'w': // Transformações geométricas
		    switch (operacao) {
		        case TRA:
		            if(!formas.empty()){
						printf("Translacao para cima\n");
		            	translacao(0, deslocamento);
					} 
		            break;
		        case ESCA:
		        	if(!formas.empty()){
		        		printf("Formas aumentadas em 2x\n");
		        		escala(fatorEscala,fatorEscala);
					
					}
		    }
		    break;
		case 'S':
        case 's':
        	switch (operacao) {
		        case TRA:
		            if(!formas.empty()){
						printf("Translacao para baixo\n");
		            	translacao(0, -deslocamento);
					} 
		            break;
		        case ESCA:
		        	if(!formas.empty()){
		        		printf("Formas diminuidas em 2x\n");
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
						printf("Translacao para esquerda\n");
		            	translacao(-deslocamento, 0);
					} 
		            break;
		    }
		    break;
        case 'D':
        case 'd':
        	switch (operacao) {
		        case TRA:
		            if(!formas.empty()){
						printf("Translacao para direita\n");
		            	translacao(deslocamento, 0);
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
						rotacao(anguloEmRadianos);
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
            switch(modo){
                case LIN:
                    if (state == GLUT_DOWN) {
                        if(click1){
                            x_2 = x;
                            y_2 = height - y - 1;
                            printf("Clique 2 (%d, %d)\n\n",x_2,y_2);
                            pushLinha(x_1, y_1, x_2, y_2);
                            click1 = false;
                            glutPostRedisplay();
                        }else{
                            click1 = true;
                            x_1 = x;
                            y_1 = height - y - 1;
                            printf("Linha\n");
                            printf("Clique 1 (%d, %d)\n",x_1,y_1);
                        }
                    }
                	break;
            	case QUA:
            		if (state == GLUT_DOWN) {
				        if (click1) {
				            x_2 = x;
				            y_2 = height - y - 1; 
				            printf("Clique 2 (%d, %d)\n\n", x_2, y_2);
				            pushQuadrilatero(x_1, y_1, x_2, y_2);
				            click1 = false;
				            glutPostRedisplay();
				        } else {
				        	click1 = true;
				            x_1 = x;
				            y_1 = height - y - 1; 
				            printf("Quadrilatero\nClique 1 (%d, %d)\n", x_1, y_1);
				        }
			    	}
   					break;
   				case TRI:
   					if (state == GLUT_DOWN){
						if(!click1){
							click1 = true;
							x_1 = x;
				            y_1 = height - y - 1;
				            printf("Triangulo\nClique 1 (%d, %d)\n", x_1, y_1);
						} else if(!click2){
							click2 = true;
							x_2 = x;
				            y_2 = height - y - 1; 
				            printf("Clique 2 (%d, %d)\n", x_2, y_2);
						} else{
							click1 = click2 = false;
							x_3 = x;
				            y_3 = height - y - 1;
				            printf("Clique 3 (%d, %d)\n\n", x_3, y_3);
				            pushTriangulo(x_1, y_1, x_2, y_2, x_3, y_3);
						}
					} 
					break;
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
 
	if (n == 0) return vertice{0, 0}; // Retorna {0, 0} se não houver vértices
 	 return vertice{somaX / n, somaY / n}; // Retorna o centroide calculado	
}

void multiplicarMatrizes(double a[3][3], double b[3][3], double resultado[3][3]) {
	// Inicializar matriz resultado
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            resultado[i][j] = 0;
        }
    }

    // Multiplicação de matrizes
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            for (int k = 0; k < 3; k++) {
                resultado[i][j] += a[i][k] * b[k][j]; // Soma dos produtos
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
    multiplicarMatrizes(transladaParaOrigem, operacao, operacaoNaOrigem); 			// realiza a operação na origem
    multiplicarMatrizes(operacaoNaOrigem, transladaDeVolta, matrizTransformacao);	// retorna para onde estava
}

void aplicarTransformacaoVertices(forma& f, double matrizTransformacao[3][3]){
	int i = 0;
	for(forward_list<vertice>::iterator v = f.v.begin(); v != f.v.end(); v++, i++){
		double coordHomogenea[3] = {(double)v->x, (double)v->y, 1.0}; // converte para coordenada homogênea
		double verticeResultante[3];
		
		multiplicarVetorPorMatriz(coordHomogenea, matrizTransformacao, verticeResultante);
		
		printf("(%d, %d) -> (%d, %d)\n", v->x, v->y, (int)round(verticeResultante[0]), (int)round(verticeResultante[1]));
			
        // Atualiza as coordenadas do vértice
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
		vertice centroide = calcularCentroide(*f); 		// Chama a função e obtém um vertice
		
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
		vertice centroide = calcularCentroide(*f); 		// Chama a função e obtém um vertice
		
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
		vertice centroide = calcularCentroide(*f); 		// Chama a função e obtém um vertice
				
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
		vertice centroide = calcularCentroide(*f); 		// Chama a função e obtém um vertice
		
		double transformacaoRotacao[3][3];
		calcularMatrizTransformacao(centroide, matrizRotacao, transformacaoRotacao);
		aplicarTransformacaoVertices(*f, transformacaoRotacao);
	}
}

void translacao(int dx, int dy){
	int i = 0;
	
	for(forward_list<forma>::iterator f = formas.begin(); f != formas.end(); f++){
		for(forward_list<vertice>::iterator v = f->v.begin(); v != f->v.end(); v++, i++){
			printf("(%d, %d) -> (%d, %d)\n", v->x, v->y, v->x+dx, v->y+dy);
			v->x += dx;  // Move o vértice na direção x
		    v->y += dy;	 // Move o vértice na direção y
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
	if (modo == QUA) {
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
			case QUA:
				for(forward_list<vertice>::iterator v = f->v.begin(); v != f->v.end(); v++, i++){
                    x.push_back(v->x);
                    y.push_back(v->y);
                }
                
            	// Desenha quadrilatero
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
        }
    }
}

void algoritmoBresenham(double x1, double y1, double x2, double y2) {
    double delta_x, delta_y, desvio, x, y, incE, incNE;
    int sinal_x, sinal_y;
    bool trocado = false;
    
	delta_x = x2 - x1;
    delta_y = y2 - y1;

	// obter sinal para incrementar depois
    sinal_x = (delta_x >= 0) ? 1 : -1;
    sinal_y = (delta_y >= 0) ? 1 : -1;

	// ponto inicial
    x = x1;
    y = y1;

	// trocar delta x e delta y
    if (abs(delta_y) > abs(delta_x)) { 
        double temp_delta = delta_x;
        delta_x = delta_y;
        delta_y = temp_delta;
        trocado = true;
    }

    desvio = 2 * abs(delta_y) - abs(delta_x);
    incE = 2 * abs(delta_y);
    incNE = 2 * abs(delta_y) - 2 * abs(delta_x);

    drawPixel((int)x, (int)y);

    for (int i = 0; i <= abs(delta_x); i++) {
        if (desvio <= 0) {
            if (!trocado) x += sinal_x;
            else y += sinal_y;
            desvio += incE;
        }else {
            x += sinal_x;
            y += sinal_y;
            desvio += incNE;
        }
        drawPixel((int)x, (int)y);
    }
}