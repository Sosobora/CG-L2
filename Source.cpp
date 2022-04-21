#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>

GLuint VBO;
//GLuint gWorldLocation;
GLuint gScaleLocation; //GGGGGGGGGGGGGGGGGGGGGGGGGGGGGG

static const char* pVS = "                                                          \n\
#version 330                                                                        \n\
                                                                                    \n\
layout (location = 0) in vec3 Position;                                             \n\
                                                                                    \n\
uniform float gScale;                                                               \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
    gl_Position = vec4(gScale * Position.x, gScale * Position.y, Position.z, 1.0);  \n\
}";

static const char* pFS = "                                                    \n\
#version 330                                                                  \n\
                                                                              \n\
out vec4 FragColor;                                                           \n\
                                                                              \n\
void main()                                                                   \n\
{                                                                             \n\
    FragColor = vec4(0.8, 0.9, 0.5, 1.0);                                     \n\
}";

static void RenderSceneCB()
{
	glClear(GL_COLOR_BUFFER_BIT);

	static float Scale = 0.0f;
	Scale += 0.01f;
	glUniform1f(gScaleLocation, sinf(Scale)); //GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG

	glm::mat4 WorldRot
	{
		{cosf(Scale), -sinf(Scale), 0.0f, 0.0f},
		{sinf(Scale), cosf(Scale), 0.0f, 0.0f},
		{0.0f, 0.0f, 1.0f, 0.0f},
		{0.0f, 0.0f, 0.0f, 1.0f}
	};
	glm::mat4 WorldMov
	{
		{1.0f, 0.0f, 0.0f, sinf(Scale)},
		{0.0f, 1.0f, 0.0f, 1.0f},
		{0.0f, 0.0f, 1.0f, 0.0f},
		{0.0f, 0.0f, 0.0f, 1.0f}
	};
	glm::mat4 WorldSc
	{
		{sinf(Scale), 0.0f, 0.0f, 0.0f},
		{0.0f, cosf(Scale), 0.0f, 1.0f},
		{0.0f, 0.0f, sinf(Scale), 0.0f},
		{0.0f, 0.0f, 0.0f, 1.0f}
	};
	auto transformMat = WorldRot * WorldMov * WorldSc;

	//glUniformMatrix4fv(gWorldLocation, 1, GL_TRUE, &transformMat[0][0]);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glutSwapBuffers();
}

void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
	GLuint ShaderObj = glCreateShader(ShaderType);
	if (ShaderObj == 0) 
	{
		fprintf(stderr, "Error creating shader type %d\n", ShaderType);
		exit(0);
	}
	const GLchar* p[1];
	p[0] = pShaderText;
	GLint Lengths[1];
	Lengths[0] = strlen(pShaderText);
	glShaderSource(ShaderObj, 1, p, Lengths);
	glCompileShader(ShaderObj);
	GLint success;
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		GLchar InfoLog[1024];
		glGetShaderInfoLog(ShaderObj, sizeof(InfoLog), NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
	}
	glAttachShader(ShaderProgram, ShaderObj);
	glLinkProgram(ShaderProgram);
}

void CompileShaders()
{
	GLuint ShaderProgram = glCreateProgram();
	if (ShaderProgram == 0) //GGGGGGGGGGGGGGGGGGGGGGGGGGGGGG
	{
		fprintf(stderr, "Error creating shader program\n");
	}
	AddShader(ShaderProgram, pVS, GL_VERTEX_SHADER);
	AddShader(ShaderProgram, pFS, GL_FRAGMENT_SHADER);

	GLint Success = 0;
	GLchar ErrorLog[1024] = { 0 };

	glLinkProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);
	if (Success == 0)
	{
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
	}
	glValidateProgram(ShaderProgram);
	glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);
	if (!Success)
	{
		glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
	}
	glUseProgram(ShaderProgram);

	//gWorldLocation = glGetUniformLocation(ShaderProgram, "gWorld");
	//assert(gWorldLocation != 0xFFFFFFFF);
	gScaleLocation = glGetUniformLocation(ShaderProgram, "gScale");
	assert(gScaleLocation != 0xFFFFFFFF);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(1024, 768);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Tutorial");

	glutDisplayFunc(RenderSceneCB);
	glutIdleFunc(RenderSceneCB);//GGGGGGGGGGGGGGGGGGGGGGGGGGGGG

	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}

	glClearColor(0.0f, 0.3f, 0.4f, 0.0f);

	// CreateVertexBuffer
	glm::vec3 Vertices[3];
	Vertices[0] = glm::vec3(-1.0f, -1.0f, 0.0f);
	Vertices[1] = glm::vec3(1.0f, -1.0f, 0.0f);
	Vertices[2] = glm::vec3(0.0f, 1.0f, 0.0f);
	glGenBuffers(3, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

	CompileShaders();

	glutMainLoop();
}