///////////////////////////////////////////////////////////////////////////////
// viewmanager.h
// ============
// manage the viewing of 3D objects within the viewport
//
//  AUTHOR: Brian Battersby - SNHU Instructor / Computer Science
//	Created for CS-330-Computational Graphics and Visualization, Nov. 1st, 2023
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ShaderManager.h"
#include "camera.h"

// GLFW library
#include "GLFW/glfw3.h" 

class ViewManager
{
public:
	// constructor
	ViewManager();
	ViewManager(ShaderManager* pShaderManager);
	~ViewManager();

	void processInput(GLFWwindow* window, float deltaTime);
	static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	void switchToPerspective();
	void switchToOrthographic();
	void PrepareSceneView();

	// Public setter for cameraSpeed
	void setCameraSpeed(float speed);
	// mouse position callback for mouse interaction with the 3D scene

private:
	// pointer to shader manager object
	ShaderManager* m_pShaderManager;
	// active OpenGL display window
	GLFWwindow* m_pWindow;
	
	float cameraSpeed;
	float cameraPosX, cameraPosY, cameraPosZ;
	float cameraFrontX, cameraFrontY, cameraFrontZ;
	float cameraUpX, cameraUpY, cameraUpZ;

	// Other private methods and members
	void handleKeyboardInput(GLFWwindow* window, float deltaTime);
	void handleMouseInput(GLFWwindow* window);
	static void Mouse_Position_Callback(GLFWwindow* window, double xMousePos, double yMousePos);
	// process keyboard events for interaction with the 3D scene
	void ProcessKeyboardEvents();

public:
	// create the initial OpenGL display window
	GLFWwindow* CreateDisplayWindow(const char* windowTitle);
	
	// prepare the conversion from 3D object display to 2D scene display
};