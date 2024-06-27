///////////////////////////////////////////////////////////////////////////////
// viewmanager.h
// ============
// manage the viewing of 3D objects within the viewport
//
//  AUTHOR: Brian Battersby - SNHU Instructor / Computer Science
//	Created for CS-330-Computational Graphics and Visualization, Nov. 1st, 2023
///////////////////////////////////////////////////////////////////////////////

#include "ViewManager.h"

// GLM Math Header inclusions
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>    

// declaration of the global variables and defines
namespace
{
    // Variables for window width and height
    const int WINDOW_WIDTH = 1000;
    const int WINDOW_HEIGHT = 800;
    const char* g_ViewName = "view";
    const char* g_ProjectionName = "projection";

    // camera object used for viewing and interacting with
    // the 3D scene
    Camera* g_pCamera = nullptr;

    // these variables are used for mouse movement processing
    float gLastX = WINDOW_WIDTH / 2.0f;
    float gLastY = WINDOW_HEIGHT / 2.0f;
    bool gFirstMouse = true;

    // time between current frame and last frame
    float gDeltaTime = 0.0f;
    float gLastFrame = 0.0f;

    // the following variable is false when orthographic projection
    // is off and true when it is on
    bool bOrthographicProjection = false;

    // Global pointer to ViewManager instance
    ViewManager* g_ViewManagerInstance = nullptr;
}

ViewManager::ViewManager() {
    cameraSpeed = 2.5f;
    cameraPosX = 0.0f;
    cameraPosY = 2.0f; // Adjusted Y position
    cameraPosZ = 5.0f; // Adjusted Z position
    cameraFrontX = 0.0f;
    cameraFrontY = 0.0f;
    cameraFrontZ = -1.0f;
    cameraUpX = 0.0f;
    cameraUpY = 1.0f;
    cameraUpZ = 0.0f;

    // Set the global instance pointer
    g_ViewManagerInstance = this;
}

void ViewManager::processInput(GLFWwindow* window, float deltaTime) {
    handleKeyboardInput(window, deltaTime);
    handleMouseInput(window);
}

void ViewManager::handleKeyboardInput(GLFWwindow* window, float deltaTime) {
    float velocity = cameraSpeed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPosX += cameraFrontX * velocity;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPosX -= cameraFrontX * velocity;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPosZ -= cameraFrontZ * velocity;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPosZ += cameraFrontZ * velocity;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        cameraPosY += velocity;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        cameraPosY -= velocity;
}

void ViewManager::handleMouseInput(GLFWwindow* window) {
    static double lastX = 400, lastY = 300;
    static bool firstMouse = true;

    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // Reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    // Update cameraFrontX, cameraFrontY, cameraFrontZ based on xoffset and yoffset

    // Scroll callback for adjusting speed
    glfwSetScrollCallback(window, ScrollCallback);
}

void ViewManager::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    if (g_ViewManagerInstance) {
        if (yoffset > 0)
            g_ViewManagerInstance->setCameraSpeed(g_ViewManagerInstance->cameraSpeed + 0.1f);
        else if (yoffset < 0)
            g_ViewManagerInstance->setCameraSpeed(g_ViewManagerInstance->cameraSpeed - 0.1f);
    }
}

void ViewManager::setCameraSpeed(float speed) {
    cameraSpeed = speed;
}

void ViewManager::switchToPerspective() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 4.0 / 3.0, 0.1, 100.0); // Ensure near and far planes are set correctly
    glMatrixMode(GL_MODELVIEW);
}

void ViewManager::switchToOrthographic() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-10, 10, -10, 10, -10, 10); // Adjust the orthographic projection bounds
    glMatrixMode(GL_MODELVIEW);
}

/***********************************************************
 *  ViewManager()
 *
 *  The constructor for the class
 ***********************************************************/
ViewManager::ViewManager(
    ShaderManager* pShaderManager)
{
    // initialize the member variables
    m_pShaderManager = pShaderManager;
    m_pWindow = NULL;
    g_pCamera = new Camera();
    // default camera view parameters
    g_pCamera->Position = glm::vec3(0.0f, 5.0f, 12.0f);
    g_pCamera->Front = glm::vec3(0.0f, -0.5f, -2.0f);
    g_pCamera->Up = glm::vec3(0.0f, 1.0f, 0.0f);
    g_pCamera->Zoom = 80;

    // Set the global instance pointer
    g_ViewManagerInstance = this;
}

/***********************************************************
 *  ~ViewManager()
 *
 *  The destructor for the class
 ***********************************************************/
ViewManager::~ViewManager()
{
    // free up allocated memory
    m_pShaderManager = NULL;
    m_pWindow = NULL;
    if (NULL != g_pCamera)
    {
        delete g_pCamera;
        g_pCamera = NULL;
    }

    // Reset the global instance pointer
    if (g_ViewManagerInstance == this) {
        g_ViewManagerInstance = nullptr;
    }
}

/***********************************************************
 *  CreateDisplayWindow()
 *
 *  This method is used to create the main display window.
 ***********************************************************/
GLFWwindow* ViewManager::CreateDisplayWindow(const char* windowTitle)
{
    GLFWwindow* window = nullptr;

    // try to create the displayed OpenGL window
    window = glfwCreateWindow(
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        windowTitle,
        NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return NULL;
    }
    glfwMakeContextCurrent(window);

    // tell GLFW to capture all mouse events
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // this callback is used to receive mouse moving events
    glfwSetCursorPosCallback(window, &ViewManager::Mouse_Position_Callback);

    // enable blending for supporting tranparent rendering
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_pWindow = window;

    return(window);
}

/***********************************************************
 *  Mouse_Position_Callback()
 *
 *  This method is automatically called from GLFW whenever
 *  the mouse is moved within the active GLFW display window.
 ***********************************************************/
void ViewManager::Mouse_Position_Callback(GLFWwindow* window, double xMousePos, double yMousePos)
{
}

/***********************************************************
 *  ProcessKeyboardEvents()
 *
 *  This method is called to process any keyboard events
 *  that may be waiting in the event queue.
 ***********************************************************/
void ViewManager::ProcessKeyboardEvents()
{
    // close the window if the escape key has been pressed
    if (glfwGetKey(m_pWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(m_pWindow, true);
    }
}

/***********************************************************
 *  PrepareSceneView()
 *
 *  This method is used for preparing the 3D scene by loading
 *  the shapes, textures in memory to support the 3D scene
 *  rendering
 ***********************************************************/
void ViewManager::PrepareSceneView()
{
    glm::mat4 view;
    glm::mat4 projection;

    // per-frame timing
    float currentFrame = glfwGetTime();
    gDeltaTime = currentFrame - gLastFrame;
    gLastFrame = currentFrame;

    // process any keyboard events that may be waiting in the 
    // event queue
    ProcessKeyboardEvents();

    // get the current view matrix from the camera
    view = g_pCamera->GetViewMatrix();

    // define the current projection matrix
    projection = glm::perspective(glm::radians(g_pCamera->Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);

    // if the shader manager object is valid
    if (NULL != m_pShaderManager)
    {
        // set the view matrix into the shader for proper rendering
        m_pShaderManager->setMat4Value(g_ViewName, view);
        // set the view matrix into the shader for proper rendering
        m_pShaderManager->setMat4Value(g_ProjectionName, projection);
        // set the view position of the camera into the shader for proper rendering
        m_pShaderManager->setVec3Value("viewPosition", g_pCamera->Position);
    }
}