#include <iostream>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
#include <gl/glew.h>
#include <gl/freeglut.h>

#include <stdlib.h>
#include <time.h>
#include <math.h>

#define window_width 600
#define window_high 600
#define VAO_Max 5
#define VBO_Max 10
#define random_num 100

#define cubesize 0.1

using namespace std;

GLvoid make_vertexShaders();
GLvoid make_fragmentShader();
GLvoid InitShader();
GLvoid InitBuffer();
GLchar* filetobuf(const GLchar* file);

GLvoid drawScene();
GLvoid Reshape(int w, int h);
GLvoid KeyBoard(unsigned char Key, int x, int y);
GLvoid Timer(int value);

float RandomColor(int color);
float Gravitiy(float object_move_y, float first_y);

GLuint s_program;
GLuint VAO[VAO_Max], VBO[VBO_Max];
GLuint shaderID;
GLuint vertexShader;
GLuint fragmentShader;

int g_window_w;
int g_window_h;

GLUquadricObj* qobj;

float test1, test2, test3;

float cube[] = {
    //앞면
    -cubesize,cubesize,cubesize,
    cubesize,-cubesize,cubesize,
    -cubesize,-cubesize,cubesize,

    cubesize,-cubesize,cubesize,
    -cubesize,cubesize,cubesize,
    cubesize,cubesize,cubesize,

    //윗면
    -cubesize,cubesize,-cubesize,
    cubesize,cubesize,cubesize,
    -cubesize,cubesize,cubesize,

    cubesize,cubesize,-cubesize,
    cubesize,cubesize,cubesize,
    -cubesize,cubesize,-cubesize,

    //아랫면
    cubesize,-cubesize,cubesize,
    -cubesize,-cubesize,-cubesize,
    -cubesize,-cubesize,cubesize,

    -cubesize,-cubesize,-cubesize,
    cubesize,-cubesize,cubesize,
    cubesize,-cubesize,-cubesize,

    //뒷면
    cubesize,-cubesize,-cubesize,
    -cubesize,cubesize,-cubesize,
    -cubesize,-cubesize,-cubesize,

    -cubesize,cubesize,-cubesize,
    cubesize,-cubesize,-cubesize,
    cubesize,cubesize,-cubesize,

    //오른면
    cubesize,cubesize,cubesize,
    cubesize,-cubesize,-cubesize,
    cubesize,-cubesize,cubesize,

    cubesize,cubesize,cubesize,
    cubesize,cubesize,-cubesize,
    cubesize,-cubesize,-cubesize,

    //외면
    -cubesize,-cubesize,-cubesize,
     -cubesize,cubesize,cubesize,
    -cubesize,-cubesize,cubesize,

    -cubesize,cubesize,-cubesize,
    -cubesize,cubesize,cubesize,
    -cubesize,-cubesize,-cubesize

};
float color[] = {
    //앞면
    0.4,0.4,0.4,
    0.4,0.4,0.4,
    0.4,0.4,0.4,

    0.4,0.4,0.4,
    0.4,0.4,0.4,
    0.4,0.4,0.4,

    //윗면
    0.6,0.6,0.6,
    0.6,0.6,0.6,
    0.6,0.6,0.6,

    0.6,0.6,0.6,
    0.6,0.6,0.6,
    0.6,0.6,0.6,

    //아랫면
    0.1,0.1,0.1,
    0.1,0.1,0.1,
    0.1,0.1,0.1,

    0.1,0.1,0.1,
    0.1,0.1,0.1,
    0.1,0.1,0.1,

    //뒷면
    0.2,0.2,0.2,
    0.2,0.2,0.2,
    0.2,0.2,0.2,

    0.2,0.2,0.2,
    0.2,0.2,0.2,
    0.2,0.2,0.2,

    //오른면
    0.3,0.3,0.3,
    0.3,0.3,0.3,
    0.3,0.3,0.3,

    0.3,0.3,0.3,
    0.3,0.3,0.3,
    0.3,0.3,0.3,

    //왼면
    0.3,0.3,0.3,
    0.3,0.3,0.3,
    0.3,0.3,0.3,

    0.3,0.3,0.3,
    0.3,0.3,0.3,
    0.3,0.3,0.3

};

float ball_move_y;
float gravtime;

bool onoff_timer = true;
bool onoff_culling = false;
bool perspective = true;
bool isSolid = false;


int main(int argc, char** argv)
{

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH); //depth+
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(window_width, window_high);
    glutCreateWindow("Example 1");

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        cerr << "NOT INIT" << endl;
    }
    else
        cout << "INIT<<endl";

    srand((unsigned)time(NULL));


    glEnable(GL_DEPTH_TEST);

    InitShader();
    InitBuffer();
    glutTimerFunc(70, Timer, 1);
    glutKeyboardFunc(KeyBoard);
    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
    glutMainLoop();
}

GLvoid drawScene()
{
    GLfloat rColor = 1.0;
    GLfloat gColor = 1.0;
    GLfloat bColor = 1.0;
    glClearColor(rColor, gColor, bColor, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //깊이 체크 (컬링)
    InitBuffer();


    glUseProgram(s_program);
    int objColorLocation = glGetUniformLocation(s_program, "objectColor");
    unsigned isCheck = glGetUniformLocation(s_program, "isCheck");


    //카메라세팅
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 2.5f); //--- 카메라 위치
    glm::vec3 cameraDirection = glm::vec3(0.0, 0.0, 0.0); //--- 카메라 바라보는 방향
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); //--- 카메라 위쪽 방향
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
    unsigned int viewLocation = glGetUniformLocation(s_program, "viewTransform"); //--- 뷰잉 변환 설정
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);


    //원근법 유무(Perspective = 원근투영)
    if (perspective == false) {
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -100.0f, 100.0f); //--- 투영 공간을 [-100.0, 100.0] 공간으로 설정
        unsigned int projectionLocation = glGetUniformLocation(s_program, "projectionTransform"); //--- 투영 변환 값 설정
        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);
    }
    else {
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
        projection = glm::translate(projection, glm::vec3(0.0, 0.0, -1.0)); //--- 공간을 약간 뒤로 미뤄줌
        unsigned int projectionLocation = glGetUniformLocation(s_program, "projectionTransform"); //--- 투영 변환 값 설정
        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);
        //cout << "peron" << endl;
    }

    //은면제거
    if (onoff_culling == false) {
        glDisable(GL_CULL_FACE);
    }
    else {
        glEnable(GL_CULL_FACE);
    }


    glm::mat4 S_Matrix = glm::mat4(1.0f);
    unsigned int EllipsetransformLocation = glGetUniformLocation(s_program, "transform");

    //상자
    glBindVertexArray(VAO[0]);
    S_Matrix = glm::mat4(1.0f);
    S_Matrix = glm::translate(S_Matrix, glm::vec3(0.0, 0.0, 0.0));
    S_Matrix = glm::rotate(S_Matrix, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    S_Matrix = glm::scale(S_Matrix, glm::vec3(10.0, 10.0, 10.0));
    EllipsetransformLocation = glGetUniformLocation(s_program, "transform");
    glUniformMatrix4fv(EllipsetransformLocation, 1, GL_FALSE, glm::value_ptr(S_Matrix));
    glUniform1f(isCheck, true);
    //glUniform1f(isCheck, false);
    //glUniform4f(objColorLocation, 1.0f, 1.0f, 0.0f, 1.0);
    glDrawArrays(GL_TRIANGLES, 6, 30);


    //공
    S_Matrix = glm::mat4(1.0f);
    S_Matrix = glm::translate(S_Matrix, glm::vec3(0.0, Gravitiy(ball_move_y, 0.5), 0.0));
    S_Matrix = glm::rotate(S_Matrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    S_Matrix = glm::scale(S_Matrix, glm::vec3(1.0, 1.0, 1.0));
    EllipsetransformLocation = glGetUniformLocation(s_program, "transform");
    glUniformMatrix4fv(EllipsetransformLocation, 1, GL_FALSE, glm::value_ptr(S_Matrix));
    glUniform1f(isCheck, false);
    glUniform4f(objColorLocation, 1.0f, 1.0f, 0.0f, 1.0);
    qobj = gluNewQuadric();
    gluQuadricDrawStyle(qobj, GLU_FILL);
    gluSphere(qobj, 0.1, 24, 24);


    glutSwapBuffers();
}
GLvoid KeyBoard(unsigned char Key, int x, int y)
{
    switch (Key) {






        //test구역=====================================
    case 'u':
        test1 += 0.1f;
        cout << "test1= " << test1 << endl;
        break;
    case 'j':
        test1 -= 0.1f;
        cout << "test1= " << test1 << endl;
        break;

    case 'i':
        test2 += 1.0f;
        cout << "test2= " << test2 << endl;
        break;
    case 'k':
        test2 -= 1.0f;
        cout << "test2= " << test2 << endl;
        break;
        //==============================================
    case 'p':
        if (perspective == false)
            perspective = true;
        else
            perspective = false;
        break;
        /*case 'h':
            if (onoff_culling == false) {
                onoff_culling = true;
            }
            else
                onoff_culling = false;
            break;*/

    case 'q':
        exit(1);
        break;
    }
    drawScene();
}
GLvoid Timer(int value)
{
    if (onoff_timer == true) {

        gravtime += 0.01;

    }
    glutPostRedisplay();
    glutTimerFunc(10, Timer, 1);
}

float RandomColor(int color) {
    return (float)(rand() % random_num) / 100 - 0.5;
}

float Gravitiy(float object_move_y, float first_y) {

    float floor = -1.0;

    if (0.0 <= gravtime && gravtime < 1.0 / 2.0 * (sqrt(first_y - floor))) {
        object_move_y = -4 * gravtime * gravtime + first_y;
    }
    else if (1.0 / 2.0 * (sqrt(first_y - floor)) < gravtime && gravtime < sqrt(first_y - floor)) {
        object_move_y = -4 * (gravtime - sqrt(first_y - floor)) * (gravtime - sqrt(first_y - floor)) + first_y;
    }
    else gravtime = 0.0;

    return object_move_y;
}

GLvoid make_vertexShaders()
{
    GLchar* vertexSource;
    vertexSource = filetobuf("vertex.glsl");
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);

}
GLvoid make_fragmentShader()
{
    GLchar* fragmentSource;
    fragmentSource = filetobuf("fragment.glsl");
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);

}
GLvoid InitShader()
{
    make_vertexShaders();
    make_fragmentShader();

    s_program = glCreateProgram();

    glAttachShader(s_program, vertexShader);
    glAttachShader(s_program, fragmentShader);
    glLinkProgram(s_program);

    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
        cerr << "ERROR :  fragment Shader Fail Compile \n" << errorLog << endl;
        exit(-1);
    }

    else
        cout << "good";
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glUseProgram(s_program);
}
GLvoid InitBuffer()
{
    glGenVertexArrays(VAO_Max, VAO);
    glGenBuffers(VBO_Max, VBO);

    //Cube
    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    //->color
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]); // 색상
    glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

}
GLvoid Reshape(int w, int h)
{
    glViewport(0, 0, w, h);
}
GLchar* filetobuf(const GLchar* file)
{
    FILE* fptr;
    long length;
    GLchar* buf;

    fopen_s(&fptr, file, "rb");
    if (!fptr)
        return NULL;

    fseek(fptr, 0, SEEK_END);
    length = ftell(fptr);
    buf = (GLchar*)malloc(length + 1);
    fseek(fptr, 0, SEEK_SET);
    fread(buf, length, 1, fptr);
    fclose(fptr);
    buf[length] = 0;

    return buf;
}