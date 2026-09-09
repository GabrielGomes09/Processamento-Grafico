
#include <iostream>
#include <string>
#include <assert.h>
 
using namespace std;
 
// GLAD
#include <glad/glad.h>
 
// GLFW
#include <GLFW/glfw3.h>
 
// GLM (ALTERADO: incluído para construir a matriz de projeção ortográfica)
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
 
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
int setupShader();
int setupGeometry();
 
const GLuint WIDTH = 800, HEIGHT = 600;
 
// Vertex Shader (ALTERADO: recebe a matriz "projection" e multiplica a posição por ela)
const GLchar *vertexShaderSource = R"glsl(
 #version 400
 layout (location = 0) in vec3 position;
 uniform mat4 projection;
 void main()
 {
	 gl_Position = projection * vec4(position.x, position.y, position.z, 1.0);
 }
 )glsl";
 
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
 
	GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Exercicio 1 -- Janela Ortografica -10 a 10", nullptr, nullptr);
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
	GLint projLoc = glGetUniformLocation(shaderID, "projection");
 
	glUseProgram(shaderID);
 
	// ALTERADO: matriz de projeção ortográfica com a janela do mundo pedida no exercício
	glm::mat4 projection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, -1.0f, 1.0f);
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
				sprintf(tmp, "Exercicio 1 -- Janela Ortografica -10 a 10\tFPS %.2lf", fps);
				glfwSetWindowTitle(window, tmp);
				title_countdown_s = 0.1;
			}
		}
 
		glfwPollEvents();
 
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
 
		glBindVertexArray(VAO);
		glUniform4f(colorLoc, 1.0f, 0.0f, 1.0f, 1.0f); // magenta
 
		glDrawArrays(GL_TRIANGLES, 0, 3);
 
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
 
// ALTERADO: coordenadas agora em "unidades de mundo" dentro do intervalo -10..10,
// em vez de -1..1 (NDC), já que quem faz essa conversão agora é a matriz de projeção
int setupGeometry()
{
	GLfloat vertices[] = {
		-5.0f, -5.0f, 0.0f,
		 5.0f, -5.0f, 0.0f,
		 0.0f,  5.0f, 0.0f,
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
 