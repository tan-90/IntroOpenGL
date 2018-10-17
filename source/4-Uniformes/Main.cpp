
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

// Incluindo bibliotecas
#define GLEW_STATIC
#include <GL\glew.h>
#include <GLFW\glfw3.h>

// Prototipo da funcao chamada em eventos de teclado
void KeyCallback(GLFWwindow* Window, int Key, int Scancode, int Action, int Mode);

// Prototipo da funcao que le dois arquivos e retorna um programa com as duas shaders compiladas
GLuint ShaderProgramFromFiles(const GLchar* VertexShaderFile, const GLchar* FragShaderFile);

int main()
{
	// Inicializa GLFW e configura a versao
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// Desativa redimensionamento da janela
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Cria uma janela com (largura, altura, nome, , ) e checa o resultado
	GLFWwindow* Window = glfwCreateWindow(800, 600, "seminfo", nullptr, nullptr);
	if (Window == nullptr)
	{
		std::cout << "Falha na criacao da janela." << std::endl;
		glfwTerminate();
		return -1;
	}
	// Torna a janela criada o contexto atual do OpenGL
	glfwMakeContextCurrent(Window);

	// Define a funcao a ser chamada em eventos de teclado
	glfwSetKeyCallback(Window, KeyCallback);

	// Inicializa GLEW para a janela atual e checa o resultado
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Falha inicializando glew." << std::endl;
		return -1;
	}

	// Define a area da janela a ser usada, nesse caso, toda a area
	int Width, Height;
	glfwGetFramebufferSize(Window, &Width, &Height);
	glViewport(0, 0, Width, Height);

	// Cria um programa com duas shaders via arquivo
	GLuint ShaderProgram = ShaderProgramFromFiles("shaders/vert.glsl", "shaders/frag.glsl");

	// Define as coordenadas dos vertices de um quadrado
	GLfloat Vertices[] = 
	{
		 0.5f,  0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
	};
	// Define dois triangulos a partir dos vertices
	GLuint Indices[] = 
	{
		0, 1, 2,
		2, 3, 0
	};
	// Cria um vertex array, um vertex buffer e um index buffer
	GLuint VAO, VBO, IBO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// Define os dados do vertex buffer como os vertices do quadrado
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), (void*)Vertices, GL_STATIC_DRAW);
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	// Define os dados do index buffer como os triangulos que formam o quadrado
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), (void*)Indices, GL_STATIC_DRAW);
	// Define como os dados serao enviados para a shader
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Mantem o programa executando enquanto a janela estiver aberta
	while (!glfwWindowShouldClose(Window))
	{
		// Processa eventos da janela
		glfwPollEvents();

		// Define uma cor e limpa a janela
		glClearColor(174 / 255.0f, 255 / 255.0f, 138 / 255.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Busca um identificador para a variavel uniforme "inColor"
		GLint UniformColorLocation = glGetUniformLocation(ShaderProgram, "inColor");

		// Gera um valor RGB dependendo do tempo atual
		GLfloat Time = glfwGetTime();
		GLfloat Red = (sin(Time) + 10.0f) / 11.0f;
		GLfloat Green = (cos(Time) + 6.0f) / 7.0f;
		GLfloat Blue = (sin(Time) + 3.0f) / 4.0f;		

		// Ativa as shaders
		glUseProgram(ShaderProgram);
		// Define um valor para a variavel uniforme
		glUniform4f(UniformColorLocation, Red, Green, Blue, 1.0f);

		// Ativa o vertex array e o index buffer e desenha os dois triangulos
		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		// Troca os buffers para exibir o resultado da ultima chamada de desenho
		glfwSwapBuffers(Window);
	}

	// Termina o GLFW antes de o programa encerrar
	glfwTerminate();

	return 0;
}

void KeyCallback(GLFWwindow* Window, int Key, int Scancode, int Action, int Mode)
{
	// Verifica o pressionamento de esc e fecha a janela
	if (Key == GLFW_KEY_ESCAPE && Action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(Window, GL_TRUE);
	}
}

GLuint ShaderProgramFromFiles(const GLchar* VertexShaderFile, const GLchar* FragShaderFile)
{
	// Strings a serem lidas do arquivo
	std::string VertexSource;
	std::string FragSource;

	// Abstracao de c++ de um arquivo a ser aberto
	std::ifstream VertexFile;
	std::ifstream FragFile;

	// Stream de dados a serem lidos de um arquivo
	std::stringstream VertexStream;
	std::stringstream FragStream;

	// Abre os dois arquivos especificados
	VertexFile.open(VertexShaderFile);
	FragFile.open(FragShaderFile);

	// Le o conteudo dos arquivos para uma stream
	VertexStream << VertexFile.rdbuf();
	FragStream << FragFile.rdbuf();

	// Fecha os dois arquivos
	VertexFile.close();
	FragFile.close();

	// Converte a stream de dados em uma string
	VertexSource = VertexStream.str();
	FragSource = FragStream.str();

	//std::cout << VertexSource << std::endl;
	//std::cout << FragSource << std::endl;

	// Converte a string em um vetor de caracteres
	const GLchar* VertexShaderSource = VertexSource.c_str();
	const GLchar* FragShaderSource = FragSource.c_str();

	// Cria um programa com as duas shaders lidas via arquivo
	GLint  ShaderStatus;
	GLchar ErrorInfo[512];
	// Cria uma vertex shader, define seu codigo, compila e checa o resultado, exibindo informacao sobre os erros
	GLuint VertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(VertexShader, 1, &VertexShaderSource, NULL);
	glCompileShader(VertexShader);
	glGetShaderiv(VertexShader, GL_COMPILE_STATUS, &ShaderStatus);
	if (!ShaderStatus)
	{
		glGetShaderInfoLog(VertexShader, 512, NULL, ErrorInfo);
		std::cout << "Erro compilando vertex shader: " << std::endl;
		std::cout << ErrorInfo << std::endl;
	}
	// Cria uma fragment shader, define seu codigo, compila e checa o resultado, exibindo informacao sobre os erros
	GLuint FragShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(FragShader, 1, &FragShaderSource, NULL);
	glCompileShader(FragShader);
	glGetShaderiv(FragShader, GL_COMPILE_STATUS, &ShaderStatus);
	if (!ShaderStatus)
	{
		glGetShaderInfoLog(FragShader, 512, NULL, ErrorInfo);
		std::cout << "Erro compilando fragment shader: " << std::endl;
		std::cout << ErrorInfo << std::endl;
	}
	// Cria um programa, associa as duas shaders criadas ao programa e checa o resultado
	GLuint ShaderProgram = glCreateProgram();
	glAttachShader(ShaderProgram, VertexShader);
	glAttachShader(ShaderProgram, FragShader);
	glLinkProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &ShaderStatus);
	if (!ShaderStatus)
	{
		glGetProgramInfoLog(ShaderProgram, 512, NULL, ErrorInfo);
		std::cout << "Erro de link no programa: " << std::endl;
		std::cout << ErrorInfo << std::endl;
	}

	return ShaderProgram;
}