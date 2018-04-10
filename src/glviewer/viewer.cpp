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
#include <glm/gtc/type_ptr.hpp>

#include <glviewer/viewer.h>
#include <glviewer/camera.h>

#include <nori/bsdfs/bsdf.h>
#include <nori/bsdfs/phong.h>
#include <nori/bsdfs/diffuse.h>
#include <nori/cameras/camera.h>
#include <nori/core/bitmap.h>
#include <nori/core/block.h>
#include <nori/core/common.h>
#include <nori/core/gui.h>
#include <nori/core/scene.h>
#include <nori/shapes/shape.h>
#include <nori/shapes/mesh.h>
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

	// GLEW initialization
	glewExperimental = GL_TRUE;
	auto glewE = glewInit();
	if (glewE != GLEW_OK)
	{
		throw std::runtime_error("glewInit failed");
		exit(EXIT_FAILURE);
	}

	// TODO: fix WARNING: Only supports Meshes for now
	for (auto obj : m_scene->getShapes()) {
		if (obj->isMesh())
			obj->initializeBuffers();
	}
}

void Viewer::setCamera(const nori::Transform& transform, const nori::PerspectiveCamera& camera) {
	assert(!m_camera); 
	if (!m_camera)
	{
		auto size = camera.getOutputSize();
		m_width = size(0);
		m_height = size(1); 

		glm::mat4 initMat = toGLM(transform);

		m_camera = new viewer::Camera(initMat, camera.getFOV(), float(m_width) / float(m_height), camera.getNearClip(), camera.getFarClip());
	
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
	, m_isRuntime(true)
{
	// GLFW initialization
	if (!glfwInit())
	{
		throw std::runtime_error("glfwInit failed");
		exit(EXIT_FAILURE);
	}

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
			updateFrame();
			m_screen->drawWidgets();
			glfwSwapBuffers(m_screen->glfwWindow());

			/* Wait for mouse/keyboard or empty refresh events */
			glfwWaitEvents();
		}

	/* Process events once more */
	glfwPollEvents();
}

GLuint Viewer::getShaderProgram(nori::BSDF::EBSDFType bsdfType)
{
	GLuint sp = -1; 

	switch (bsdfType)
	{
	case nori::BSDF::EBSDFType::EDielectric:
		throw nori::NoriException("bsdf::getShader : no shader program associated with Dielectric BSDF");
		break;
	case nori::BSDF::EBSDFType::EDiffuse: { // TODO: Implement diffuse shader
		static viewer::ShaderProgram phong("../src/glviewer/shaders/Diffuse.vx.glsl", "../src/glviewer/shaders/Diffuse.fg.glsl");
		sp = phong.getId();
		break;
	}
	case nori::BSDF::EBSDFType::EMicrofacet:
		throw nori::NoriException("bsdf::getShader : no shader program associated with Microfacet BSDF");
		break;
	case nori::BSDF::EBSDFType::EMirror:
		throw nori::NoriException("bsdf::getShader : no shader program associated with Mirror BSDF");
		break;
	case nori::BSDF::EBSDFType::EPhong: {
		static viewer::ShaderProgram phong("../src/glviewer/shaders/Phong.vs.glsl", "../src/glviewer/shaders/Phong.fg.glsl");
		sp = phong.getId();
		break;
	}
	case nori::BSDF::EBSDFType::ENone:
	default:
		break;
	}

	return sp; 
}

void Viewer::updateFrame()
{
	//GLfloat currentFrameTime = glfwGetTime();
	//m_deltaTime = currentFrameTime - m_lastFrameTime;
	//m_lastFrameTime = currentFrameTime;

	//// Check and call events
	//glfwPollEvents();
	//interaction();

	m_screen->clear();

	auto objects = m_scene->getShapes(); 
	
	for (auto obj : objects) {

		// Only manage normal meshes for now (no area lights neither analytic shapes)
		if(obj->isEmitter() || !obj->isMesh())
			continue;

		// WARNING : ONLY DIFFUSE IS SUPPORTED FOR NOW
		const nori::BSDF* bsdf = obj->getBSDF();
		assert(bsdf->getBSDFType() == BSDF::EBSDFType::EDiffuse && "ONLY DIFFIUSE IS SUPPORTED FOR NOW");
		const nori::Diffuse* diff = static_cast<const nori::Diffuse*>(bsdf); 

		GLuint shaderID = getShaderProgram(obj->getBSDF()->getBSDFType());
		glUseProgram(shaderID);

		glm::mat4 objToWorld = toGLM(obj->toWorld()); 
		glm::mat4 viewMatrix = m_camera->GetViewMatrix(); 
		glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(viewMatrix * objToWorld)));
		glm::mat4 projection = glm::perspective(m_camera->getFovy(), m_camera->getAspect(), m_camera->getNear(), m_camera->getFar()); 
		nori::Color3f albedo = diff->albedo();

		// Get Uniforms location
		GLuint modelLoc = glGetUniformLocation(shaderID, "model");
		GLuint viewLoc = glGetUniformLocation(shaderID, "view");
		GLuint projectionLoc = glGetUniformLocation(shaderID, "projection");
		GLuint normalMatrixLoc = glGetUniformLocation(shaderID, "normalMatrix");
		GLuint objectColorLoc = glGetUniformLocation(shaderID, "objectColor");
		GLuint emitterRadianceLoc = glGetUniformLocation(shaderID, "emitterRadiance");
		GLuint emitterPositionLoc = glGetUniformLocation(shaderID, "emitterPosition");
		GLuint objectDiffuseLoc = glGetUniformLocation(shaderID, "objectMaterial.diffuseCoefs");
		GLuint objectSpecularLoc = glGetUniformLocation(shaderID, "objectMaterial.specularCoefs");
		GLuint objectShininessLoc = glGetUniformLocation(shaderID, "objectMaterial.shininess");
		
		// Assign Uniforms Values
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(objToWorld));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
		//glUniform3f(lightPositionLoc, lightPosition.x, lightPosition.y, lightPosition.z);
		glUniform3f(objectDiffuseLoc, albedo.x(), albedo.y(), albedo.z());

		glBindVertexArray(obj->getVAO());
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj->getEBO());
		glDrawElements(GL_TRIANGLES, obj->getNIndices(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

	}

	//// Swap the buffers
	m_screen->drawAll();
	glfwSwapBuffers(m_screen->glfwWindow());


		////draw(); 


}

glm::mat4 Viewer::toGLM(const nori::Transform& trans) {
	glm::mat4 matrix;
	for (int y = 0; y < 4; ++y)
		for (int x = 0; x < 4; ++x)
			matrix[x][y] = trans.getMatrix()(x, y); 

	return matrix; 
}

VIEWER_NAMESPACE_END