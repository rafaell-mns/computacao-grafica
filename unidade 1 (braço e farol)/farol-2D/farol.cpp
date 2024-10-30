#ifdef __APPLE__ // MacOS
    #define GL_SILENCE_DEPRECATION
    #include <GLUT/glut.h>
    #include <OpenGL/gl.h>
    #include <OpenGL/glu.h>
#else // Windows e Linux
    #include <GL/glut.h>
    #include <GL/gl.h>
    #include <GL/glu.h>
#endif

#include <math.h>

float R, G, B; // Vari�veis globais para defini��o de cores

#define ALTURA 70 // Altura de cada trap�zio
#define POS_X 256 // Coordenada x central

void init(void);
void keyboard(unsigned char key, int x, int y);
void display(void);
void reshape(int w, int h);
void drawSymmetricTrapezoid(float x, float y, float baseWidth, float topWidth, float height, float r, float g, float b);
void drawRectangle(float x, float y, float width, float height, float r, float g, float b);
void drawHalfCircle(float x, float y, float radius, float r, float g, float b);
void drawCircle(float x, float y, float radius, float r, float g, float b);
void drawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, float r, float g, float b);

int main(int argc, char** argv) {
    glutInit(&argc, argv);                         // Inicializa GLUT
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);   // Define o modo de display
    glutInitWindowSize(512, 512);                  // Tamanho da janela
    glutInitWindowPosition(100, 100);              // Posi��o inicial da janela do OpenGL
    glutCreateWindow("Desenho Farol 2D");          // Cria a janela
    init();                                        // Inicializa configura��es do OpenGL

    // Registra fun��es de callback
    glutDisplayFunc(display);                      // Fun��o de desenho
    glutReshapeFunc(reshape);                      // Fun��o de redimensionamento
    glutKeyboardFunc(keyboard);                    // Fun��o para teclas comuns
  
    glutMainLoop();  // Inicia o loop principal do GLUT
    return 0;
}

void init(void) {
    glClearColor(1.0, 1.0, 1.0, 1.0); // Limpa a tela com a cor branca
    R = 0.0; G = 0.0; B = 0.0;         // Inicializa cor como preto
}

void keyboard(unsigned char key, int x, int y) {
    if (key == 27) exit(0); // ESC encerra o programa
}

void reshape(int w, int h) {
    glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, w, h);
    glOrtho (0, 512, 0, 512, -1 ,1);  
	glMatrixMode(GL_MODELVIEW);
}

void drawSymmetricTrapezoid(float x, float y, float baseWidth, float topWidth, float height, float r, float g, float b) {
    glColor3f(r, g, b); // Define a cor

    float halfBaseWidth = baseWidth / 2.0;
    float halfTopWidth = topWidth / 2.0;

    glBegin(GL_QUADS);
        glVertex2f(x - halfBaseWidth, y);        // V�rtice inferior esquerdo
        glVertex2f(x + halfBaseWidth, y);         // V�rtice inferior direito
        glVertex2f(x + halfTopWidth, y + height); // V�rtice superior direito
        glVertex2f(x - halfTopWidth, y + height);  // V�rtice superior esquerdo
    glEnd();
}

void drawRectangle(float x, float y, float width, float height, float r, float g, float b) {
    // Define a cor com os valores RGB passados como par�metro
    glColor3f(r, g, b);

    // Desenha o ret�ngulo centralizado em rela��o ao eixo X
    glBegin(GL_QUADS);
        glVertex2f(x - width / 2, y);              // V�rtice inferior esquerdo
        glVertex2f(x + width / 2, y);              // V�rtice inferior direito
        glVertex2f(x + width / 2, y + height);     // V�rtice superior direito
        glVertex2f(x - width / 2, y + height);     // V�rtice superior esquerdo
    glEnd();
}

void drawHalfCircle(float x, float y, float radius, float r, float g, float b) {
    int num_segments = 100;  // Define o n�mero de segmentos para suavizar o c�rculo
    float theta;
    
    glColor3f(r, g, b);  // Define a cor do semic�rculo
    glBegin(GL_TRIANGLE_FAN);  // Usamos TRIANGLE_FAN para desenhar o semic�rculo

    glVertex2f(x, y);  // Centro do semic�rculo

    // Desenhar o arco do semic�rculo (apenas metade do c�rculo)
    for (int i = 0; i <= num_segments; i++) {
        theta = i * M_PI / num_segments;  // M_PI � a constante para pi

        float cx = radius * cos(theta);  // Coordenada X do ponto
        float cy = radius * sin(theta);  // Coordenada Y do ponto

        glVertex2f(x + cx, y + cy);
    }

    glEnd();
}

void drawCircle(float x, float y, float radius, float r, float g, float b) {
    int num_segments = 100;  // N�mero de segmentos para suavizar o c�rculo
    float theta;

    glColor3f(r, g, b);  // Define a cor do c�rculo
    glBegin(GL_TRIANGLE_FAN);  // Usamos TRIANGLE_FAN para desenhar o c�rculo

    glVertex2f(x, y);  // Centro do c�rculo

    // Desenhar os pontos ao longo da circunfer�ncia do c�rculo
    for (int i = 0; i <= num_segments; i++) {
        theta = 2.0f * M_PI * i / num_segments;  // �ngulo em radianos

        float cx = radius * cos(theta);  // Coordenada X do ponto
        float cy = radius * sin(theta);  // Coordenada Y do ponto

        glVertex2f(x + cx, y + cy);
    }

    glEnd();
}

void drawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, float r, float g, float b) {
    glColor3f(r, g, b);  // Define a cor do tri�ngulo

    glBegin(GL_TRIANGLES);  // Inicia o desenho de tri�ngulos
        glVertex2f(x1, y1);  // Primeiro v�rtice
        glVertex2f(x2, y2);  // Segundo v�rtice
        glVertex2f(x3, y3);  // Terceiro v�rtice
    glEnd();
}

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);   // Limpa a tela
    glLoadIdentity();               // Reseta a matriz de visualiza��o

	int pos_y = 20;
	int largura_base = 270;
	int largura_topo = 235;
	int reducao = largura_base - largura_topo;
	
	// Desenhar 5 trap�zios alternando a cor
	for (int i = 0; i < 5; i++) {
	    if (i % 2 == 0) {
	        drawSymmetricTrapezoid(POS_X, pos_y, largura_base, largura_topo, ALTURA, 1.0, 0.0, 0.0); // Trap�zio vermelho
	    } else {
	        drawSymmetricTrapezoid(POS_X, pos_y, largura_base, largura_topo, ALTURA, 0.83, 0.83, 0.83); // Trap�zio cinza
		}
	
	    // Atualiza a posi��o e as larguras para o pr�ximo trap�zio
	    pos_y = pos_y + ALTURA;
	    largura_base = largura_base - reducao;
	    largura_topo = largura_topo - reducao;
	}
	
	int altura_topo = 30;
	
	// Topo do farol
  	drawRectangle(POS_X, pos_y, largura_base, ALTURA - altura_topo, 0.66, 0.66, 0.66); // Ret�ngulo cinza escuro
  	drawRectangle(POS_X, pos_y + ALTURA - altura_topo, largura_base + 30, ALTURA - 55, 0.66, 0.66, 0.66); // Telhado
  	drawRectangle(POS_X - 21, pos_y, largura_base/3, ALTURA - altura_topo, 1, 1, 0); // Janela amarela esquerda
	drawRectangle(POS_X + 21, pos_y, largura_base/3, ALTURA - altura_topo, 1, 1, 0); // Janela amarela direita
	drawSymmetricTrapezoid(POS_X, pos_y + ALTURA - altura_topo + ALTURA - 55, largura_base, largura_topo + 20, 35, 0.83, 0.83, 0.83); // Trap�zio cinza
	drawHalfCircle(POS_X, pos_y + ALTURA - altura_topo + ALTURA - 20, 15, 0.66, 0.66, 0.66); 
  
    // Porta principal
    drawRectangle(POS_X, 20, 36, 45, 0.36, 0.82, 0.82); // Ret�ngulo pra porta
    drawHalfCircle(POS_X, 65, 18.5, 0.36, 0.82, 0.82); // Arco do topo
    
    // Janela azul
    drawCircle(POS_X, 266, 26, 0, 0, 0); // Borda preta
    drawCircle(POS_X, 266, 21, 0.36, 0.82, 0.82); // Janela azul
    
    // Luz do farol
    int x1_luz_r = POS_X + largura_base/2; // Coordenada x para luz direita (right)
    int x1_luz_l = POS_X - largura_base/2; // Coordenada y para luz esquerda (left)
    int y1_luz = pos_y + (ALTURA - altura_topo)/2; // Coordenada y
    
    drawTriangle(x1_luz_r, y1_luz, x1_luz_r + 160, y1_luz + 40, x1_luz_r + 160, y1_luz - 40, 0.976, 0.804, 0.282); // Luz direita
    drawTriangle(x1_luz_l, y1_luz, x1_luz_l - 160, y1_luz + 40, x1_luz_l - 160, y1_luz - 40, 0.976, 0.804, 0.282); // Luz esquerda
    
    glFlush();  // Envia os comandos para serem executados
}
