#pragma once

#include <GL/glew.h>
#include <nori/core/vector.h>
#include <nori/core/common.h>
#include <glviewer/camera.h>
#include <nori/core/transform.h>
#include <nori/cameras/perspective.h>
#include <nori/bsdfs/bsdf.h>

#include <Eigen/Geometry>

VIEWER_NAMESPACE_BEGIN

/**
* \brief Viewer
*
* This class is the root of Sparkles' Real-time OpenGL viewer,
* allowing to pre-visualize a scene before offline rendering 
* along with allowing to modify the scene slightly (camera's 
* viewpoint, objects' positions, etc.). 
* It can also be used to test various Real-time techniques and
* compare them to ground truth (offline techniques). 
*/

class Viewer {
public : 
	// Callbacks implementations
	void error_callback_impl(int error, const char* description);
	void key_callback_impl(GLFWwindow* window, int key, int scancode, int action, int mods);
	void window_size_callback_impl(GLFWwindow* window, int width, int height);
	void mouse_callback_impl(GLFWwindow* window, double xpos, double ypos);
	void mouse_button_callback_impl(GLFWwindow* window, int button, int action, int mods);
	void scroll_callback_impl(GLFWwindow* window, double xoffset, double yoffset); 
	void cursor_pos_callback_impl(GLFWwindow* window, double x, double y);
	void char_callback_impl(GLFWwindow* window, unsigned int codepoint);
	void drop_callback_impl(GLFWwindow* window, int count, const char **filenames);
	void frame_buffer_impl(GLFWwindow* window, int width, int height); 

	// Getters methods
	static Viewer& getInstance();
	bool getInitWithRuntime() { return m_isRuntime; }

	// Initialization methods
	void initialize(nori::Scene* scene);
	void setCamera(const nori::Transform& transform, const nori::PerspectiveCamera& cam);

	// Rendering methods
	void render(std::string filename);

	~Viewer();

private : 
	Viewer();
	Viewer(const Viewer& viewer) = delete; 
	
	//Initialization methods
	void createWindow(const nori::Camera& camera); 
	void setCallbacks();
	void setupViewport(); 
	void destroy();
	
	// Rendering methods
	void updateFrame();
	void renderOffline(std::string filename);
	void renderOfflineBlock(nori::ImageBlock& block, nori::Sampler* sampler);
	void renderOnline();
	GLuint getShaderProgram(nori::BSDF::EBSDFType bsdfType);

	// Runtime methods
	void interaction();
	//void setMatrices();
	//void draw();

	// Utility methods
	glm::mat4 toGLM(const nori::Transform& trans);

	// Display information
	bool m_isRuntime;
	bool m_mainLoopActive;

	// Windows information
	nori::NoriScreen* m_screen; 
	float m_width;
	float m_height;

	// Interaction information
	bool m_keys[1024]; // All the keys that can be pressed
	glm::vec2 m_lastMousePosition;
	bool m_firstClick; 
	bool m_mouseIsClicked;
	float m_timeMouseLastClicked;

	// Time related information
	float m_deltaTime;
	float m_lastFrameTime;

	// Visualisation modes
	bool m_wireFrameEnabled; 
	
	// Scene and render information
	nori::Scene* m_scene; 
	viewer::Camera* m_camera;
	nori::BlockGenerator* m_blockGenerator;
	nori::ImageBlock* m_image; 
};

VIEWER_NAMESPACE_END