#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h"
#include <GL/gl.h>
#include <vector>
#include <iostream>

#define _USE_MATH_DEFINES
#include <math.h>

/* Minimal time wanted between two images */
static const float GL_VIEW_SIZE = 6;
static float aspectRatio;
static const double FRAMERATE_IN_SECONDS = 1. / 30.;
static int window_width = 800;
static int window_height = 800;
static int form = 0;
static float x_square_center{};
static float y_square_center{};

enum Primitives
{
    Triangle,
    Polygone,
    Point,
    Line,
    Quad
};

enum Formes
{
    Origin,
    Circle,
    Square
};

enum Operations
{
    Translation,
    Rotation,
    Scale
};

static Primitives primitive;

struct Vertex
{
    double posX;
    double posY;
};

std::vector<Vertex> vectex{};

/* Error handling function */
void onError(int error, const char *description)
{
    std::cout << "GLFW Error (" << error << ") : " << description << std::endl;
}

void drawPrimitive(Primitives prim)
{
    glPointSize(10);
    switch (prim)
    {
    case Primitives::Triangle:
        glBegin(GL_TRIANGLES);
        break;
    case Primitives::Quad:
        glBegin(GL_QUADS);
        break;
    case Primitives::Line:
        glBegin(GL_LINES);
        break;
    case Primitives::Point:
        glBegin(GL_POINTS);
        break;
    case Primitives::Polygone:
        glBegin(GL_POLYGON);
        break;
    }

    for (Vertex v : vectex)
    {
        glVertex2d(v.posX, v.posY);
    }
    glEnd();
}

void drawOrigin()
{

    glBegin(GL_LINES);

    glColor3f(1, 0, 0);
    glVertex2d(-GL_VIEW_SIZE / 4, 0);
    glVertex2d(GL_VIEW_SIZE / 4, 0);

    glColor3f(0, 1, 0);
    glVertex2d(0, -GL_VIEW_SIZE / 4);
    glVertex2d(0, GL_VIEW_SIZE / 4);

    glEnd();
}

void setup_matrix(Operations operation, float x, float y, float z, float alpha)
{
    switch (operation)
    {
    case Operations::Translation:
        glTranslatef(x, y, z);
        break;
    case Operations::Rotation:
        glRotatef(alpha, x, y, z);
        break;
    case Operations::Scale:
        glScalef(x, y, z);
        break;
    }
}

void drawSquare(bool full)
{
    full ? glBegin(GL_POLYGON) : glBegin(GL_LINE_LOOP);

    glVertex2d(-0.5, 0.5);
    glVertex2d(-0.5, -0.5);
    glVertex2d(0.5, -0.5);
    glVertex2d(0.5, 0.5);

    glEnd();
}

void drawCircle(float x, float y, float r, bool full)
{
    glPointSize(5);
    full ? glBegin(GL_POLYGON) : glBegin(GL_LINE_LOOP);
    glColor3f(1, 0.5, 0);
    double teta{0};
    while (teta <= 2 * M_PI)
    {
        double xPoint{x + r * cos(teta)};
        double yPoint{y + r * sin(teta)};

        glVertex2d(xPoint, yPoint);

        teta += (2 * M_PI) / 30;
    }
    glEnd();
}

void drawForms(bool full)
{
    switch (form)
    {
    case 0:
        drawCircle(1, 2, 0.5, full);
        break;
    case 1:
        glColor3f(1, 0.5, 0);
        setup_matrix(Operations::Translation, x_square_center, y_square_center, 0, 0);
        drawSquare(full);
    case 2:
        glColor3f(1, 0.5, 0);
        setup_matrix(Operations::Rotation, 0, 0, 1, 45);
        setup_matrix(Operations::Translation, 1, 0, 0, 0);
        drawSquare(full);
        glColor3f(0.64f, 0.1f, 1);
        glLoadIdentity();
        setup_matrix(Operations::Translation, 1, 0, 0, 0);
        setup_matrix(Operations::Rotation, 0, 0, 1, 45);
        setup_matrix(Operations::Translation, 1, 0, 0, 0);
        drawSquare(full);
        break;
    }
}

void onWindowResized(GLFWwindow *window, int width, int height)
{
    aspectRatio = width / (float)height;
    window_width = width;
    window_height = height;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (aspectRatio > 1)
    {
        glOrtho(
            -GL_VIEW_SIZE / 2. * aspectRatio, GL_VIEW_SIZE / 2. * aspectRatio,
            -GL_VIEW_SIZE / 2., GL_VIEW_SIZE / 2., -1.0, 1.0);
    }
    else
    {
        glOrtho(
            -GL_VIEW_SIZE / 2., GL_VIEW_SIZE / 2.,
            -GL_VIEW_SIZE / 2. / aspectRatio, GL_VIEW_SIZE / 2. / aspectRatio, -1.0, 1.0);
    }
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_Q && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    else if (key == GLFW_KEY_C && action == GLFW_PRESS)
        form = 0;

    else if (key == GLFW_KEY_O && action == GLFW_PRESS)
        form = 1;

    else if (key == GLFW_KEY_S && action == GLFW_PRESS)
        form = 2;
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        if (aspectRatio >= 1)
        {
            xpos = (GL_VIEW_SIZE / 2) * (aspectRatio * (xpos * (double(2) / window_width) - 1));
            ypos = (GL_VIEW_SIZE / 2) * (-ypos * (double(2) / window_height) + 1);
        }
        else
        {
            xpos = (GL_VIEW_SIZE / 2) * (xpos * (double(2) / window_width) - 1);
            ypos = (GL_VIEW_SIZE / 2) * (1 / aspectRatio * (-ypos * (double(2) / window_height) + 1));
        }

        Vertex v{xpos, ypos};
        vectex.push_back(v);
        x_square_center = float(v.posX);
        y_square_center = float(v.posY);
    }
}

int main()
{
    // Initialize the library
    if (!glfwInit())
    {
        return -1;
    }

    /* Callback to a function if an error is rised by GLFW */
    glfwSetErrorCallback(onError);

    // Create a windowed mode window and its OpenGL context
#ifdef __APPLE__
    // We need to explicitly ask for a 3.3 context on Mac
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
    GLFWwindow *window = glfwCreateWindow(window_width, window_height, "TD2", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Intialize glad (loads the OpenGL functions)
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        return -1;
    }

    onWindowResized(window, window_width, window_height);
    glfwSetWindowSizeCallback(window, onWindowResized);

    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Get time (in second) at loop beginning */
        double startTime = glfwGetTime();

        /* Render here */
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glClear(GL_COLOR_BUFFER_BIT);

        drawOrigin();
        drawForms(0);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();

        /* Elapsed time computation from loop begining */
        double elapsedTime = glfwGetTime() - startTime;
        /* If to few time is spend vs our wanted FPS, we wait */
        if (elapsedTime < FRAMERATE_IN_SECONDS)
        {
            glfwWaitEventsTimeout(FRAMERATE_IN_SECONDS - elapsedTime);
        }
    }

    glfwTerminate();
    return 0;
}

// QUESTION 6 : les carrés rouge et violet ne sont pas à la même position car l'ordre de transformation importe (c'est le repère qui bouge, pas la forme directement).