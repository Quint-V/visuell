
// Local headers
#include "program.hpp"
#include "gloom/gloom.hpp"
#include "gloom/shader.hpp"


void runProgram(GLFWwindow* window)
{
	// Enable depth (Z) buffer (accept "closest" fragment)
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Configure miscellaneous OpenGL settings
	glEnable(GL_CULL_FACE);

	// Set default colour after clearing the colour buffer
	glClearColor(0.3f, 0.5f, 0.8f, 1.0f);

	// Set up your scene here (create Vertex Array Objects, etc.)

	float coordinates[] = {

		0.6f,	-0.8f,	-1.2f,
		0.0f,	 0.4f,	 0.0f,
		-0.8f,	-0.2f,	 1.2f,

		//-0.8f,	 0.7f,	0.0f,
		//-0.8f,	 0.0f,	0.0f,
		//-0.6f,	 0.0f,	0.0f,

		//-0.5f,	 -0.7f,	0.0f,
		//-0.3f,	 -0.7f,	0.0f,
		//-0.3f,	 -0.0f,	0.0f,

		//-0.2f,	 0.7f,	0.0f,
		//-0.2f,	 0.0f,	0.0f,
		//-0.0f,	 0.0f,	0.0f,

		//0.2f,	 -0.7f,	0.0f,
		//0.4f,	 -0.7f,	0.0f,
		//0.4f,	 -0.0f,	0.0f,

		//0.6f,	 0.7f,	0.0f,
		//0.6f,	 0.0f,	0.0f,
		//0.8f,	 0.0f,	0.0f,

	};

	//	/* working triangle, horizontal coord */ 
	//	-0.6f,	-0.6f,	0.0f,
	//	 0.6f,	-0.6f,	0.0f,
	//	 0.0f,	 0.6f,	0.0f,
	//
	//	
	//	/* clipped triangle horiz coord */
	//	0.6f,	-0.8f,	-1.2f,
	//	0.0f,	 0.4f,	 0.0f,
	//	-0.8f,	-0.2f,	 1.2f,



	int indices[] = { 0, 1, 2, };// 3, 4, 5,  6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23 }; // task 1b)

	unsigned int VAO_ID;

	VAO_ID = VAOsetup(coordinates, sizeof(coordinates), indices, sizeof(indices));
	printGLError();

	Gloom::Shader shader;
	shader.makeBasicShader("D:\\GitHub\\gloom\\gloom\\shaders\\simple.vert",
		"D:\\GitHub\\gloom\\gloom\\shaders\\simple.frag");
	// Rendering Loop
	while (!glfwWindowShouldClose(window))
	{
		// Clear colour and depth buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Draw your scene here
		shader.activate();
		glBindVertexArray(VAO_ID);
		glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, 0);
		printGLError();

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
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}

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