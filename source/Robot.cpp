#define GLM_FORCE_RADIANS

#include <gl/glew.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include "Robot.h"
#include <gl\glm\glm\glm.hpp>
#include <gl\glm\glm\gtc\type_ptr.hpp>
#include <gl\glm\glm\gtc\matrix_transform.hpp> 
#include <gl\glm\glm\gtx\rotate_vector.hpp>
#include <gl\glm\glm\gtx\transform.hpp>
#ifndef NDEBUG
#include <utils\GLSupport.h>
#include <Editor\DebugMenu.h>
#endif


#define BUFFER_OFFSET(i) ((char *)NULL + (i))

vector<GLfloat> red = {
	1.0f,0.0f,0.0f,
	1.0f,0.0f,0.0f,
	1.0f,0.0f,0.0f,
	1.0f,0.0f,0.0f,
	1.0f,0.0f,0.0f,
	1.0f,0.0f,0.0f,
	1.0f,0.0f,0.0f,
	1.0f,0.0f,0.0f
};

vector<GLfloat> white = {
	1.0f,1.0f,1.0f,
	1.0f,1.0f,1.0f,
	1.0f,1.0f,1.0f,
	1.0f,1.0f,1.0f,
	1.0f,1.0f,1.0f,
	1.0f,1.0f,1.0f,
	1.0f,1.0f,1.0f,
	1.0f,1.0f,1.0f
};
vector<GLfloat> blue = {
	0.0f,0.0f,1.0f,
	0.0f,0.0f,1.0f,
	0.0f,0.0f,1.0f,
	0.0f,0.0f,1.0f,
	0.0f,0.0f,1.0f,
	0.0f,0.0f,1.0f,
	0.0f,0.0f,1.0f,
	0.0f,0.0f,1.0f
};

vector<GLfloat> yellow = {
	1.0f,1.0f,0.0f,
	1.0f,1.0f,0.0f,
	1.0f,1.0f,0.0f,
	1.0f,1.0f,0.0f,
	1.0f,1.0f,0.0f,
	1.0f,1.0f,0.0f,
	1.0f,1.0f,0.0f,
	1.0f,1.0f,0.0f
};

Robot::Robot(shared_ptr<Shader> shaderPtr)
{
	shader = shaderPtr;
	armAngles[LEFT_DIR] = 0.0;
	armAngles[RIGHT_DIR] = 0.0;
	legAngles[LEFT_DIR] = 0.0;
	legAngles[RIGHT_DIR] = 0.0;

	armStates[LEFT_DIR] = FORWARD_STATE;
	armStates[RIGHT_DIR] = BACKWARD_STATE;

	legStates[LEFT_DIR] = FORWARD_STATE;
	legStates[RIGHT_DIR] = BACKWARD_STATE;

	vector<GLfloat> vertices = {
		-1.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, -1.0f, -1.0f,
		0.0f, -1.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		-1.0f, 0.0f, -1.0f,
	};

	vector<GLubyte> indices = {
		0,1,2, //Front face
		0,2,3,
		1,5,6, // Right face
		1,6,2,
		4,5,6, //back face
		4,6,7,
		4,0,3, //left face
		4,3,7,
		3,2,6, //top face
		3,6,7,
		0,1,5, //bottom face
		0,5,4
	};

	glGenBuffers(3, vboHandles);
	//Create IBO
	indiceBuffer = vboHandles[0];
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indiceBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLubyte) * indices.size(), &indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	//Create VBO
	vertexBuffer = vboHandles[1];
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//Create VBO for colours
	colourBuffer = vboHandles[2];
	glBindBuffer(GL_ARRAY_BUFFER, colourBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * red.size(), &red[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//unbind VAO
	//glBindVertexArray(0);
	indexSize = indices.size();

	robot_front_Dir = glm::vec3(0.0, 0.0, -1.0);
	robot_Pos = glm::vec3(0.0, 11.5, -20.0);
	rotationAngle = 0.0f;
}

Robot::~Robot()
{
}

unsigned int Robot::getOrCreateVAO()
{
	if (vaoHandle != 0) return vaoHandle;
	//Create Vertex Array Object
	glGenVertexArrays(1, &vaoHandle);
	glBindVertexArray(vaoHandle);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	//Bind VBOs to VAO
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, (GLubyte *)NULL);
	glBindBuffer(GL_ARRAY_BUFFER, colourBuffer);
	glVertexAttribPointer(1, 3, GL_FLOAT, false, 0, (GLubyte *)NULL);
	//Bind IBO to VAO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indiceBuffer);
	return vaoHandle;
}

void Robot::DrawCube(glm::mat4 modelMatrix, vector<GLfloat> colour)
{
	shader->bindShader();
	glBindVertexArray(getOrCreateVAO());
	glBindBuffer(GL_ARRAY_BUFFER, vboHandles[2]); //Set colour buffer values
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * colour.size(), &colour[0], GL_STATIC_DRAW);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	shader->setUniform("mModel", modelMatrix);
#ifndef NDEBUG
	string check = OpenGLSupport().GetError();
#endif
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indexSize), GL_UNSIGNED_BYTE, BUFFER_OFFSET(0));
	glBindVertexArray(0);
}

void Robot::DrawArm(float xPos, float yPos, float zPos, glm::mat4 modelMatrix)
{
	DrawCube(glm::scale(modelMatrix, glm::vec3(1.0f, 4.0f, 1.0f))*glm::translate(glm::vec3(xPos, yPos, zPos)), red);
}

void Robot::DrawHead(float xPos, float yPos, float zPos, glm::mat4 modelMatrix)
{
	glm::mat4 model = modelMatrix * glm::translate(glm::vec3(xPos, yPos, zPos));
	DrawCube(glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f)), white);
}

void Robot::DrawTorso(float xPos, float yPos, float zPos, glm::mat4 modelMatrix)
{
	glm::mat4 model = modelMatrix * glm::translate(glm::vec3(xPos, yPos, zPos));
	DrawCube(glm::scale(model, glm::vec3(3.0f, 5.0f, 2.0f)), blue);
}

void Robot::DrawLeg(float xPos, float yPos, float zPos, glm::mat4 modelMatrix)
{
	glm::mat4 model = modelMatrix *glm::translate(glm::vec3(xPos, yPos, zPos));
	DrawFoot(0.0f, -5.0f, 0.0f, model * glm::translate(glm::vec3(0.0f, -0.5f, 0.0f)));
	DrawCube(glm::scale(model, glm::vec3(1.0f, 5.0f, 1.0f)), yellow);
}

void Robot::DrawFoot(float xPos, float yPos, float zPos, glm::mat4 modelMatrix)
{
	glm::mat4 model = modelMatrix * glm::translate(glm::vec3(xPos, yPos, zPos));
	DrawCube(glm::scale(model, glm::vec3(1.0f, 0.5f, 3.0f)), white);
}

void Robot::updateCamera()
{
	if(camera != nullptr)
		camera->move(glm::vec3(robot_Pos.x, robot_Pos.y, robot_Pos.z));
}
#ifndef NDEBUG
void Robot::debugMenuItemUpdate()
{
	if (ImGui::BeginMenu("Robot"))
	{
		ImGui::EndMenu();
	}
}
#endif
void Robot::DrawRobot(glm::mat4 viewMatrix, glm::mat4 projectionMatrix)
{
	shader->bindShader();
	shader->setUniform("mView", viewMatrix);
	shader->setUniform("mProjection", projectionMatrix);
	glm::mat4 model = glm::translate(robot_Pos) * glm::rotate(glm::radians(rotationAngle), glm::vec3(0, 1, 0));

	// draw head and torso parts
	DrawHead(1.0f, 2.0f, 0.0f, model);
	DrawTorso(1.5f, 0.0f, 0.0f, model);

	if (animate) {

		//move the left arm away from the torso and rotate it to give "walking" effect
		DrawArm(2.5f, 0.0f, -0.5f, model * glm::rotate(glm::radians(armAngles[LEFT]), glm::vec3(1.0f, 0.0f, 0.0f)) * glm::translate(glm::vec3(0.0f, -0.5f, 0.0f)));

		// move the right arm away from the torso and rotate it to give "walking" effect
		DrawArm(-1.5f, 0.0f, -0.5f, model * glm::rotate(glm::radians(armAngles[RIGHT]), glm::vec3(1.0f, 0.0f, 0.0f)) * glm::translate(glm::vec3(0.0f, -0.5f, 0.0f)));

		// move the left leg away from the torso and rotate it to give "walking" effect		
		DrawLeg(-0.5f, -5.0f, -0.5f, model * glm::rotate(glm::radians(legAngles[LEFT]), glm::vec3(1.0f, 0.0f, 0.0f)) * glm::translate(glm::vec3(0.0f, -0.5f, 0.0f)));

		// move the right leg away from the torso and rotate it to give "walking" effect
		DrawLeg(1.5f, -5.0f, -0.5f, model * glm::rotate(glm::radians(legAngles[RIGHT]), glm::vec3(1.0f, 0.0f, 0.0f))*glm::translate(glm::vec3(0.0f, -0.5f, 0.0f)));
	}
	else {
		DrawArm(2.5f, 0.0f, -0.5f, model * glm::translate(glm::vec3(0.0f, -0.5f, 0.0f)));
		DrawArm(-1.5f, 0.0f, -0.5f, model * glm::translate(glm::vec3(0.0f, -0.5f, 0.0f)));
		DrawLeg(-0.5f, -5.0f, -0.5f, model * glm::translate(glm::vec3(0.0f, -0.5f, 0.0f)));
		DrawLeg(1.5f, -5.0f, -0.5f, model *glm::translate(glm::vec3(0.0f, -0.5f, 0.0f)));
	}

}

void Robot::Prepare(double dt)
{
	// if leg is moving forward, increase angle, else decrease angle

	for (char side = 0; side < 2; side++)
	{
		// arms
		if (armStates[side] == FORWARD_STATE)
			armAngles[side] += 20.0f * dt;
		else
			armAngles[side] -= 20.0f * dt;

		// change state if exceeding angles
		if (armAngles[side] >= 15.0f)
			armStates[side] = BACKWARD_STATE;
		else if (armAngles[side] <= -15.0f)
			armStates[side] = FORWARD_STATE;

		// legs
		if (legStates[side] == FORWARD_STATE)
			legAngles[side] += 20.0f * dt;
		else
			legAngles[side] -= 20.0f * dt;

		// change state if exceeding angles
		if (legAngles[side] >= 15.0f)
			legStates[side] = BACKWARD_STATE;
		else if (legAngles[side] <= -15.0f)
			legStates[side] = FORWARD_STATE;
	}

}
void Robot::setAnimate(bool flag)
{
	animate = flag;
}

bool Robot::handle(MouseEvent& event)
{
	//NOP
	return false;
}

bool Robot::handle(KeyEvent& event)
{
	if (event.type == KeyEventType::KEY_REPEATED || event.type == KeyEventType::KEY_PRESSED)
	{
		switch (event.key)
		{
		case KeyCodes::W: //W
			robot_Pos.x += robot_front_Dir.x*movementSpeed;
			robot_Pos.z += robot_front_Dir.z*movementSpeed;
			updateCamera();
			setAnimate(true);
			break;
		case KeyCodes::S: //S
			robot_Pos.x -= robot_front_Dir.x*movementSpeed;
			robot_Pos.z -= robot_front_Dir.z*movementSpeed;
			updateCamera();
			setAnimate(true);
			break;
		case KeyCodes::A: //A
			rotationAngle += rotationSpeed;
			robot_front_Dir = glm::rotateY(robot_front_Dir, glm::radians(rotationSpeed));
			updateCamera();
			setAnimate(true);
			break;
		case KeyCodes::D: //D
			rotationAngle -= rotationSpeed;
			robot_front_Dir = glm::rotateY(robot_front_Dir, glm::radians(-rotationSpeed));
			updateCamera();
			setAnimate(true);
			break;
		default:
			break;
		}
	}
	else if (event.type == KeyEventType::KEY_RELEASED) {
		setAnimate(false);
	}
	return false;
}

glm::vec3 Robot::getPosition()
{
	return robot_Pos;
}

//bool Robot::checkCollision(shared_ptr<Model> model)
//{
//	glm::vec2 dist(robot_Pos.x - model->transform.position.x, robot_Pos.z - model->transform.position.z);
//	float distance = glm::length(dist);
//	return  distance < 5;
//}

void Robot::setCamera(shared_ptr<Camera> newCamera)
{
	camera = newCamera;
	camera->move(glm::vec3(robot_Pos.x, robot_Pos.y , robot_Pos.z));
	camera->lookAt(robot_front_Dir );
}

shared_ptr<Camera> Robot::getCamera()
{
	return camera;
}
