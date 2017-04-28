#ifndef __ROBOT_H
#define __ROBOT_H
#include <Graphics\Shader.h>
#include <memory>
using std::shared_ptr;
#include <gl\glm\glm\glm.hpp>
#include <vector>
using std::vector;
#include <GUI\EventListener.h>
#include <Camera\Camera.h>
#ifndef NDEBUG
#include <Editor\DebugMenuItem.h>
#endif
// constants for arm and leg movement states
const char BACKWARD_STATE = 0;
const char FORWARD_STATE = 1;

// index constants for accessing arm and leg array data
const char LEFT_DIR = 0;
const char RIGHT_DIR = 1;

/*
	Player Character
*/
#ifndef NDEBUG
class Robot : public EventListener, DebugMenuItem
#else
class Robot : public EventListener
#endif
{
private:
	char legStates[2];
	char armStates[2];

	float legAngles[2];
	float armAngles[2];

	unsigned int vboHandles[3];
	unsigned int vaoHandle = 0;
	unsigned int indiceBuffer;
	unsigned int vertexBuffer;
	unsigned int colourBuffer;
	size_t indexSize;

	shared_ptr<Shader> shader;

	bool animate = false;

	glm::vec3 robot_front_Dir;
	glm::vec3 robot_Pos;
	float rotationAngle;

	float rotationSpeed = 4.0f;
	float movementSpeed = 1.5f;

	//As vertex data is loaded on a separate thread, we may have to recrate the VAO before rendering
	unsigned int getOrCreateVAO();

	shared_ptr<Camera> camera;

	// draws a unit cube
	void DrawCube(glm::mat4 modelMatrix, vector<float> colour);

	// methods to draw the parts of the robot
	void DrawArm(float xPos, float yPos, float zPos, glm::mat4 modelMatrix);
	void DrawHead(float xPos, float yPos, float zPos, glm::mat4 modelMatrix);
	void DrawTorso(float xPos, float yPos, float zPos, glm::mat4 modelMatrix);
	void DrawLeg(float xPos, float yPos, float zPos, glm::mat4 modelMatrix);
	void DrawFoot(float xPos, float yPos, float zPos, glm::mat4 modelMatrix);

	void updateCamera();
#ifndef NDEBUG
	void debugMenuItemUpdate();
#endif

public:

	Robot(shared_ptr<Shader> shaderPtr);
	virtual ~Robot();

	// draws the entire robot
	void DrawRobot(glm::mat4 viewMatrix, glm::mat4 projectionMatrix);

	// updates the robot data
	void Prepare(double dt);

	//Set the animation flag
	void setAnimate(bool flag);

	bool  handle(MouseEvent& event);
	bool  handle(KeyEvent& event);
	glm::vec3 getPosition();
	//bool checkCollision(shared_ptr<Model> model);

	void setCamera(shared_ptr<Camera> newCamera);
	shared_ptr<Camera> getCamera();
};

#endif