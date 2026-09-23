

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <assert.h>

using namespace std;

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM (ALTERADO: usado para a projeção ortográfica em coordenadas de tela)
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods); // ALTERADO: novo callback
int setupShader();
GLuint setupGeometry();
void addVertexAtCursor(GLFWwindow *window);
void uploadTriangles();

const GLuint WIDTH = 800, HEIGHT = 600;

// Estado global dos triângulos criados por clique (ALTERADO: não existia antes)
GLuint VAO, VBO;
std::vector<GLfloat> pendingClicks;  // vértices (x,y) ainda incompletos (menos de 3)
std::vector<GLfloat> triangleData;   // dados já finalizados: [x,y,z,r,g,b] por vértice
GLsizei vertexCount = 0;

// Vertex Shader (ALTERADO: recebe posição + cor por vértice, e a matriz de projeção)
const GLchar *vertexShaderSource = R"glsl(
 #version 400
 layout (location = 0) in vec3 position;
 layout (location = 1) in vec3 color;
 uniform mat4 projection;
 out vec3 vColor;
 void main()
 {
	 gl_Position = projection * vec4(position.x, position.y, position.z, 1.0);
	 vColor = color;
 }
 )glsl";

// Fragment Shader (ALTERADO: usa a cor vinda do vertex shader, uma por triângulo)
const GLchar *fragmentShaderSource = R"glsl(
 #version 400
 in vec3 vColor;
 out vec4 color;
 void main()
 {
	 color = vec4(vColor, 1.0);
 }
 )glsl";

int main()
{
	glfwInit();
	srand((unsigned int)time(NULL)); // ALTERADO: semente para as cores aleatórias

	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Exercicio 6 -- Triangulos por Clique", nullptr, nullptr);
	if (!window)
	{
		std::cerr << "Falha ao criar a janela GLFW" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback); // ALTERADO: registra o clique do mouse

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Falha ao inicializar GLAD" << std::endl;
		return -1;
	}

	const GLubyte *renderer = glGetString(GL_RENDERER);
	const GLubyte *version = glGetString(GL_VERSION);
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;
	cout << "\nClique com o botao esquerdo para criar vertices. A cada 3 cliques, um triangulo colorido aparece.\n" << endl;

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	GLuint shaderID = setupShader();
	VAO = setupGeometry();

	GLint projLoc = glGetUniformLocation(shaderID, "projection");

	glUseProgram(shaderID);

	// ALTERADO: janela do mundo com o mesmo tamanho da tela, para que a posição do
	// clique (em pixels) sirva diretamente como coordenada do mundo (dica do enunciado)
	glm::mat4 projection = glm::ortho(0.0f, (float)WIDTH, (float)HEIGHT, 0.0f, -1.0f, 1.0f);
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

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
				sprintf(tmp, "Exercicio 6 -- Triangulos por Clique\tFPS %.2lf", fps);
				glfwSetWindowTitle(window, tmp);
				title_countdown_s = 0.1;
			}
		}

		glfwPollEvents();

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, vertexCount); // desenha todos os triângulos já completos

		glfwSwapBuffers(window);
	}
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glfwTerminate();
	return 0;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

// ALTERADO: novo callback - trata o clique do mouse e cria vértices/triângulos
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		addVertexAtCursor(window);
	}
}

// Pega a posição atual do cursor e adiciona como um novo vértice pendente;
// ao completar 3, gera um triângulo com cor aleatória e envia pro VBO
void addVertexAtCursor(GLFWwindow *window)
{
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	// A janela do mundo tem o mesmo tamanho da tela, então (xpos, ypos) já é
	// diretamente a coordenada do mundo -- não precisa converter nada
	pendingClicks.push_back((float)xpos);
	pendingClicks.push_back((float)ypos);

	if (pendingClicks.size() == 6) // 3 vértices (x,y) completos
	{
		// Cor aleatória para este triângulo
		float r = (float)(rand() % 256) / 255.0f;
		float g = (float)(rand() % 256) / 255.0f;
		float b = (float)(rand() % 256) / 255.0f;

		for (int i = 0; i < 3; i++)
		{
			float x = pendingClicks[i * 2];
			float y = pendingClicks[i * 2 + 1];

			triangleData.push_back(x);
			triangleData.push_back(y);
			triangleData.push_back(0.0f);
			triangleData.push_back(r);
			triangleData.push_back(g);
			triangleData.push_back(b);
		}

		pendingClicks.clear();
		uploadTriangles();
	}
}

// Reenvia todos os triângulos já criados para o VBO
void uploadTriangles()
{
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, triangleData.size() * sizeof(GLfloat), triangleData.data(), GL_DYNAMIC_DRAW);
	vertexCount = (GLsizei)(triangleData.size() / 6);
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

// ALTERADO: VAO/VBO começam vazios (sem geometria fixa); os dados chegam via
// uploadTriangles() conforme o usuário clica. Atributo 0 = posição, atributo 1 = cor
GLuint setupGeometry()
{
	GLuint vbo, vao;
	glGenBuffers(1, &vbo);
	glGenVertexArrays(1, &vao);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	VBO = vbo;
	return vao;
}