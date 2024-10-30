/*
 * Computacao Grafica
 * Codigo Exemplo: Braco Robotico usando GLUT/OpenGL
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

#include <stdio.h>
#include <stdlib.h>

#define ESC 27

/*
 * Declaracao de constantes e variaveis
 */
static int ombro = 0, cotovelo = 0, rotacaoY = 0, mao = 0, indicador = 0, anelar = 0, polegar = 0;

/*
 * Declaracoes antecipadas (forward) das funcoes (assinaturas)
 */
void init(void);
void keyboard (unsigned char key, int x, int y);
void display(void);
void reshape (int w, int h);

//Fucao principal
int main(int argc, char** argv){
    // inicia o GLUT
    glutInit(&argc,argv);

    // inicia o display usando RGB, double-buffering e z-buffering
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(800,600);
    glutInitWindowPosition(100,100);
    glutCreateWindow("Computacao Grafica: Braco Robotico");

    // Funcao com alguns comandos para a inicializacao do OpenGL
    init ();

    // define as funcoes de callback
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMainLoop();

  return EXIT_SUCCESS;
}

// Funcao com alguns comandos para a inicializacao do OpenGL
void init(void){
    glClearColor (1.0, 1.0, 1.0, 1.0); //Limpa a tela com a cor branca;
    glEnable(GL_DEPTH_TEST); // Habilita o algoritmo Z-Buffer
}

// Funcao callback para o reshape da janela
void reshape (int w, int h){
    // muda para o modo GL_PROJECTION e reinicia a matriz de projecao
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity();

    // define o tamanho da area de desenho da janela
    glViewport (0, 0, (GLsizei) w, (GLsizei) h);

    // Define a forma do volume de visualizacao para termos
    // uma projecao perspectiva (3D).
    gluPerspective(60, (float)w/(float)h , 1.0, 9.0); //(angulo, aspecto, ponto_proximo, ponto distante)
    gluLookAt(0.0,0.0,7.0,  // posicao da camera (olho)
              0.0,1.0,0.0,  // direcao da camera (geralmente para centro da cena)
              0.0,1.0,0.0); // sentido ou orientacao da camera (de cabeca para cima)
    // muda para o modo GL_MODELVIEW para desenhar na tela
    glMatrixMode (GL_MODELVIEW);

}

// Fucao callback para controle das teclas comuns
void keyboard (unsigned char key, int x, int y){
	switch (key) {
		case 'o': ombro = (ombro - 5) % 360; break; // sentido horario
		case 'O': ombro = (ombro + 5) % 360; break; // sentido anti-horario
		case 'c': cotovelo = (cotovelo - 5) % 360; break; // sentido horario
		case 'C': cotovelo = (cotovelo + 5) % 360; break; // sentido anti-horario
		case 'y': rotacaoY = (rotacaoY - 5) % 360; break; // rotação horária em torno do eixo Y
        case 'Y': rotacaoY = (rotacaoY + 5) % 360; break; // rotação anti-horária em torno do eixo Y
	    case 'm': mao = (mao - 5) % 360; break; // sentido horario
 		case 'M': mao = (mao + 5) % 360; break; // sentido anti-horario
        case 'i': indicador = (indicador - 5) % 360; break; // sentido horario
 		case 'I': indicador = (indicador + 5) % 360; break; // sentido anti-horario
		case 'a': anelar = (anelar - 5) % 360; break; // sentido horario
 		case 'A': anelar = (anelar + 5) % 360; break; // sentido anti-horario
		case 'p': polegar = (polegar - 5) % 360; break; // sentido horario
 		case 'P': polegar = (polegar + 5) % 360; break; // sentido anti-horario
		case ESC: exit(EXIT_SUCCESS); break; // sai do programa
	}
    glutPostRedisplay();
}

// Funcao callback para desenhar na janela
void display(void){
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Limpa o Buffer de Cores
    glLoadIdentity();
    
    // Base
   glPushMatrix();
       glTranslatef(0.0, -2.0, 0.0);
       glRotatef(-90, 1, 0, 0);
       glColor3f(0.1, 0.1, 0.1);
       glutSolidCone(1.1, 1.1, 30, 30); // base, altura, fatias, pilhas
   glPopMatrix();
    
    // Movimento do braco
    glPushMatrix();
        glTranslatef (0.0, -1.0, 0.0); // origem posicionada no ombro
        glRotatef ((GLfloat) ombro, 0.0, 0.0, 1.0);
		glRotatef(rotacaoY, 0.0, 1.0, 0.0); // Rotaciona em torno do eixo Y
        glTranslatef (0.0, 1.0, 0.0); // origem posicionada no centro do braco
        
        // Ombro (base-braco)
		   glPushMatrix();
		        glTranslatef(0.0, -1.0, 0.0);
				glRotatef(rotacaoY, 0.0, 1.0, 0.0); // Rotaciona em torno do eixo Y
		        glColor3f(0.5, 0.5, 0.5); // desenha
		      glutSolidSphere(0.5, 30, 30); // raio, fatias, pilhas
		   glPopMatrix();
        
        // Braco
        glPushMatrix();
            glRotatef(90, 0, 0, 1); // horizontal -> vertical
            glScalef (2.0, 0.5, 0.5); // cubo -> retângulo
            glColor3f(0.0,0.4,0.8);
            glutSolidCube (1.0); // Tamanho
        glPopMatrix();
        
		// Cotovelo (braco-antebraco)
	       glPushMatrix();
				glTranslatef(0.0, 1.0, 0.0);
				glColor3f(0.5, 0.5, 0.5); // desenha
				glutSolidSphere(0.4, 30, 30); // raio, fatias, pilhas
	       glPopMatrix();
       
        // Movimento do antebraco
       glPushMatrix();
            glTranslatef (0.0, 1.0, 0.0); //origem posicionada no cotovelo
           glRotatef ((GLfloat) cotovelo, 0.0, 0.0, 1.0);
            glTranslatef (0.0, 1.0, 0.0);
            
   	   // Punho (antebraco-mao)
	       glPushMatrix();
				glTranslatef(0.0, 1.0, 0.0); // posiciona
	           glColor3f(0.5, 0.5, 0.5); // desenha
	           glutSolidSphere(0.3, 30, 30); // raio, fatias, pilhas
	       glPopMatrix();
	       
		// Antebraco
           glPushMatrix();
                glRotatef(90, 0, 0, 1);
                glScalef (2.0, 0.4, 0.4);
               glColor3f(0.0,0.8,0.2);
            	glutSolidCube (1.0); // Tamanho
           glPopMatrix();
	       
	    // Movimento da mao
       	   glPushMatrix();
	       	   glTranslatef (0.0, 1.0, 0.0); // origem posicionada no punho
		        glRotatef ((GLfloat) mao, 0.0, 0.0, 1.0);
		        glTranslatef (0.0, 1.0, 0.0); // origem posicionada no centro do braco
          
	       	   // Mao
	       	   glPushMatrix();
		            glRotatef(90, 0, 0, 1); // horizontal -> vertical
		            glScalef (2.0, 0.3, 0.3); // cubo -> retângulo
		            glColor3f(0.8,0.0,0.0);
         	      glutSolidCube (1.0); // Tamanho
		        glPopMatrix();
		        
		        // Punho (mao-garra)
		        glPushMatrix();
					glTranslatef(0.0, 1.0, 0.0); // posiciona
		            glColor3f(0.5, 0.5, 0.5); // desenha
	           glutSolidSphere(0.2, 30, 30); // raio, fatias, pilhas
                glPopMatrix();
                
                // Movimento indicador
                glPushMatrix();
		       	   glTranslatef (0.0, 1, 0.0); // origem posicionada no punho
			        glRotatef ((GLfloat) indicador, 0.0, 0.0, 1.0);
			        glTranslatef (0.0, 1, 0.0); // origem posicionada no centro do braco
                
	                // Indicador
	                 glPushMatrix();
	                  glTranslatef (0.1, -0.7, 0.1);
			            glRotatef(90, 0, 0, 1); // horizontal -> vertical
			            glScalef (2.0, 0.4, 0.4); // cubo -> retângulo
			            glColor3f(0.6,0.2,0.8);
			            glutSolidCube (0.22); // Tamanho
			        glPopMatrix();
                
                glPopMatrix(); // fim movimento indicador
                
               // Movimento anelar
                glPushMatrix();
		       	   glTranslatef (0.0, 1, 0.0); // origem posicionada no punho
			        glRotatef ((GLfloat) anelar, 0.0, 0.0, 1.0);
			        glTranslatef (0.0, 1, 0.0); // origem posicionada no centro do braco
                
	                // Anelar
	                glPushMatrix();
					  glTranslatef (0.1, -0.7, -0.1); // Move para a esquerda
					  glRotatef(90, 0, 0, 1);         // Gira para a vertical
					  glScalef (2.0, 0.4, 0.4);       // Mesma escala que o indicador
					  glColor3f(0.8,0.4,0.9);         // Mesma cor
					  glutSolidCube (0.22);            // Tamanho igual
					glPopMatrix();
                
                glPopMatrix(); // fim movimento anelar
                	
                // Movimento polegar
                glPushMatrix();
		       	   glTranslatef (0.0, 1, 0.0); // origem posicionada no punho
			        glRotatef ((GLfloat) polegar, 0.0, 0.0, 1.0);
			        glTranslatef (0.0, 1, 0.0); // origem posicionada no centro do braco
                
	                // Polegar
	                glPushMatrix();
					  glTranslatef (-0.1, -0.7, 0.0); // Move para a esquerda
					  glRotatef(90, 0, 0, 1);         // Gira para a vertical
					  glScalef (2.0, 0.4, 0.4);       // Mesma escala que o indicador
					  glColor3f(0.4,0.1,0.6);         // Mesma cor
					  glutSolidCube (0.22);            // Tamanho igual
					glPopMatrix();
                
                glPopMatrix(); // fim movimento polegar
                
	         glPopMatrix(); // fim movimento mao
       glPopMatrix(); // fim movimento antebraco
    glPopMatrix(); // fim movimento braco
    
    // Troca os buffers, mostrando o que acabou de ser desenhado
    glutSwapBuffers();
}
