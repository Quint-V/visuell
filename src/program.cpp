
// Local headers
#include "program.hpp"
#include "gloom/gloom.hpp"
#include "gloom/shader.hpp"
#include "glm/mat4x4.hpp"
#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

// setup for view manipulation, must be available to all functions.

float pi = 3.14159265359f;
float fov = 2 * pi* 75.0f / 360.0f;
glm::vec4 newPos; // extract [0,1,2] from the new position after 4x4 * 4x1 = 4x1;
glm::vec4 newDir; // extract [0,1,2] from the new direction
glm::mat4x4 projectionMatrix = glm::perspective(fov, 1024.0f / 768.0f, 1.0f, 100.0f);
glm::mat4x4 viewMatrix;
glm::mat4x4 viewProj;
float t = 0.0f;
float angleStep = 0.75f/360.0f * 2.0f*pi;
float positionStep = 0.0015f;
glm::mat4x4 idMatrix = glm::mat4x4(1.0f);

// init cam values:
glm::vec3 camPos = glm::vec3(0.0f, 0.0f, 0.0f);;
glm::vec3 camDirection = glm::vec3(0.0f, 0.0f, 1.0f);
glm::vec3 camUp = glm::vec3(0.0f, 1.0f, 0.0f);

glm::vec3 storedPosition;

void runProgram(GLFWwindow* window)
{
	// Enable depth (Z) buffer (accept "closest" fragment)
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Configure miscellaneous OpenGL settings
	glEnable(GL_CULL_FACE);

	// Set default colour after clearing the colour buffer
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	
	// Transparency - not as popular as it should be
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Set up your scene here (create Vertex Array Objects, etc.)

	float coordinates[] = {

		// further away= travel along +z direction
		-0.6f,	 0.8f,	 (0.6f + 3.0f), 
		-0.6f,	 0.0f,	 (0.6f + 3.0f), 
		 0.4f,	 0.6f,	 (0.6f + 3.0f), 

		-0.6f,	 -0.6f,	 (0.4f + 3.0f), 
		 0.3f,	 -0.6f,	 (0.4f + 3.0f), 
		 0.0f,	  0.9f,	 (0.4f + 3.0f), 

		-0.0f,	 1.0f,	 (0.2f + 3.0f), 
		-0.0f,	 0.0f,	 (0.2f + 3.0f), 
		 1.0f,	 0.0f,	 (0.2f + 3.0f), 


		// facing the other direction...

		-0.6f,	 0.8f,	  (0.6f + 3.0f),
		0.4f,	 0.6f,	  (0.6f + 3.0f),
		-0.6f,	 0.0f,	  (0.6f + 3.0f),

		-0.6f,	 -0.6f,	  (0.4f + 3.0f),
		0.0f,	  0.9f,	  (0.4f + 3.0f),
		0.3f,	 -0.6f,	  (0.4f + 3.0f),

		-0.0f,	 1.0f,	 (0.2f + 3.0f),
		1.0f,	 0.0f,	 (0.2f + 3.0f),
		-0.0f,	 0.0f,	 (0.2f + 3.0f)
	};

	float colours[] = {

		1.0f,	0.0f,	0.9f,	0.5f,	// tri1
		1.0f,	0.9f,	0.0f,	0.5f,
		1.0f,	0.0f,	0.0f,	0.5f,

		0.9f,	1.0f,	0.0f,	0.5f,	// tri2
		0.0f,	1.0f,	0.0f,	0.5f,
		0.0f,	1.0f,	0.9f,	0.5f,

		0.0f,	0.9f,	1.0f,	0.5f,	// tri3
		0.9f,	0.0f,	1.0f,	0.5f,
		0.0f,	0.0f,	1.0f,	0.5f,	

		1.0f,	0.0f,	0.9f,	0.5f,	// tri1
		1.0f,	0.0f,	0.0f,	0.5f,
		1.0f,	0.9f,	0.0f,	0.5f,

		0.9f,	1.0f,	0.0f,	0.5f,	// tri2
		0.0f,	1.0f,	0.9f,	0.5f,
		0.0f,	1.0f,	0.0f,	0.5f,

		0.0f,	0.9f,	1.0f,	0.5f,	// tri3
		0.0f,	0.0f,	1.0f,	0.5f,	
		0.9f,	0.0f,	1.0f,	0.5f,


	};
	
	int indices[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, };// 18, 19, 20, 21, 22, 23 }; // task 1b)

	unsigned int VAO_ID;


//	VAO_ID = VAOsetup(coordinates, sizeof(coordinates), indices, sizeof(indices));
	VAO_ID = VAOsetup_colourized(coordinates, sizeof(coordinates)*sizeof(float), 
		indices, sizeof(indices), colours, sizeof(colours));
	printGLError();

	Gloom::Shader shader;
	shader.makeBasicShader("D:\\__Skole, NTNU\\Visuell informasjonsbehandling\\gloom\\gloom\\shaders\\simple.vert",
		"D:\\__Skole, NTNU\\Visuell informasjonsbehandling\\gloom\\gloom\\shaders\\simple.frag");

	glm::mat4 modifier = idMatrix;

	// Rendering Loop

	int timer = 0;

	while (!glfwWindowShouldClose(window))
	{

		timer += 1;
		// Clear colour and depth buffers
		//	glUniformMatrix4fv(10, 1, GL_FALSE, glm::value_ptr(viewer));
		
		// Transformation matrix setup and modification
		if (timer % 3000 == 0){
			printf("timer is: %i \n", timer);
			printf("cam position  : %f, %f, %f \n", camPos[0], camPos[1], camPos[2]);
			printf("view direction: %f, %f, %f \n\n\n\ ", camDirection[0], camDirection[1], camDirection[2]);
		}
		// column major matrix testing used for task 3 b
		// matrix accessing: [vertical ][horizontal]
		//  * sin(t);
		//	* cos(t);
		//	modifier[0][0] = .8f * cos(t);	//stretch		(+shear=shear across arbitrary axis)
		//	modifier[0][1] = .8f * sin(t);	//shear
		//	modifier[1][0] = .8f * -sin(t);	//shear	
		//	modifier[1][1] = .8f * cos(t);	//stretch
		//	modifier[3][0] = .8f * -sin(t);	//translation
		//	modifier[3][1] = .8f * cos(t);	//translation

		// insert commands for change in position and orientation;
		// TODO: rotate across x-axis, y-axis
		// TODO: change position along x, y, z axis.
		viewMatrix = glm::lookAt(camPos, camPos + camDirection, camUp);
		viewProj= projectionMatrix * viewMatrix;
		glUniformMatrix4fv(10, 1, GL_FALSE, glm::value_ptr(viewProj));



		// Clear colour and depth buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Draw your scene here
		shader.activate();

		glBindVertexArray(VAO_ID);
		glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, 0);

		// Handle other events
		glfwPollEvents();
		handleKeyboardInput(window);

		// Flip buffers
		glfwSwapBuffers(window);
	}
	printGLError();
	shader.deactivate();
}

void handleKeyboardInput(GLFWwindow* window)
{
	// Use escape key for terminating the GLFW window
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	float normalSquared = camDirection[0]* camDirection[0] + camDirection[1]* camDirection[1] + camDirection[2]* camDirection[2];
	float normal = sqrt(normalSquared);

	// FINISHED: movement across major axes; rotation around x and y axis. 
	// (rotation around z would be like turning your head around)

	// reset button
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) { // move north
		camPos = glm::vec3(0.0f, 0.0f, 0.0f);
		camDirection = glm::vec3(0.0f, 0.0f, 1.0f);
	}

	

	// proper method: modify matrices.	DIRECTIONS: +x = west, +y = up, +z = north
	// find define movement/rotation, the resulting vector from matrix * vector, extract proper data
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) { // move north
		viewMatrix = glm::translate(idMatrix, glm::vec3(0.0f, 0.0f, positionStep));
		newPos = viewMatrix * glm::vec4(camPos, 1.0f);
		camPos = glm::vec3(newPos[0], newPos[1], newPos[2]);
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) { // move south
		viewMatrix = glm::translate(idMatrix, glm::vec3(0.0f, .0f, -positionStep));
		newPos = viewMatrix * glm::vec4(camPos, 1.0f);
		camPos = glm::vec3(newPos[0], newPos[1], newPos[2]);
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) { // move west
		viewMatrix = glm::translate(idMatrix, glm::vec3(positionStep, .0f, 0.0f));
		newPos = viewMatrix * glm::vec4(camPos, 1.0f);
		camPos = glm::vec3(newPos[0], newPos[1], newPos[2]);
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {	// move east
		viewMatrix = glm::translate(idMatrix, glm::vec3(-positionStep, .0f, .0f));
		newPos = viewMatrix * glm::vec4(camPos, 1.0f);
		camPos = glm::vec3(newPos[0], newPos[1], newPos[2]);
	}

	if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS) { // move up
		viewMatrix = glm::translate(idMatrix, glm::vec3(0.0f, positionStep, 0.0f));
		newPos = viewMatrix * glm::vec4(camPos, 1.0f);
		camPos = glm::vec3(newPos[0], newPos[1], newPos[2]);
	}

	if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS) { // move down
		viewMatrix = glm::translate(idMatrix, glm::vec3(0.0f, -positionStep, 0.0f));
		newPos = viewMatrix * glm::vec4(camPos, 1.0f);
		camPos = glm::vec3(newPos[0], newPos[1], newPos[2]);
	}


	// rotation directions: across x axis, and y axis.
	// order of operations: undo translation, rotate, redo translation.
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {	// look up
		viewMatrix = glm::rotate(viewMatrix, -0.001f, glm::vec3(1.0f, 0.0f, 0.0f));
		newDir = glm::rotate(idMatrix, 0.001f, glm::vec3(-1.0f, 0.0f, 0.0f)) * glm::vec4(camDirection,1.0f);
		camDirection = glm::normalize(glm::vec3(newDir[0], newDir[1], newDir[2]) );
	}

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {	// look down
		viewMatrix = glm::rotate(viewMatrix, 0.001f, glm::vec3(1.0f, 0.0f, 0.0f));
		newDir = glm::rotate(idMatrix, 0.001f, glm::vec3(1.0f, 0.0f, 0.0f)) * glm::vec4(camDirection, 1.0f);
		camDirection = glm::normalize(glm::vec3(newDir[0], newDir[1], newDir[2]));
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {	// turn left
		viewMatrix = glm::rotate(viewMatrix, 0.001f, glm::vec3(0.0f, 1.0f, 0.0f));
		newDir = glm::rotate(idMatrix, 0.001f, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(camDirection, 1.0f);
		camDirection = glm::normalize(glm::vec3(newDir[0], newDir[1], newDir[2]));
	}

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {	// turn right
		viewMatrix = glm::rotate(viewMatrix, -0.001f, glm::vec3(0.0f, 1.0f, 0.0f));
		newDir = glm::rotate(idMatrix, -0.001f, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(camDirection, 1.0f);
		camDirection = glm::normalize(glm::vec3(newDir[0], newDir[1], newDir[2]));
	}


	// naive methods - modify vectors. Rotation limited to 90 degree angles

	// position directions: north, west, south, east, up, down.

	//if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) { // move north
	//	camPos[2] += positionStep;
	//}

	//if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) { // move south
	//	camPos[2] -= positionStep;
	//}

	//if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) { // move west
	//	camPos[0] += positionStep;
	//}

	//if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {	// move east
	//	camPos[0] -= positionStep;
	//}

	//if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS) { // move up
	//	camPos[1] += positionStep;
	//}

	//if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS) { // move down
	//	camPos[1] -= positionStep;
	//}

	//// rotation directions: across x axis, and y axis.
	//if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {	// look up
	//	camDirection[1] += angleStep;
	//}

	//if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {	// look down
	//	camDirection[1] -= angleStep;
	//}

	//if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {	// turn left
	//	camDirection[0] += angleStep;
	//}

	//if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {	// turn right
	//	camDirection[0] -= angleStep;
	//}



}

unsigned int VAOsetup_colourized(float* coords, int coordBytes, int* indices, int indexBytes,
									float* colours, int coloursBytes) {

	unsigned int VAOID = 0; // return this

	// "Follow the white rabbit."



	// init. setup; allocate & bind pointers
	glGenVertexArrays(1, &VAOID);
	glBindVertexArray(VAOID);

	// prep. indexing, pass data to GPU
	unsigned int bufferID = 0;
	glGenBuffers(1, &bufferID);
	glBindBuffer(GL_ARRAY_BUFFER, bufferID);
	glBufferData(GL_ARRAY_BUFFER, coordBytes, coords, GL_STATIC_DRAW);

	// format; lock position onto layout(location=index);
	int layoutLocation = 0;
	glVertexAttribPointer(layoutLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(layoutLocation);

	// finalize indexing //
	unsigned int indexingID = 0;
	glGenBuffers(1, &indexingID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexingID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBytes, indices, GL_STATIC_DRAW);

	// prep. colouring
	unsigned int colourID = 0;
	glGenBuffers(1, &colourID);
	glBindBuffer(GL_ARRAY_BUFFER, colourID);
	glBufferData(GL_ARRAY_BUFFER, coloursBytes, colours, GL_STATIC_DRAW);
	
	// format; lock location index of colouring, into layout(location=[...])
	layoutLocation = 1;
	glVertexAttribPointer(layoutLocation, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(layoutLocation);

	unsigned int positionID = 0;
	glGenBuffers(1, &positionID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexingID);

	return VAOID;
}

 // cam movement

//void camNorth(float* position) {
//
//}
//
//void camNorth(float* position) {
//
//}
//
//void camSouth(float* position) {
//
//}
//
//void camUpward(float* position){
//
//}
//
//void camDownward(float* position) {
//
//}
//
//void camWest(float* position) {
//
//}
//
//void camEast(float* position) {
//
//}






// redacted
unsigned int VAOsetup(float* coords, int coordBytes, int* indices, int indexBytes) {

	unsigned int VAOID = 0; // return this

	// init. setup; allocate & bind pointers
	glGenVertexArrays(1, &VAOID);
	glBindVertexArray(VAOID);

	// prep. indexing, pass data to GPU
	unsigned int buffer_ID = 0;
	glGenBuffers(1, &buffer_ID);
	glBindBuffer(GL_ARRAY_BUFFER, buffer_ID);
	glBufferData(GL_ARRAY_BUFFER, coordBytes, coords, GL_STATIC_DRAW);

	int just2Bsure = 0;
	glVertexAttribPointer(just2Bsure, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(just2Bsure);

	// finalize indexing //
	unsigned int indexingID = 0;
	glGenBuffers(1, &indexingID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexingID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBytes, indices, GL_STATIC_DRAW);

	return VAOID;
}

