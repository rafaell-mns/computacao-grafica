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

//Enumeracao com os tipos de formas geometricas
enum tipo_forma{LIN = 1, QUA = 2, TRI = 3, POL = 4, CIR = 5}; // Linha, Triangulo, Retangulo Poligono, Circulo
enum tipo_transf{TRA = 6, ROT = 7};

// Verificacoes booleanas
bool click1 = false, click2 = false; 	// clique do mouse
bool movido = false;					// controle da translacao
bool calculado = false;					// controle para centroide e outros calculos

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
void drawTransformacoes(int dx, int dy);
void translacao(int dx, int dy);
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
	
	// Menu principal
	glutCreateMenu(menu_popup);
	glutAddSubMenu("Desenhar", menu_desenhar);
	glutAddSubMenu("Transformar", menu_transformacao);
	glutAddMenuEntry("Sair", 0);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	
	printf("Por padrao, LINHA e TRANSLACAO estao previamente selecionados\n\n");
	printf("Teclas:\n");
	printf("  'C' - Limpar a tela\n");
	printf("  'Z' - Apagar a ultima forma desenhada\n");
	printf("  'WASD' - Controlar a translacao\n\n");

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
	int dx = 0, dy = 0;
    glClear(GL_COLOR_BUFFER_BIT); 				//Limpa o buffer de cores e reinicia a matriz
    glColor3f (0.0, 0.0, 0.0); 					// Seleciona a cor default como preto
    drawFormas(); 								// Desenha as formas geometricas da lista
    drawTransformacoes(dx, dy);
    //Desenha texto com as coordenadas da posicao do mouse
    draw_text_stroke(0, 0, "(" + to_string(m_x) + "," + to_string(m_y) + ")", 0.2);
    glutSwapBuffers(); 							// manda o OpenGl renderizar as primitivas
}

// Controla o menu pop-up
void menu_popup(int value){
    if (value == 0) exit(EXIT_SUCCESS);
    modo = value;
	operacao = value;
}

// Controle das teclas comuns do teclado
void keyboard(unsigned char key, int x, int y){
    switch (key) { // key - variável que possui valor ASCII da tecla pressionada
        case ESC: 
            exit(EXIT_SUCCESS); 
            break;
        case 'c':
        case 'C':
            formas.clear();
            printf("Tela limpada!\n\n");
            glutPostRedisplay();
            break;
        case 'z':
	    case 'Z':
	    	if(!formas.empty()){
			  formas.pop_front();
	    	  printf("Ultima forma desfeita!\n\n");
			  glutPostRedisplay();	
			} 
            break;
        case 'w':  // Mover para cima
            movido = false;
            printf("Translacao para cima\n");
            translacao(0, 25);
            break;
        case 's': // Mover para baixo
            movido = false;
			printf("Translacao para baixo\n");
            translacao(0, -25);
            break;
        case 'a': // Mover para a esquerda
            movido = false;
            printf("Translacao para esquerda\n");            
            translacao(-25, 0);
            break;
        case 'd': // Mover para a direita
            movido = false;
            printf("Translacao para direita\n");            
            translacao(25, 0);
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
	if(!calculado){
		
		for(forward_list<vertice>::iterator v = f.v.begin(); v != f.v.end(); v++, i++){
	        somaX += v->x;
	        somaY += v->y;
	        ++n;
	    }
	    calculado = true;
	}
	if (n == 0) return vertice{0, 0}; // Retorna {0, 0} se não houver vértices
 	 return vertice{somaX / n, somaY / n}; // Retorna o centroide calculado	
}

void rotacao(){
	for(forward_list<forma>::iterator f = formas.begin(); f != formas.end(); f++){
		vertice centroide = calcularCentroide(*f); // Chama a função e obtém um vertice
        printf("Centroide: (%d, %d)\n", centroide.x, centroide.y); // Exibe o centroide
	}
	calculado = true;
}

// Transformacoes
void translacao(int dx, int dy){
	int i = 0;
	if(!movido){
		for(forward_list<forma>::iterator f = formas.begin(); f != formas.end(); f++){
			for(forward_list<vertice>::iterator v = f->v.begin(); v != f->v.end(); v++, i++){
			    v->x += dx;  	// Move o vértice em x
			    v->y += dy;		// Move o vértice em y
				printf("(x', y') = (%d, %d)\n", v->x, v->y);
			}
			printf("\n");
		}
		movido = true;
	}
}

void drawTransformacoes(int dx, int dy){
	switch(operacao){
	case TRA:
		translacao(dx, dy);
		break;
	case ROT:
		rotacao();
		break;
	default:
		break;
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