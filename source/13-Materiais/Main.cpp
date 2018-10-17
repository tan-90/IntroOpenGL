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

// Agrupa os dados referentes a um material
struct Material
{
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
};

struct Light
{
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};

// Vetor de materiais disponiveis
enum MaterialNames
{
	EMERALD = 0,
	JADE,
	OBSIDIAN,
	PEARL,
	RUBY,
	TURQUOISE,
	BRASS,
	BRONZE,
	CHROME,
	COPPER,
	GOLD,
	SILVER,
	BLACK_PLASTIC,
	CYAN_PLASTIC,
	GREEN_PLASTIC,
	RED_PLASTIC,
	WHITE_PLASTIC,
	YELLOW_PLASTIC,
	BLACK_RUBBER,
	CYAN_RUBBER,
	GREEN_RUBBER,
	RED_RUBBER,
	WHITE_RUBBER,
	YELLOW_RUBBER,
	MATERIAL_COUNT
};

Material Materials[MATERIAL_COUNT];

// Prototipo da funcao chamada em eventos de teclado
void KeyCallback(GLFWwindow* Window, int Key, int Scancode, int Action, int Mode);
// Prototipo da funcao chamada em eventos de cursor
void CursorPositionCallback(GLFWwindow* Window, double Xpos, double Ypos);

// Prototipo da funcao que le dois arquivos e retorna um programa com as duas shaders compiladas
GLuint ShaderProgramFromFiles(const GLchar* VertexShaderFile, const GLchar* FragShaderFile);

// Prototipo da funcao que le um modelo 3D a partir de um arquivo e retorna uma Mesh
Mesh LoadObjToMesh(const GLchar* FileName, glm::vec3 Color);

// Prototipo da funcao que carrega um material para a shader
void LoadMaterial(Material Mat, GLuint ShaderProgram);

// Prototipo da funcao que carrega uma luz para a shader
void LoadLight(Light Light, glm::vec3 Position, GLuint Program);

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
glm::vec3 LightPosition(5.0f);

GLfloat InteractiveLightSensitivity = 0.005f;

// The current material for the object
int CurrentMaterial;

int main()
{
	// Cria vetor de materiais
	Materials[EMERALD] = { { 0.0215f, 0.1745f, 0.0215f },{ 0.07568f, 0.61424f, 0.07568f },{ 0.633f, 0.727811f, 0.633f }, 0.6f };
	Materials[JADE] = { { 0.135f, 0.2225f, 0.1575f },{ 0.54f, 0.89f, 0.63f },{ 0.316228f, 0.316228f, 0.316228f }, 0.1f };
	Materials[OBSIDIAN] = { { 0.05375f, 0.05f, 0.06625f },{ 0.18275f, 0.17f, 0.22525f },{ 0.332741f, 0.328634f, 0.346435f }, 0.3f };
	Materials[PEARL] = { { 0.25f, 0.20725f, 0.20725f },{ 1.0f, 0.829f, 0.829f },{ 0.296648f, 0.296648f, 0.296648f }, 0.088f };
	Materials[RUBY] = { { 0.1745f, 0.01175f, 0.01175f },{ 0.61424f, 0.04136f, 0.04136f },{ 0.727811f, 0.626959f, 0.626959f }, 0.6f };
	Materials[TURQUOISE] = { { 0.1f, 0.18725f, 0.1745f },{ 0.396f, 0.74151f, 0.69102f },{ 0.297254f, 0.30829f, 0.306678f }, 0.1f };
	Materials[BRASS] = { { 0.329412f, 0.223529f, 0.027451f },{ 0.780392f, 0.568627f, 0.113725f },{ 0.992157f, 0.941176f, 0.807843f }, 0.21794872f };
	Materials[BRONZE] = { { 0.2125f, 0.1275f, 0.054f },{ 0.714f, 0.4284f, 0.18144f },{ 0.393548f, 0.271906f, 0.166721f }, 0.2f };
	Materials[CHROME] = { { 0.25f, 0.25f, 0.25f },{ 0.4f, 0.4f, 0.4f },{ 0.774597f, 0.774597f, 0.774597f }, 0.6f };
	Materials[COPPER] = { { 0.19125f, 0.0735f, 0.0225f },{ 0.7038f, 0.27048f, 0.0828f },{ 0.256777f, 0.137622f, 0.086014f }, 0.1f };
	Materials[GOLD] = { { 0.24725f, 0.1995f, 0.0745f },{ 0.75164f, 0.60648f, 0.22648f },{ 0.628281f, 0.555802f, 0.366065f }, 0.4f };
	Materials[SILVER] = { { 0.19225f, 0.19225f, 0.19225f },{ 0.50754f, 0.50754f, 0.50754f },{ 0.508273f, 0.508273f, 0.508273f }, 0.4f };
	Materials[BLACK_PLASTIC] = { { 0.0f, 0.0f, 0.0f },{ 0.01f, 0.01f, 0.01f },{ 0.50f, 0.50f, 0.50f }, 0.25f };
	Materials[CYAN_PLASTIC] = { { 0.0f, 0.1f, 0.06f },{ 0.0f, 0.50980392f, 0.50980392f },{ 0.50196078f, 0.50196078f, 0.50196078f }, 0.25f };
	Materials[GREEN_PLASTIC] = { { 0.0f, 0.0f, 0.0f },{ 0.1f, 0.35f, 0.1f },{ 0.45f, 0.55f, 0.45f }, 0.25f };
	Materials[RED_PLASTIC] = { { 0.0f, 0.0f, 0.0f },{ 0.5f, 0.0f, 0.0f },{ 0.7f, 0.6f, 0.6f }, 0.25f };
	Materials[WHITE_PLASTIC] = { { 0.0f, 0.0f, 0.0f },{ 0.55f, 0.55f, 0.55f },{ 0.70f, 0.70f, 0.70f }, 0.25f };
	Materials[YELLOW_PLASTIC] = { { 0.0f, 0.0f, 0.0f },{ 0.5f, 0.5f, 0.0f },{ 0.60f, 0.60f, 0.50f }, 0.25f };
	Materials[BLACK_RUBBER] = { { 0.02f, 0.02f, 0.02f },{ 0.01f, 0.01f, 0.01f },{ 0.4f, 0.4f, 0.4f }, 0.078125f };
	Materials[CYAN_RUBBER] = { { 0.0f, 0.05f, 0.05f },{ 0.4f, 0.5f, 0.5f },{ 0.04f, 0.7f, 0.7f }, 0.078125f };
	Materials[GREEN_RUBBER] = { { 0.0f, 0.05f, 0.0f },{ 0.4f, 0.5f, 0.4f },{ 0.04f, 0.7f, 0.04f }, 0.078125f };
	Materials[RED_RUBBER] = { { 0.05f, 0.0f, 0.0f },{ 0.5f, 0.4f, 0.4f },{ 0.7f, 0.04f, 0.04f }, 0.078125f };
	Materials[WHITE_RUBBER] = { { 0.05f, 0.05f, 0.05f },{ 0.5f, 0.5f, 0.5f },{ 0.7f, 0.7f	, 0.7f }, 0.078125f };
	Materials[YELLOW_RUBBER] = { { 0.05f, 0.05f, 0.0f }, { 0.5f, 0.5f, 0.4f }, { 0.7f, 0.7f	, 0.04f }, 0.078125f };

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
	Mesh LoadedMesh = LoadObjToMesh("res/cube.obj", glm::vec3(RGB_INT_DEC(255), RGB_INT_DEC(255), RGB_INT_DEC(255)));

	Mesh LampMesh = LoadObjToMesh("res/sphere.obj", glm::vec3(1.0f));

	// Define as variaveis para as matrizes de transformacao
	glm::mat4 Model;
	glm::mat4 View;
	glm::mat4 Projection;
	glm::mat4 Transformation;

	// Cria uma matriz de projecao
	Projection = glm::perspective(45.0f, Width / (float)Height, 0.1f, 100.0f);


	FirstMouse = true;

	CurrentMaterial = CHROME;

	Light SceneLight;
	SceneLight.ambient.x = 1.0f;
	SceneLight.ambient.y = 1.0f;
	SceneLight.ambient.z = 1.0f;

	SceneLight.diffuse.x = 1.0f;
	SceneLight.diffuse.y = 1.0f;
	SceneLight.diffuse.z = 1.0f;

	SceneLight.specular.x = 1.0f;
	SceneLight.specular.y = 1.0f;
	SceneLight.specular.z = 1.0f;

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
		glClearColor(RGB_INT_DEC(153), RGB_INT_DEC(153), RGB_INT_DEC(153), 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// Ativa as shaders
		glUseProgram(ShaderProgram);

		// Multiplica as matrizes, obtendo a transformacao final
		Transformation = Projection * View * Model;
		
		// Busca a localizacao das matrizes de transformacao
		GLuint ModelLocation = glGetUniformLocation(ShaderProgram, "model");
		glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, glm::value_ptr(Model));

		GLuint TransformationLocation = glGetUniformLocation(ShaderProgram, "transformation");
		glUniformMatrix4fv(TransformationLocation, 1, GL_FALSE, glm::value_ptr(Transformation));

		// Busca a localizacao dos vetores uniformes responsaveis pela iluminacao
		GLuint CameraPositionLocation = glGetUniformLocation(ShaderProgram, "cameraPosition");


		glUniform3f(CameraPositionLocation, CameraPosition.x, CameraPosition.y, CameraPosition.z);


		// Carrega um material
		LoadMaterial(Materials[CurrentMaterial], ShaderProgram);

		// Carrega uma luz
		LoadLight(SceneLight, LightPosition, ShaderProgram);

		// Ativa o vertex array e o index buffer e desenha os dois triangulos
		glBindVertexArray(LoadedMesh.VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, LoadedMesh.IBO);
		glDrawElements(GL_TRIANGLES, LoadedMesh.VertexCount, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		glm::mat4 LampModel;
		LampModel = glm::translate(LampModel, LightPosition);
		LampModel = glm::scale(LampModel, glm::vec3(0.05f));
		
		Transformation = Projection * View * LampModel;

		glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, glm::value_ptr(LampModel));
		glUniformMatrix4fv(TransformationLocation, 1, GL_FALSE, glm::value_ptr(Transformation));


		glUseProgram(ShaderProgram);
		// Ativa o vertex array e o index buffer e desenha os dois triangulos
		glBindVertexArray(LampMesh.VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, LampMesh.IBO);
		glDrawElements(GL_TRIANGLES, LampMesh.VertexCount, GL_UNSIGNED_INT, 0);
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

	if (Key == GLFW_KEY_UP && Action == GLFW_PRESS)
	{
		CurrentMaterial++;
		CurrentMaterial %= MATERIAL_COUNT;
	}

	if (Key == GLFW_KEY_DOWN && Action == GLFW_PRESS)
	{
		CurrentMaterial--;
		if (CurrentMaterial < 0)
		{
			CurrentMaterial = 0;
		}
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
		LightPosition.x -= InteractiveLightSensitivity;
	}
	if (Keys[GLFW_KEY_KP_8])
	{
		LightPosition.x += InteractiveLightSensitivity;
	}

	// Directional light Y
	if (Keys[GLFW_KEY_KP_4])
	{
		LightPosition.y -= InteractiveLightSensitivity;
	}
	if (Keys[GLFW_KEY_KP_5])
	{
		LightPosition.y += InteractiveLightSensitivity;
	}

	// Directional light Z
	if (Keys[GLFW_KEY_KP_1])
	{
		LightPosition.z -= InteractiveLightSensitivity;
	}
	if (Keys[GLFW_KEY_KP_2])
	{
		LightPosition.z += InteractiveLightSensitivity;
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

void LoadMaterial(Material Mat, GLuint ShaderProgram)
{
	// Carrega parametros do material
	GLuint MaterialAmbientLocation = glGetUniformLocation(ShaderProgram, "material.ambient");
	GLuint MaterialDiffuseLocation = glGetUniformLocation(ShaderProgram, "material.diffuse");
	GLuint MaterialSpecularLocation = glGetUniformLocation(ShaderProgram, "material.specular");
	GLuint MaterialShininessLocation = glGetUniformLocation(ShaderProgram, "material.shininess");

	glUniform3f(MaterialAmbientLocation, Mat.ambient.x, Mat.ambient.y, Mat.ambient.z);
	glUniform3f(MaterialDiffuseLocation, Mat.diffuse.x, Mat.diffuse.y, Mat.diffuse.z);
	glUniform3f(MaterialSpecularLocation, Mat.specular.x, Mat.specular.y, Mat.specular.z);
	glUniform1f(MaterialShininessLocation, Mat.shininess * 128);
}

void LoadLight(Light Light, glm::vec3 LightPosition, GLuint ShaderProgram)
{
	// Carrega parametros da luz
	GLuint LightAmbientLocation = glGetUniformLocation(ShaderProgram, "light.ambient");
	GLuint LightDiffuseLocation = glGetUniformLocation(ShaderProgram, "light.diffuse");
	GLuint LightSpecularLocation = glGetUniformLocation(ShaderProgram, "light.specular");
	GLuint LightPositionLocation = glGetUniformLocation(ShaderProgram, "light.position");

	glUniform3f(LightAmbientLocation, Light.ambient.x, Light.ambient.y, Light.ambient.z);
	glUniform3f(LightDiffuseLocation, Light.diffuse.x, Light.diffuse.y, Light.diffuse.z);
	glUniform3f(LightSpecularLocation, Light.specular.x, Light.specular.y, Light.specular.z);
	glUniform3f(LightPositionLocation, LightPosition.x, LightPosition.y, LightPosition.z);
}