//#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <thread>

#include <glm\glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glviewer/viewer.h>
#include <glviewer/camera.h>

#include <nori/cameras/camera.h>
#include <nori/core/bitmap.h>
#include <nori/core/block.h>
#include <nori/core/common.h>
#include <nori/core/gui.h>
#include <nori/core/scene.h>
#include <nori/core/timer.h>
#include <nori/integrators/integrator.h>
#include <nori/samplers/sampler.h>

#include <nanogui/screen.h>
NAMESPACE_BEGIN(nanogui)
extern std::map<GLFWwindow *, Screen *> __nanogui_screens;
NAMESPACE_END(nanogui)

//#include <nori/core/timer.h>

#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>

VIEWER_NAMESPACE_BEGIN

// --------------------- //
// -- Callbacks Setup -- //
// --------------------- //
static void error_callback(int error, const char* description)
{
	Viewer::getInstance().error_callback_impl(error, description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Viewer::getInstance().key_callback_impl(window, key, scancode, action, mods);
}
static void window_size_callback(GLFWwindow* window, int width, int height)
{
	Viewer::getInstance().window_size_callback_impl(window, width, height);
}
static void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{	
	Viewer::getInstance().mouse_callback_impl(window, xpos, ypos);
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	Viewer::getInstance().mouse_button_callback_impl(window, button, action, mods);
}

static void cursor_pos_callback(GLFWwindow* window, double x, double y) {
	Viewer::getInstance().cursor_pos_callback_impl(window, x, y); 
}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	Viewer::getInstance().scroll_callback_impl(window, xoffset, yoffset); 
}

static void char_callback(GLFWwindow* window, unsigned int codepoint) {
	Viewer::getInstance().char_callback_impl(window, codepoint); 
}

static void drop_callback(GLFWwindow* window, int count, const char **filenames) {
	Viewer::getInstance().drop_callback_impl(window, count, filenames);
}

static void frame_buffer_size_callback(GLFWwindow* window, int width, int height) {
	Viewer::getInstance().frame_buffer_impl(window, width, height);
}

// ------------------------------- //
// -- Callbacks implementations -- //
// ------------------------------- //
void Viewer::error_callback_impl(int error, const char* description)
{
	fputs(description, stderr);
}

void Viewer::key_callback_impl(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Escape application
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	// Wireframe management
	if (key == GLFW_KEY_L && action == GLFW_PRESS)
	{
		if (m_wireFrameEnabled)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			m_wireFrameEnabled = false; 
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			m_wireFrameEnabled = true;
		}
			
	}
	
	// Realtime (key held) interaction management
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			m_keys[key] = true;
		else if (action == GLFW_RELEASE)
			m_keys[key] = false;
	}
}

void Viewer::window_size_callback_impl(GLFWwindow* window, int width, int height)
{
	glfwSetWindowSize(window, width, height); 
	m_width = width; 
	m_height = height; 
}

void Viewer::mouse_callback_impl(GLFWwindow* window, double xpos, double ypos)
{	
	if (m_mouseIsClicked)
	{
		if (m_firstClick)
		{
			m_lastMousePosition.x = xpos;
			m_lastMousePosition.y = ypos;
			m_firstClick = false;
		}

		GLfloat xoffset = xpos - m_lastMousePosition.x;
		GLfloat yoffset = m_lastMousePosition.y - ypos; // Y coordinates range from bottom to top

		m_lastMousePosition.x = xpos;
		m_lastMousePosition.y = ypos;

		m_camera->rotate(xoffset, yoffset);
	}
}

void Viewer::mouse_button_callback_impl(GLFWwindow* window, int button, int action, int mods)
{
	m_screen->mouseButtonCallbackEvent(button, action, mods);
	//if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	//{
	//	m_mouseIsClicked = true;
	//	m_firstClick = true;
	//}
	//if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	//	m_mouseIsClicked = false; 
}

void Viewer::scroll_callback_impl(GLFWwindow* window, double xoffset, double yoffset)
{
	m_camera->zoom(yoffset);
}

void Viewer::cursor_pos_callback_impl(GLFWwindow* window, double x, double y) {
	m_screen->cursorPosCallbackEvent(x, y);
}

void Viewer::char_callback_impl(GLFWwindow* window, unsigned int codepoint) {
	m_screen->charCallbackEvent(codepoint);
}

void Viewer::drop_callback_impl(GLFWwindow* window, int count, const char **filenames) {
	m_screen->dropCallbackEvent(count, filenames);
}

void Viewer::frame_buffer_impl(GLFWwindow* window, int width, int height) {
	m_screen->resizeCallbackEvent(width, height);
}


// ---------------------------- //
// -- Initialization methods -- //
// ---------------------------- //

void Viewer::createWindow(const nori::Camera& camera)
{
	nori::Vector2i outputSize = nori::Vector2i(m_width, m_height); 

	/* Create a block generator (i.e. a work scheduler) */
	m_blockGenerator = new nori::BlockGenerator(outputSize, NORI_BLOCK_SIZE);

	/* Allocate memory for the entire output image and clear it */
	m_image = new nori::ImageBlock(outputSize, camera.getReconstructionFilter());
	m_image->clear();

	/* Create a window that visualizes the partially rendered result */
	nanogui::init();
	m_screen = new nori::NoriScreen(*m_image);
}

void Viewer::setCallbacks()
{
	glfwSetErrorCallback(error_callback);
	//glfwSetKeyCallback(m_screen->glfwWindow(), key_callback);
	//glfwSetWindowSizeCallback(m_screen->glfwWindow(), window_size_callback);
	//glfwSetCursorPosCallback(m_screen->glfwWindow(), mouse_callback);
	glfwSetMouseButtonCallback(m_screen->glfwWindow(), mouse_button_callback);
	//glfwSetCharCallback(m_screen->glfwWindow(), char_callback);
	//glfwSetDropCallback(m_screen->glfwWindow(), drop_callback);
	//glfwSetScrollCallback(m_screen->glfwWindow(), scroll_callback); 
	//glfwSetFramebufferSizeCallback(m_screen->glfwWindow(), frame_buffer_size_callback);
}

void Viewer::setupViewport()
{
	//// Create Viewport
	//glViewport(0, 0, m_width, m_height);

	//// Set GL options
	//glEnable(GL_DEPTH_TEST);
}

void Viewer::initialize(nori::Scene* scene)
{
	m_scene = scene;
	m_scene->getIntegrator()->preprocess(m_scene);
	setCallbacks();
}

void Viewer::setCamera(const nori::Transform& transform, const nori::PerspectiveCamera& camera) {
	assert(!m_camera); 
	if (!m_camera)
	{
		auto size = camera.getOutputSize();
		m_width = size(0);
		m_height = size(1); 

		auto matrix = transform.getMatrix();
		glm::mat4 initMat;

		for (int y = 0; y < 4; ++y) {
			for (int x = 0; x < 4; ++x)
				initMat[x][y] = matrix(x,y); 
		}

		m_camera = new viewer::Camera(initMat, camera.getFOV(), float(m_width) / float(m_height));
	
		createWindow(camera); 
	}
}

void Viewer::destroy()
{
	delete m_blockGenerator;
	delete m_camera; 
}

// --------------------- //
// -- Runtime methods -- //
// --------------------- //
void Viewer::interaction()
{
	// Move Camera
	if (m_keys[GLFW_KEY_W])
		m_camera->move(Camera::CameraDirection::eForward, m_deltaTime);
	if (m_keys[GLFW_KEY_S])
		m_camera->move(Camera::CameraDirection::eBackward, m_deltaTime);
	if (m_keys[GLFW_KEY_A])
		m_camera->move(Camera::CameraDirection::eLeft, m_deltaTime);
	if (m_keys[GLFW_KEY_D])
		m_camera->move(Camera::CameraDirection::eRight, m_deltaTime);
	if (m_keys[GLFW_KEY_Z])
		m_camera->zoom(0.001);
	if (m_keys[GLFW_KEY_X])
		m_camera->zoom(-0.001);
}

Viewer& Viewer::getInstance()
{
	static Viewer instance;
	return instance; 
}

/*
* @brief : Acts as the main in a non-object oriented OpenGL program.  
*/
Viewer::Viewer()
	: m_lastMousePosition(0, 0)
	, m_firstClick(true)
	, m_scene(nullptr)
	, m_camera(nullptr)
	, m_wireFrameEnabled(false)
	, m_mouseIsClicked(false)
	, m_isRuntime(false)
{
	// GLFW initialization
	if (!glfwInit())
	{
		throw std::runtime_error("glfwInit failed");
		exit(EXIT_FAILURE);
	}

	//// GLEW initialization
	//glewExperimental = GL_TRUE;
	//if (glewInit() != GLEW_OK)
	//{
	//	throw std::runtime_error("glewInit failed");
	//	exit(EXIT_FAILURE);
	//}

	glfwSwapInterval(1);

}

Viewer::~Viewer()
{
	delete m_screen;
	nanogui::shutdown();

	exit(EXIT_SUCCESS);
}

void Viewer::render(std::string filename) {
	if (m_isRuntime)
		renderOnline();
	else
		renderOffline(filename);
}

void Viewer::renderOfflineBlock(nori::ImageBlock& block, nori::Sampler* sampler) {
	const nori::Camera *camera = m_scene->getCamera();
	const nori::Integrator *integrator = m_scene->getIntegrator();

	nori::Point2i offset = block.getOffset();
	nori::Vector2i size = block.getSize();

	/* Clear the block contents */
	block.clear();

	/* For each pixel and pixel sample sample */
	for (int y = 0; y < size.y(); ++y) {
		for (int x = 0; x < size.x(); ++x) {
			for (uint32_t i = 0; i < sampler->getSampleCount(); ++i) {
				nori::Point2f pixelSample = nori::Point2f((float)(x + offset.x()), (float)(y + offset.y())) + sampler->next2D();
				nori::Point2f apertureSample = sampler->next2D();

				/* Sample a ray from the camera */
				nori::Ray3f ray;
				nori::Color3f value = camera->sampleRay(ray, pixelSample, apertureSample);

				/* Compute the incident radiance */
				value *= integrator->Li(m_scene, sampler, ray);

				/* Store in the image block */
				block.put(pixelSample, value);
			}
		}
	}
}

void Viewer::renderOffline(std::string filename) {
	const nori::Camera *camera = m_scene->getCamera();

	/* Do the following in parallel and asynchronously */
	std::thread render_thread([&] {
		std::cout << "Rendering .. ";
		std::cout.flush();
		nori::Timer timer;

		tbb::blocked_range<int> range(0, m_blockGenerator->getBlockCount());

		auto map = [&](const tbb::blocked_range<int> &range) {
			/* Allocate memory for a small image block to be rendered
			by the current thread */
			nori::ImageBlock block(nori::Vector2i(NORI_BLOCK_SIZE),
				camera->getReconstructionFilter());

			/* Create a clone of the sampler for the current thread */
			std::unique_ptr<nori::Sampler> sampler(m_scene->getSampler()->clone());

			for (int i = range.begin(); i<range.end(); ++i) {
				/* Request an image block from the block generator */
				m_blockGenerator->next(block);

				/* Inform the sampler about the block to be rendered */
				sampler->prepare(block);

				/* Render all contained pixels */
				renderOfflineBlock(block, sampler.get());

				/* The image block has been processed. Now add it to
				the "big" block that represents the entire image */
				m_image->put(block);
			}
		};

		/// Uncomment the following line for single threaded rendering
		// map(range);

		/// Default: parallel rendering
		tbb::parallel_for(range, map);

		std::cout << "done. (took " << timer.elapsedString() << ")" << std::endl;
	});

	/* Enter the application main loop */
	nanogui::mainloop();

	/* Shut down the user interface */
	render_thread.join();

	/* Now turn the rendered image block into
	a properly normalized bitmap */
	std::unique_ptr<nori::Bitmap> bitmap(m_image->toBitmap());

	/* Determine the filename of the output bitmap */
	std::string outputName = filename;
	size_t lastdot = outputName.find_last_of(".");
	if (lastdot != std::string::npos)
		outputName.erase(lastdot, std::string::npos);
	outputName += ".exr";

	/* Save using the OpenEXR format */
	bitmap->save(outputName);
}

void Viewer::renderOnline() {
	m_mainLoopActive = true; 
	
	while (m_mainLoopActive) {
			if (!m_screen->visible()) {
				continue;
			}
			else if (glfwWindowShouldClose(m_screen->glfwWindow())) {
				m_screen->setVisible(false);
				continue;
			}

			m_screen->clear();
			m_screen->drawWidgets();
			glfwSwapBuffers(m_screen->glfwWindow());

			/* Wait for mouse/keyboard or empty refresh events */
			glfwWaitEvents();
		}

	/* Process events once more */
	glfwPollEvents();
}

void Viewer::updateFrame()
{
		//GLfloat currentFrameTime = glfwGetTime();
		//m_deltaTime = currentFrameTime - m_lastFrameTime;
		//m_lastFrameTime = currentFrameTime;

		//// Check and call events
		//glfwPollEvents();
		//interaction();

		//// Clear the colorbuffer
		//glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//// Coordinate system matrices 
		//glm::mat4 view = m_camera->GetViewMatrix();
		//glm::mat4 projection = glm::perspective(m_camera->getFovy(), m_width / m_height, 0.1f, 100.0f); 

		////m_scene->setViewMatrix(view);
		////m_scene->setProjectionMatrix(projection);

		////draw(); 

		//// Swap the buffers
		//glfwSwapBuffers(m_window);
		m_screen->drawAll(); 
}

VIEWER_NAMESPACE_END