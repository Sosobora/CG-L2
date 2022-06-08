#include <iostream>
#include <GL\glew.h>
#include <GL\freeglut.h>
#include <glm/glm.hpp>
#include <cmath>

const double PI = acos(-1.0); // need to convert from degree to radian

GLuint VBO;
GLuint IBO;
GLuint gWorldLocation;

struct { // for perspective projection
    float FOV;
    float Width;
    float Height;
    float zNear;
    float zFar;
} m_persProj;

struct { // for determining the location of the camera
    glm::vec3 Pos;
    glm::vec3 Target;
    glm::vec3 Up;
} m_camera;

class Pipeline
{
public:
    Pipeline()
    {
        m_scale = glm::vec3(1.0f, 1.0f, 1.0f);
        m_worldPos = glm::vec3(0.0f, 0.0f, 0.0f);
        m_rotateInfo = glm::vec3(0.0f, 0.0f, 0.0f);
    }

    void Scale(float ScaleX, float ScaleY, float ScaleZ) // setting the scale parameters
    {
        m_scale.x = ScaleX;
        m_scale.y = ScaleY;
        m_scale.z = ScaleZ;
    }

    void WorldPos(float x, float y, float z) // setting moving parameters
    {
        m_worldPos.x = x;
        m_worldPos.y = y;
        m_worldPos.z = z;
    }

    void Rotate(float RotateX, float RotateY, float RotateZ) // setting rotation parameters
    {
        m_rotateInfo.x = RotateX;
        m_rotateInfo.y = RotateY;
        m_rotateInfo.z = RotateZ;
    }
    void InitScaleTransform(glm::mat4& ScaleTrans) const 
    { // setting scale matrix
        glm::mat4  World
        {
            { abs(sinf(m_scale.x)), 0.0f, 0.0f, 0.0f },
            { 0.0f, abs(sinf(m_scale.y)), 0.0f, 0.0f },
            { 0.0f, 0.0f, 1.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 1.0f }
        };
        ScaleTrans = World;
    }
    void InitRotateTransform(glm::mat4& RotateTrans) const 
    { // initializing rotation matrices

        const float x = m_rotateInfo.x * PI / 180.0f;
        const float y = m_rotateInfo.y * PI / 180.0f;
        const float z = m_rotateInfo.z * PI / 180.0f;

        glm::mat4 RotateX
        {
            { 1.0f, 0.0f, 0.0f, 0.0f },
            { 0.0f, cosf(x), -sinf(x), 0.0f },
            { 0.0f, sinf(x), cosf(x), 0.0f },
            { 0.0f, 0.0f, 0.0f, 1.0f }
        };
        glm::mat4 RotateY
        {
            { cosf(y), 0.0f, -sinf(y), 0.0f },
            { 0.0f, 1.0f, 0.0f, 0.0f },
            { sinf(y), 0.0f, cosf(y), 0.0f },
            { 0.0f, 0.0f, 0.0f, 1.0f }
        };
        glm::mat4 RotateZ
        {
            { cosf(z), -sinf(z), 0.0f, 0.0f },
            { sinf(z), cosf(z), 0.0f, 0.0f },
            { 0.0f, 0.0f, 1.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f, 1.0f }
        };

        RotateTrans = RotateX * RotateY * RotateZ;
    }

    void InitTranslationTransform(glm::mat4& TranslationTrans) const 
    { // setting moving parameters
        glm::mat4 Move
        {
            { 1.0f, 0.0f, 0.0f, sinf(m_worldPos.x) },
            { 0.0f, 1.0f, 0.0f, sinf(m_worldPos.x) },
            { 0.0f, 0.0f, 1.0f, sinf(m_worldPos.x) },
            { 0.0f, 0.0f, 0.0f, 1.0f }
        };
        TranslationTrans = Move;
    }

    glm::mat4 InitTranslationTransform(float x, float y, float z) const 
    { // required for Camera Transformation
        glm::mat4  Move
        {
            { 1.0f, 0.0f, 0.0f, x },
            { 0.0f, 1.0f, 0.0f, y },
            { 0.0f, 0.0f, 1.0f, z },
            { 0.0f, 0.0f, 0.0f, 1.0f }
        };
        return Move;
    }

    void InitPerspectiveProj(glm::mat4& m) const // matrix for perspective projection
    {
        const float ar = m_persProj.Width / m_persProj.Height;
        const float zNear = m_persProj.zNear;
        const float zFar = m_persProj.zFar;
        const float zRange = zNear - zFar;
        const float tanHalfFOV = tanf((m_persProj.FOV / 2.0) * PI / 180.0f);

        m = 
        {
            { 1.0f / (tanHalfFOV * ar), 0.0f, 0.0f, 0.0f },
            { 0.0f, 1.0f / tanHalfFOV, 0.0f, 0.0f },
            { 0.0f, 0.0f, (-zNear - zFar) / zRange, 2.0f * zFar * zNear / zRange },
            { 0.0f, 0.0f, 1.0f, 0.0f }
        };
    }
    void SetPerspectiveProj(float FOV, float width, float height, float zFar, float zNear) 
    {
        m_persProj.FOV = FOV;
        m_persProj.Width = width;
        m_persProj.Height = height;
        m_persProj.zFar = zFar;
        m_persProj.zNear = zNear;
    }
    void Normalize(glm::vec3& Vec) 
    {
        const float Length = sqrtf(Vec.x * Vec.x + Vec.y * Vec.y + Vec.z * Vec.z);
        Vec.x /= Length;
        Vec.y /= Length;
        Vec.z /= Length;
    }
    glm::vec3 Cross(glm::vec3& v1, glm::vec3& v2) const // finding a vector perpendicular to these
    {
        const float _x = v1.y * v2.z - v1.z * v2.y;
        const float _y = v1.z * v2.x - v1.x * v2.z;
        const float _z = v1.x * v2.y - v1.y * v2.x;

        return glm::vec3(_x, _y, _z);
    }
    glm::mat4 InitCameraTransform(glm::vec3& Target, const glm::vec3& Up) // setting camera transformation parameters
    {
        glm::vec3 N = Target;
        Normalize(N);
        glm::vec3 U = Up;
        Normalize(U);
        U = Cross(U, Target);
        glm::vec3 V = Cross(N, U);

        glm::mat4 m
        {
            { U.x, U.y, U.z, 0.0f },
            { V.x, V.y, V.z, 0.0f },
            { N.x, N.y, N.z, 0.0f },
            { 0.0f, 0.0f, 0.0f, 1.0f }
        };
        return m;
    }
    void SetCamera(glm::vec3 CameraPos, glm::vec3 CameraTarget, glm::vec3 CameraUp) {
        m_camera.Pos = CameraPos;
        m_camera.Target = CameraTarget;
        m_camera.Up = CameraUp;
    }
    const glm::mat4* GetTrans();
private:
    glm::vec3 m_scale;
    glm::vec3 m_worldPos;
    glm::vec3 m_rotateInfo;
    glm::mat4  m_transformation;
};

const glm::mat4* Pipeline::GetTrans() // unite all transformations
{
    glm::mat4 ScaleTrans, RotateTrans, TranslationTrans, CameraTranslationTrans, CameraRotateTrans, PersProjTrans;

    InitScaleTransform(ScaleTrans);
    InitRotateTransform(RotateTrans);
    InitTranslationTransform(TranslationTrans);
    CameraTranslationTrans = InitTranslationTransform(-m_camera.Pos.x, -m_camera.Pos.y, -m_camera.Pos.z);
    CameraRotateTrans = InitCameraTransform(m_camera.Target, m_camera.Up);

    InitPerspectiveProj(PersProjTrans);

    m_transformation = //PersProjTrans * 
        CameraRotateTrans *
        CameraTranslationTrans *
        TranslationTrans *
        RotateTrans * ScaleTrans;

    return &m_transformation;
}

static const char* pVS = "#version 330\n\
layout (location = 0) in vec3 Position;\n\
uniform mat4 gWorld;\n\
out vec4 Color;\n\
void main()\n\
{gl_Position = gWorld * vec4(Position, 2.0);\n\
Color = vec4(clamp(Position, 0.3, 0.8), 1.0); }";

static const char* pFS = "#version 330\n\
out vec4 FragColor;\n\
in vec4 Color;\n\
void main()\n\
{//FragColor = vec4(0.2, 0.0, 0.8, 0.3);\n\
FragColor = Color;\n\
}";

void RenderSceneCB()
{
    glClear(GL_COLOR_BUFFER_BIT);

    static float Scale = 0.0f;
    Scale += 0.0008f;

    Pipeline p;
    p.Scale(sinf(Scale * 0.1f), sinf(Scale * 0.1f), sinf(Scale * 0.1f));
    p.WorldPos(sinf(Scale), 0.0f, 0.0f);
    p.Rotate(sinf(Scale) * 90.0f, sinf(Scale) * 90.0f, sinf(Scale) * 90.0f);

    p.SetPerspectiveProj(30.0f, GLUT_WINDOW_WIDTH, GLUT_WINDOW_HEIGHT, 1.0f, 100.0f);

    glm::vec3 CameraPos(1.0f, 0.0f, 1.0f); // Here?
    glm::vec3 CameraTarget(0.5f, 0.0f, 1.0f);
    glm::vec3 CameraUp(0.0f, 1.0f, 0.0f);

    p.SetCamera(CameraPos, CameraTarget, CameraUp);

    glUniformMatrix4fv(gWorldLocation, 1, GL_TRUE, (const GLfloat*)p.GetTrans());

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

    glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

    glDisableVertexAttribArray(0);

    glutSwapBuffers();
}

static void CreateVertexBuffer()
{
    glm::vec3 Vertices[4];
    Vertices[0] = glm::vec3(-1.0f, -1.0f, 0.0f);
    Vertices[1] = glm::vec3(0.0f, 0.0f, 2.0f);//(1.0f, -1.0f, 0.0f);
    Vertices[2] = glm::vec3(1.0f, -1.0f, 0.0f);//(0.0f, 0.0f, 1.0f);
    Vertices[3] = glm::vec3(0.0f, 1.0f, 0.0f);//(0.0f, 1.0f, 0.0f);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
}
static void CreateIndexBuffer()
{
    unsigned int Indices[] = { 0, 3, 1,
                          1, 3, 2,
                          2, 3, 0,
                          0, 2, 1 };

    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);
}

static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
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
        glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
        fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
        exit(1);
    }
    glAttachShader(ShaderProgram, ShaderObj);
}

static void CompileShaders()
{
    GLuint ShaderProgram = glCreateProgram();

    if (ShaderProgram == 0) 
    {
        fprintf(stderr, "Error creating shader program\n");
        exit(1);
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
        exit(1);
    }

    glValidateProgram(ShaderProgram);
    glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);
    if (!Success) 
    {
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
        exit(1);
    }

    glUseProgram(ShaderProgram);

    gWorldLocation = glGetUniformLocation(ShaderProgram, "gWorld");
    assert(gWorldLocation != 0xFFFFFFFF);
}

int main(int argc, char** argv)
{
    // initializing the window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

    // setting the size, position and name of window
    glutInitWindowSize(700, 700);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Tutorial");

    // InitializeGlutCallbacks
    glutDisplayFunc(RenderSceneCB);
    glutIdleFunc(RenderSceneCB);

    // clearing the window with black color
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    GLenum res = glewInit();
    if (res != GLEW_OK)
    {
        fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
        return 1;
    }

    glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
    CreateVertexBuffer();
    CreateIndexBuffer();

    CompileShaders();

    glutMainLoop();
}
