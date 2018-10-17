#define _CRT_SECURE_NO_WARNINGS

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <vector>

// Incluindo bibliotecas
#define GLEW_STATIC
#include <GL\glew.h>
#include <GLFW\glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Converte um valor de cor inteiro entre 0 e 255 para um valor decimal entre 0 e 1
#define RGB_INT_DEC(X) X / 255.0f

// Agrupa os atributos de um vertice
struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Color;
	glm::vec3 Normal;
};

// Agrupa os dados que são gerados quando um arquivo obj é carregado
struct Mesh
{
	GLuint VAO;
	GLuint IBO;

	GLuint VertexCount;
};

// Prototipo da funcao chamada em eventos de teclado
void KeyCallback(GLFWwindow* Window, int Key, int Scancode, int Action, int Mode);
// Prototipo da funcao chamada em eventos de cursor
void CursorPositionCallback(GLFWwindow* Window, double Xpos, double Ypos);

// Prototipo da funcao que le dois arquivos e retorna um programa com as duas shaders compiladas
GLuint ShaderProgramFromFiles(const GLchar* VertexShaderFile, const GLchar* FragShaderFile);

// Prototipo da funcao que le um modelo 3D a partir de um arquivo e retorna uma Mesh
Mesh LoadObjToMesh(const GLchar* FileName, glm::vec3 Color);

// Prototipo da funcao que atualiza a posicao da camera
void UpdateCamera();

// Prototipo da funcao que atualiza parametros de iluminacao
void UpdateInteractiveLights();

// Variaveis globais para a camera
// Posiciona a camera a 2 unidades de distancia do quadrado
glm::vec3 CameraPosition = glm::vec3(0.0f, 0.0f, 2.0f);
glm::vec3 CameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
// Define o "vetor normal" da camera como o eixo y. Corresponde a definir o eixo de rotacao da camera.
glm::vec3 CameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

// Define a velocidade de movimento da camera
GLfloat CameraSpeed = 5.0f;

// Registra as teclas pressionadas
bool Keys[512];

// Salva o tempo gasto pelo ultimo frame
GLfloat DeltaTime = 0.0f;
GLfloat LastFrame = 0.0f;

// Salva a ultima posicao do mouse e a variacao de posicao
GLfloat LastMouseX;
GLfloat LastMouseY;

GLfloat MouseOffsetX = 0;
GLfloat MouseOffsetY = 0;

GLfloat Yaw   = -90;
GLfloat Pitch =   0;

// Identifica se o mouse foi atualizado pela primeira vez
bool FirstMouse;

// Sensibilidade do mouse
GLfloat MouseSensitivity = 0.07f;

// Light uniform values
glm::vec3 AmbientLightColor(RGB_INT_DEC(253), RGB_INT_DEC(160), RGB_INT_DEC(255));
GLfloat AmbientStrength = 0.2f;

glm::vec3 LightDirection(0.0f, -1.0f, 0.0f);
glm::vec3 LightColor(RGB_INT_DEC(253), RGB_INT_DEC(160), RGB_INT_DEC(255));
GLfloat DiffuseLigthStrength = 0.4f;

GLfloat InteractiveLightSensitivity = 0.001f;

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

	// Esconde o cursor enquanto a janela estiver em foco
	glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Define a funcao a ser chamada em eventos de teclado
	glfwSetKeyCallback(Window, KeyCallback);
	// Define a funcao a ser chamada em eventos de mouse
	glfwSetCursorPosCallback(Window, CursorPositionCallback);

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

	glEnable(GL_DEPTH_TEST);

	// Cria um programa com duas shaders via arquivo
	GLuint ShaderProgram = ShaderProgramFromFiles("shaders/vert.glsl", "shaders/frag.glsl");

	// Carrega um modelo 3D a partir de um arquivo
	Mesh LoadedMesh = LoadObjToMesh("res/pendant.obj", glm::vec3(RGB_INT_DEC(153), RGB_INT_DEC(153), RGB_INT_DEC(153)));

	// Define as variaveis para as matrizes de transformacao
	glm::mat4 Model;
	glm::mat4 View;
	glm::mat4 Projection;
	glm::mat4 Transformation;

	// Cria uma matriz de projecao
	Projection = glm::perspective(45.0f, Width / (float)Height, 0.1f, 100.0f);


	FirstMouse = true;
	// Mantem o programa executando enquanto a janela estiver aberta
	while (!glfwWindowShouldClose(Window))
	{
		// Atualiza o tempo gasto desde o ultimo frame
		GLfloat CurrentFrame = glfwGetTime();
		DeltaTime = CurrentFrame - LastFrame;
		LastFrame = CurrentFrame;

		// Processa eventos da janela
		glfwPollEvents();

		// Atualiza a camera logo que os eventos sao processados
		UpdateCamera();
		// Cria uma matriz view que corresponde ao angulo de que a camera observa a cena
		View = glm::lookAt(CameraPosition, CameraPosition + CameraFront, CameraUp);

		UpdateInteractiveLights();

		// Define uma cor e limpa a janela
		glClearColor(RGB_INT_DEC(253), RGB_INT_DEC(255), RGB_INT_DEC(248), 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// Multiplica as matrizes, obtendo a transformacao final
		Transformation = Projection * View * Model;
		
		// Busca a localizacao da matriz uniforme "transformation"
		GLuint TransformationLocation = glGetUniformLocation(ShaderProgram, "transformation");
		glUniformMatrix4fv(TransformationLocation, 1, GL_FALSE, glm::value_ptr(Transformation));

		// Busca a localizacao dos vetores uniformes responsaveis pela iluminacao
		GLuint AmbientLightColorLocation = glGetUniformLocation(ShaderProgram, "ambientLightColor");
		GLuint AmbientLightStrengthLocation = glGetUniformLocation(ShaderProgram, "ambientLightStrength");

		GLuint LightDirectionLocation = glGetUniformLocation(ShaderProgram, "lightDirection");
		GLuint LightColorLocation = glGetUniformLocation(ShaderProgram, "lightColor");

		glUniform3f(AmbientLightColorLocation, AmbientLightColor.x, AmbientLightColor.y, AmbientLightColor.z);
		glUniform1f(AmbientLightStrengthLocation, AmbientStrength);

		LightDirection = glm::normalize(LightDirection);
		LightDirection *= DiffuseLigthStrength;
		glUniform3f(LightDirectionLocation, LightDirection.x, LightDirection.y, LightDirection.z);
		glUniform3f(LightColorLocation, LightColor.x, LightColor.y, LightColor.z);

		// Ativa as shaders
		glUseProgram(ShaderProgram);

		// Ativa o vertex array e o index buffer e desenha os dois triangulos
		glBindVertexArray(LoadedMesh.VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, LoadedMesh.IBO);
		glDrawElements(GL_TRIANGLES, LoadedMesh.VertexCount, GL_UNSIGNED_INT, 0);
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

	if (Action == GLFW_PRESS)
	{
		Keys[Key] = true;
	}
	else if(Action == GLFW_RELEASE)
	{
		Keys[Key] = false;
	}	
}

void CursorPositionCallback(GLFWwindow* Window, double Xpos, double Ypos)
{
	if (FirstMouse)
	{
		LastMouseX = Xpos;
		LastMouseY = Ypos;

		FirstMouse = false;
	}
	
	// Calcula a variacao na posicao do cursor cada vez que sua posicao muda
	MouseOffsetX = Xpos - LastMouseX;
	// A coordenada y e invertida
	MouseOffsetY = LastMouseY - Ypos;
	LastMouseX = Xpos;
	LastMouseY = Ypos;

	// Atualiza a direcao da camera de acordo com o movimento do mouse
	Yaw   += (MouseOffsetX * MouseSensitivity);
	Pitch += (MouseOffsetY * MouseSensitivity);		
	
	// Impede a camera de girar ao redor do eixo y
	if (Pitch > 89.0f)
	{
		Pitch = 89.0f;
	}
	else if (Pitch < -89.0f)
	{
		Pitch = -89.0f;
	}

	glm::vec3 Front;
	Front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	Front.y = sin(glm::radians(Pitch));
	Front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	CameraFront = glm::normalize(Front);

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

void UpdateCamera()
{
	// Atualiza a posicao da camera de acordo com as teclas pressionadas
	GLfloat Speed = CameraSpeed * DeltaTime;
	if (Keys[GLFW_KEY_W])
	{
		CameraPosition += Speed * CameraFront;
	}
	if (Keys[GLFW_KEY_S])
	{
		CameraPosition -= Speed * CameraFront;
	}
	if (Keys[GLFW_KEY_A])
	{
		CameraPosition -= glm::normalize(glm::cross(CameraFront, CameraUp)) * Speed;
	}
	if (Keys[GLFW_KEY_D])
	{
		CameraPosition += glm::normalize(glm::cross(CameraFront, CameraUp)) * Speed;
	}
	if (Keys[GLFW_KEY_SPACE])
	{
		CameraPosition += CameraUp * Speed;
	}
	if (Keys[GLFW_KEY_LEFT_SHIFT])
	{
		CameraPosition -= CameraUp * Speed;
	}
}
void UpdateInteractiveLights()
{
	// Directional light X
	if (Keys[GLFW_KEY_KP_7])
	{
		LightDirection.x -= InteractiveLightSensitivity;
		if (LightDirection.x < -1.0f)
		{
			LightDirection.x = -1.0f;
		}
	}
	if (Keys[GLFW_KEY_KP_8])
	{
		LightDirection.x += InteractiveLightSensitivity;
		if (LightDirection.x > 1.0f)
		{
			LightDirection.x = 1.0f;
		}
	}

	// Directional light Y
	if (Keys[GLFW_KEY_KP_4])
	{
		LightDirection.y -= InteractiveLightSensitivity;
		if (LightDirection.y < -1.0f)
		{
			LightDirection.y = -1.0f;
		}
	}
	if (Keys[GLFW_KEY_KP_5])
	{
		LightDirection.y += InteractiveLightSensitivity;
		if (LightDirection.y > 1.0f)
		{
			LightDirection.y = 1.0f;
		}
	}

	// Directional light Z
	if (Keys[GLFW_KEY_KP_1])
	{
		LightDirection.z -= InteractiveLightSensitivity;
		if (LightDirection.z < -1.0f)
		{
			LightDirection.z = -1.0f;
		}
	}
	if (Keys[GLFW_KEY_KP_2])
	{
		LightDirection.z += InteractiveLightSensitivity;
		if (LightDirection.z > 1.0f)
		{
			LightDirection.z = 1.0f;
		}
	}
}


Mesh LoadObjToMesh(const GLchar* FileName, glm::vec3 Color)
{	
	Mesh Result;
	// Vetores de dados a serem lidos
	std::vector<GLuint> PositionIndices;
	std::vector<GLuint> NormalIndices  ;

	std::vector<glm::vec3> Positions;
	std::vector<glm::vec3> Normals  ;

	std::vector<GLuint> Indices;
	std::vector<Vertex> Vertices;

	FILE* File = fopen(FileName, "r");

	char LineType[128];
	while (fscanf(File, "%s", LineType) != EOF)
	{
		if (strcmp(LineType, "v") == 0)
		{
			glm::vec3 Position;
			fscanf(File, "%f %f %f\n", &Position.x, &Position.y, &Position.z);
			Positions.push_back(Position);
		}
		else if (strcmp(LineType, "vn") == 0)
		{
			glm::vec3 Normal;
			fscanf(File, "%f %f %f\n", &Normal.x, &Normal.y, &Normal.z);
			Normals.push_back(Normal);
		}
		else if (strcmp(LineType, "f") == 0)
		{
			GLuint VertexIndex[3], NormalIndex[3];
			int n = fscanf(File, "%d//%d %d//%d %d//%d\n", 
				&VertexIndex[0], &NormalIndex[0],
				&VertexIndex[1], &NormalIndex[1],
				&VertexIndex[2], &NormalIndex[2]
				);

			if (n != 6)
			{
				std::cout << "Esse arquivo nao e suportado." << std::endl;
				Result.VAO = 0;
				Result.IBO = 0;
				Result.VertexCount = 0;
				return Result;
			}

			PositionIndices.push_back(VertexIndex[0]);
			PositionIndices.push_back(VertexIndex[1]);
			PositionIndices.push_back(VertexIndex[2]);

			NormalIndices.push_back(NormalIndex[0]);
			NormalIndices.push_back(NormalIndex[1]);
			NormalIndices.push_back(NormalIndex[2]);
		}
	}

	for (unsigned int i = 0; i < PositionIndices.size(); i++)
	{
		Vertex CurrentVertex;
		CurrentVertex.Position = Positions[PositionIndices[i] - 1];
		CurrentVertex.Color = Color;
		CurrentVertex.Normal = Normals[NormalIndices[i] - 1];


		Indices.push_back((GLuint) i);
		Vertices.push_back(CurrentVertex);
	}
		
	GLuint VAO;
	GLuint VBO;
	GLuint IBO;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * Vertices.size(), &Vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * Indices.size(), &Indices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	
	Result.VAO = VAO;
	Result.IBO = IBO;
	Result.VertexCount = Indices.size();


	return Result;	
}