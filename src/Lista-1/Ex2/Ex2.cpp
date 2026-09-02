/*
 * Hello Triangle - Código adaptado de:
 *   - https://learnopengl.com/#!Getting-started/Hello-Triangle
 *   - https://antongerdelan.net/opengl/glcontext2.html
 *
 * Adaptado por: Rossana Baptista Queiroz
 * Adaptado para o Exercício 2 - Lista 1 (círculo e polígonos via equação paramétrica)
 *
 * Controles:
 *   [1] Círculo
 *   [2] Octógono
 *   [3] Pentágono
 *   [4] Pac-Man
 *   [5] Fatia de pizza
 *   [ESC] Sair
 */

#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <assert.h>

using namespace std;

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// Protótipo da função de callback de teclado
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

// Protótipos das funções
int setupShader();
GLuint setupGeometry();
void loadShape(int mode);
void updateGeometry(const std::vector<GLfloat> &vertices);

// Dimensões da janela (pode ser alterado em tempo de execução)
const GLuint WIDTH = 800, HEIGHT = 600;

const float PI = 3.14159265358979323846f;

// Estado global da geometria atual (ALTERADO: antes não existia, pois só tínhamos 1 forma fixa)
GLuint VAO, VBO;
GLenum drawPrimitive = GL_TRIANGLE_FAN;
GLsizei vertexCount = 0;
int shapeMode = 1;

// Código fonte do Vertex Shader (em GLSL): ainda hardcoded
const GLchar *vertexShaderSource = R"glsl(
 #version 400
 layout (location = 0) in vec3 position;
 void main()
 {
	 gl_Position = vec4(position.x, position.y, position.z, 1.0);
 }
 )glsl";

// Código fonte do Fragment Shader (em GLSL): ainda hardcoded
const GLchar *fragmentShaderSource = R"glsl(
 #version 400
 uniform vec4 inputColor;
 out vec4 color;
 void main()
 {
	 color = inputColor;
 }
 )glsl";

// Função MAIN
int main()
{
	// Inicialização da GLFW
	glfwInit();

	// Criação da janela GLFW
	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Exercicio 2 -- Circulo e Poligonos", nullptr, nullptr);
	if (!window)
	{
		std::cerr << "Falha ao criar a janela GLFW" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Fazendo o registro da função de callback para a janela GLFW
	glfwSetKeyCallback(window, key_callback);

	// GLAD: carrega todos os ponteiros d funções da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Falha ao inicializar GLAD" << std::endl;
		return -1;
	}

	// Obtendo as informações de versão
	const GLubyte *renderer = glGetString(GL_RENDERER);
	const GLubyte *version = glGetString(GL_VERSION);
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;
	cout << "\nControles: 1-Circulo 2-Octogono 3-Pentagono 4-PacMan 5-Pizza\n" << endl;

	// Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	// Compilando e buildando o programa de shader
	GLuint shaderID = setupShader();

	// Gerando o VAO/VBO (ainda vazios; a geometria é carregada em loadShape)
	VAO = setupGeometry();
	loadShape(shapeMode); // carrega a forma inicial (círculo)

	GLint colorLoc = glGetUniformLocation(shaderID, "inputColor");

	glUseProgram(shaderID);

	double prev_s = glfwGetTime();
	double title_countdown_s = 0.1;

	// Loop da aplicação - "game loop"
	while (!glfwWindowShouldClose(window))
	{
		{
			double curr_s = glfwGetTime();
			double elapsed_s = curr_s - prev_s;
			prev_s = curr_s;

			title_countdown_s -= elapsed_s;
			if (title_countdown_s <= 0.0 && elapsed_s > 0.0)
			{
				double fps = 1.0 / elapsed_s;
				char tmp[256];
				sprintf(tmp, "Exercicio 2 -- Circulo e Poligonos\tFPS %.2lf", fps);
				glfwSetWindowTitle(window, tmp);
				title_countdown_s = 0.1;
			}
		}

		glfwPollEvents();

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glLineWidth(3);
		glPointSize(8);

		glBindVertexArray(VAO);

		glUniform4f(colorLoc, 0.0f, 0.0f, 1.0f, 1.0f); // azul

		// Chamada de desenho - drawcall (ALTERADO: primitiva e contagem de vértices variam por forma)
		glDrawArrays(drawPrimitive, 0, vertexCount);

		glfwSwapBuffers(window);
	}
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glfwTerminate();
	return 0;
}

// Função de callback de teclado (ALTERADO: agora troca a forma desenhada)
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (action == GLFW_PRESS)
	{
		if (key >= GLFW_KEY_1 && key <= GLFW_KEY_5)
		{
			shapeMode = key - GLFW_KEY_0; // converte a tecla pressionada em 1..7
			loadShape(shapeMode);
		}
	}
}

int setupShader()
{
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}

// Cria o VAO/VBO ainda SEM dados (ALTERADO: dados são enviados depois, via updateGeometry,
// porque a geometria muda em tempo de execução conforme a forma escolhida)
GLuint setupGeometry()
{
	GLuint vbo, vao;
	glGenBuffers(1, &vbo);
	glGenVertexArrays(1, &vao);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	VBO = vbo;
	return vao;
}

// Envia um novo conjunto de vértices para o VBO já existente
void updateGeometry(const std::vector<GLfloat> &vertices)
{
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_DYNAMIC_DRAW);
	vertexCount = (GLsizei)(vertices.size() / 3);
}

// Gera um "leque" (TRIANGLE_FAN) usando a equação paramétrica do círculo:
//   x = cx + r*cos(angulo)   y = cy + r*sin(angulo)
// O primeiro vértice é o centro; os demais percorrem o arco de startAngle até endAngle.
// sides = quantos segmentos usar no arco (poucos = polígono, muitos = círculo "liso")
std::vector<GLfloat> generateFan(int sides, float radius, float startAngle, float endAngle, float cx = 0.0f, float cy = 0.0f)
{
	std::vector<GLfloat> vertices;
	vertices.push_back(cx);
	vertices.push_back(cy);
	vertices.push_back(0.0f);

	for (int i = 0; i <= sides; i++)
	{
		float angle = startAngle + (endAngle - startAngle) * (float)i / (float)sides;
		vertices.push_back(cx + radius * cos(angle));
		vertices.push_back(cy + radius * sin(angle));
		vertices.push_back(0.0f);
	}
	return vertices;
}

// Decide qual forma gerar de acordo com o modo escolhido pelo usuário
void loadShape(int mode)
{
	std::vector<GLfloat> vertices;
	drawPrimitive = GL_TRIANGLE_FAN;

	switch (mode)
	{
	case 1: // Círculo
		vertices = generateFan(100, 0.6f, 0.0f, 2.0f * PI);
		break;
	case 2: // Octógono (8 lados)
		vertices = generateFan(8, 0.6f, 0.0f, 2.0f * PI);
		break;
	case 3: // Pentágono (5 lados, começando apontado pra cima)
		vertices = generateFan(5, 0.6f, PI / 2.0f, PI / 2.0f + 2.0f * PI);
		break;
	case 4: // Pac-Man: círculo quase completo, com uma "boca" (arco incompleto)
		vertices = generateFan(60, 0.6f, 0.35f, 2.0f * PI - 0.35f);
		break;
	case 5: // Fatia de pizza: arco bem pequeno
		vertices = generateFan(20, 0.6f, 0.0f, PI / 4.0f);
		break;
	}

	updateGeometry(vertices);
}