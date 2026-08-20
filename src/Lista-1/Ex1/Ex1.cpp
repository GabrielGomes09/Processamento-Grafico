/*
 * Hello Triangle - Código adaptado de:
 *   - https://learnopengl.com/#!Getting-started/Hello-Triangle
 *   - https://antongerdelan.net/opengl/glcontext2.html
 *
 * Adaptado por: Rossana Baptista Queiroz
 * Exercício 1 - Lista 1 - Processamento Gráfico
 *
 * Desenha 2 triângulos, alternando entre:
 *   [1] Preenchido
 *   [2] Contorno (wireframe)
 *   [3] Pontos
 *   [4] Os 3 juntos
 */

#include <iostream>
#include <string>
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
int setupGeometry();

// Dimensões da janela
const GLuint WIDTH = 800, HEIGHT = 600;

// Modo de desenho atual: 1 = preenchido, 2 = contorno, 3 = pontos, 4 = todos juntos
int drawMode = 1;

// Vertex Shader
const GLchar *vertexShaderSource = R"glsl(
 #version 400
 layout (location = 0) in vec3 position;
 void main()
 {
	 gl_Position = vec4(position.x, position.y, position.z, 1.0);
 }
 )glsl";

// Fragment Shader
const GLchar *fragmentShaderSource = R"glsl(
 #version 400
 uniform vec4 inputColor;
 out vec4 color;
 void main()
 {
	 color = inputColor;
 }
 )glsl";

int main()
{
	glfwInit();

	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Exercicio 1 - 2 Triangulos", nullptr, nullptr);
	if (!window)
	{
		std::cerr << "Falha ao criar a janela GLFW" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, key_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Falha ao inicializar GLAD" << std::endl;
		return -1;
	}

	const GLubyte *renderer = glGetString(GL_RENDERER);
	const GLubyte *version = glGetString(GL_VERSION);
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	GLuint shaderID = setupShader();
	GLuint VAO = setupGeometry();

	GLint colorLoc = glGetUniformLocation(shaderID, "inputColor");

	glUseProgram(shaderID);

	double prev_s = glfwGetTime();
	double title_countdown_s = 0.1;

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
				sprintf(tmp, "Exercicio 1 - 2 Triangulos\tFPS %.2lf", fps);
				glfwSetWindowTitle(window, tmp);
				title_countdown_s = 0.1;
			}
		}

		glfwPollEvents();

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // fundo branco pra facilitar visualizar contorno/pontos
		glClear(GL_COLOR_BUFFER_BIT);

		glLineWidth(3);
		glPointSize(8);

		glBindVertexArray(VAO);

		// 6 vértices = 2 triângulos (3 vértices cada)
		if (drawMode == 1) // apenas preenchido
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glUniform4f(colorLoc, 0.0f, 0.0f, 1.0f, 1.0f); // azul
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
		else if (drawMode == 2) // apenas contorno
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glUniform4f(colorLoc, 0.0f, 0.0f, 0.0f, 1.0f); // preto
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
		else if (drawMode == 3) // apenas pontos
		{
			glUniform4f(colorLoc, 1.0f, 0.0f, 0.0f, 1.0f); // vermelho
			glDrawArrays(GL_POINTS, 0, 6);
		}
		else if (drawMode == 4) // as 3 formas juntas
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glUniform4f(colorLoc, 0.6f, 0.6f, 1.0f, 1.0f); // azul claro
			glDrawArrays(GL_TRIANGLES, 0, 6);

			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glUniform4f(colorLoc, 0.0f, 0.0f, 0.0f, 1.0f); // preto
			glDrawArrays(GL_TRIANGLES, 0, 6);

			glUniform4f(colorLoc, 1.0f, 0.0f, 0.0f, 1.0f); // vermelho
			glDrawArrays(GL_POINTS, 0, 6);
		}

		glfwSwapBuffers(window);
	}
	glDeleteVertexArrays(1, &VAO);
	glfwTerminate();
	return 0;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_1) drawMode = 1; // preenchido
		if (key == GLFW_KEY_2) drawMode = 2; // contorno
		if (key == GLFW_KEY_3) drawMode = 3; // pontos
		if (key == GLFW_KEY_4) drawMode = 4; // todos juntos
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

int setupGeometry()
{
	// 2 triângulos, formando o "bowtie" mostrado no enunciado
	GLfloat vertices[] = {
		// x      y     z
		// Triângulo 1 (esquerda)
		-0.5f,  0.5f, 0.0f, // v0
		-0.5f, -0.5f, 0.0f, // v1
		 0.0f,  0.0f, 0.0f, // v2

		// Triângulo 2 (direita)
		 0.0f,  0.0f, 0.0f, // v3
		 0.5f, -0.5f, 0.0f, // v4
		 0.5f,  0.5f, 0.0f, // v5
	};

	GLuint VBO, VAO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return VAO;
}