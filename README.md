# **Farol 2D**

Este programa cria um desenho de um farol em 2D utilizando OpenGL e GLUT. O código define funções para desenhar diversas formas geométricas (trapézios, retângulos, círculos, semicírculos e triângulos), que juntas formam a estrutura de um farol.

- **Componentes principais:**
  - Desenho de formas simétricas e regulares.
  - Controle da visualização e eventos de teclado.
  - Alteração de cores usando os valores RGB.
  
- **Funções principais:**
  - `drawSymmetricTrapezoid`: Desenha trapézios simétricos para construir o corpo do farol.
  - `drawRectangle`, `drawHalfCircle`, `drawCircle`, `drawTriangle`: Desenham as partes do farol como janelas, porta, e a luz do topo.
  
- **Execução**:
  1. Configuração da janela OpenGL e seus parâmetros.
  2. Loop principal que permite a visualização do farol e interações como redimensionamento e fechamento da janela (via tecla ESC).

# **Braço Robótico**

Este código simula um braço robótico articulado em 3D, permitindo movimentar as juntas (ombro, cotovelo, mão, dedos) através de interações com o teclado.

- **Componentes principais:**
  - Estrutura do braço: ombro, cotovelo, mão e dedos (indicador, anelar, polegar).
  - Movimento dos componentes articulados.
  - Controle de visualização e interação via teclado.
  
- **Funções principais:**
  - `display`: Desenha o braço robótico e aplica transformações de rotação e movimento.
  - `keyboard`: Controla o movimento de cada parte do braço, alterando ângulos e posições.
  - `reshape`: Ajusta a visualização quando a janela é redimensionada.

- **Execução**:
  1. Configuração da janela OpenGL.
  2. Movimentação das articulações via teclado.
  3. Renderização contínua do braço com movimentos animados.

Ambos os códigos são exemplos de projetos gráficos interativos que usam OpenGL para renderizar formas geométricas e criar simulações gráficas em 2D e 3D.