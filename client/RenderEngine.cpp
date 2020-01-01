//---------------------------------------------------------------
//
// RenderEngine.cpp
//

#include "RenderEngine.h"

#include "client/Game.h"
#include "client/DebugController.h"
#include "client/RenderEngineEvents.h"
#include "client/View.h"
#include "common/log.h"

// Must be included first, otherwise would be alphabetical.
#include <GL/glew.h>

#include <algorithm>
#include <glm/vec4.hpp>
#include <imgui/imgui.h>
#include <SOIL.h>

#include <fstream>
#include <sstream>
#include <filesystem>

namespace Client {

//===============================================================================

namespace {
// Logger for the render engine.
std::shared_ptr<spdlog::logger> s_logger;

auto s_startTime = std::chrono::steady_clock::now();

glm::vec4 s_clearColor = { 0.45f, 0.55f, 0.60f, 1.00f };
//glm::vec4 s_clearColor = { 0.0f, 0.0f, 0.0f, 1.00f };

namespace fs = std::filesystem;

std::string LoadShader(const std::string& name)
{
	// Places to look for shaders...
	fs::path shaderDir = "shaders";

	// Check if its here.
	fs::path filePath = fs::absolute(shaderDir / fs::path(name));

	if (!fs::exists(filePath))
	{
		return std::string();
	}

	auto fileSize = fs::file_size(filePath);
	std::ifstream in(filePath, std::ios::in | std::ifstream::binary);
	if (!in.is_open())
	{
		return std::string();
	}

	std::string shaderStr;
	shaderStr.resize(fileSize);
	in.read(shaderStr.data(), fileSize);
	in.close();
	return shaderStr;
}

// TODO:
// Figure this out glDebugMessageCallback

} // anon namespace

//-------------------------------------------------------------------------------

SDL_GLContext RenderEngine::GetGLContext()
{
	return m_glContext;
}

RenderEngine::RenderEngine()
{
	REGISTER_LOGGER("RenderEngine");
	s_logger = Log::Logger("RenderEngine");
}

RenderEngine::~RenderEngine()
{
	m_views.clear();
}

bool RenderEngine::Initialize()
{
	// Init image engine.
	int imgFlags = (IMG_INIT_PNG | IMG_INIT_JPG);
	if (!(IMG_Init(imgFlags) & imgFlags))
	{
		SPDLOG_LOGGER_ERROR(s_logger, "Failed to initialize PNG loader. error= {}", IMG_GetError());
		return false;
	}

	// Init font engine.
	if (TTF_Init() == -1)
	{
		SPDLOG_LOGGER_ERROR(s_logger, "Failed to initialize font loader. error={}", TTF_GetError());
		return false;
	}

	SPDLOG_LOGGER_INFO(s_logger, "SDL systems initialized successfully");

	SubscribeEvents();

	return true;
}

void RenderEngine::Render() const
{
	if (!m_glContext)
	{
		return;
	}

#ifdef DEBUG_BUILD
	// Log framerate
	//using namespace std::chrono;
	//static auto currentTime = time_point_cast<seconds>(steady_clock::now());
	//static int frameCount = 0;
	//static int frameRate = 0;
	//auto previousTime = currentTime;
	//currentTime = time_point_cast<seconds>(steady_clock::now());
	//++frameCount;
	//if (currentTime != previousTime)
	//{
	//	frameRate = frameCount;
	//	frameCount = 0;
	//}

	//SPDLOG_LOGGER_DEBUG(s_logger, "Frame rate:{}", frameRate);
#endif


	//glViewport(0, 0, 1280, 720);
	glClearColor(s_clearColor.x, s_clearColor.y, s_clearColor.z, s_clearColor.w);
	glClear(GL_COLOR_BUFFER_BIT);

	RenderTest();
	// TODO:
	// Render Debug UI
	// - Right now, this is a hack.
	g_game->GetDebugController()->Render();


	// Render UI
	// - Figure out what this means.

	// Render game objects
	//for (auto r : m_views)
	//{
	//	if (!r)
	//	{
	//		SPDLOG_LOGGER_WARN(s_logger, "Warning: Attempting to render non-existent element.");
	//		continue;
	//	}

	//	if (r->IsVisible())
	//	{
	//		r->Render();
	//	}
	//}

	SDL_GL_SwapWindow(m_window);
}

void RenderEngine::RegisterView(View* view)
{
	m_views.insert(std::upper_bound(std::cbegin(m_views),
		std::cend(m_views), view,
		[](const View* lhs, const View* rhs)
	{
		return lhs->GetZOrder() > rhs->GetZOrder();
	}), view);
}

void RenderEngine::UnregisterView(uint32_t id)
{
	m_views.erase(std::remove_if(std::begin(m_views), std::end(m_views),
		[id](View* View)
	{
		return id == View->GetId();
	}));
}

void RenderEngine::SubscribeEvents()
{
	auto& events = g_game->GetRenderEngineEvents();
	events.GetViewAboutToBeDestroyedEvent().subscribe([this](uint32_t id)
	{
		UnregisterView(id);
	});

	events.GetViewCreatedEvent().subscribe([this](View* view)
	{
		RegisterView(view);
	});

	events.GetMainWindowSizeChangedEvent().subscribe([this]()
	{
		RepaintWindow();
	});

	events.GetMainWindowExposedEvent().subscribe([this]()
	{
		RepaintWindow();
	});

	events.GetMainWindowCreatedEvent().subscribe([this](SDL_Window* window)
	{
		m_window = window;
		m_glContext = SDL_GL_CreateContext(window);
		InitTextureTest();
		InitRenderTest();
	});

	events.GetMainWindowAboutToBeDestroyedEvent().subscribe([this](SDL_Window* window)
	{
		SDL_GL_DeleteContext(m_glContext);
		m_glContext = nullptr;
		m_window = nullptr;
	});
}

void RenderEngine::RepaintWindow()
{
	SDL_GL_SwapWindow(m_window);
}

void RenderEngine::RenderTest() const
{
	//---------------------------------------------------------------------------
	// The below code demonstrates how to change a uniform from a shader

	// snags the id of a uniform called triangleColor;
	//GLint unicolor = glGetUniformLocation(m_shaderProgramId, "triangleColor");

	//// Tells OpenGL to set the uniform at that id to the specified value.
	//glUniform3f(unicolor, 1.0f, 0.0f, 0.0f);

	//auto nowTime = std::chrono::steady_clock::now();
	//float time = std::chrono::duration_cast<std::chrono::duration<float>>(nowTime - s_startTime).count();
	//glUniform3f(unicolor, (sin(time * 4.0f) + 1.0f) / 2.0f, 0.0f, 0.0f);

	//---------------------------------------------------------------------------


	// type of primitive
	// how many vertices to skip at the beginning
	// how many vertices
	//glDrawArrays(GL_TRIANGLES, 0, 3);

	// draw elements from the element buffer object.
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

}

void RenderEngine::InitRenderTest()
{
	// You can bind all of these settings to a vertex array object (VAO). Anytime you call glVertexAttribPointer,
	// those settings will be stored in that VAO. Switching between different vertex data then is as easy
	// as binding a different VAO.
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// instead of this
	// Vertex array
	// output and input must have same name or they won't link.
	// ex. in Color out Color.
	// position: x, y
	// color: r, g, b
	//m_vertices = {
	//		-0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // Top-left
	//		0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // Top-right
	//		0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // Bottom-right

	//		0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // Bottom-right
	//		-0.5f, -0.5f, 1.0f, 1.0f, 1.0f, // Bottom-left
	//		-0.5f,  0.5f, 1.0f, 0.0f, 0.0f  // Top-left
	//};
	//// Vertex buffer object. (VBO)
	//GLuint vbo = 0;
	//glGenBuffers(1, &vbo);
	//glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices.data()) * m_vertices.size(), m_vertices.data(), GL_STATIC_DRAW);

	//----------------------------------------------------------------------
	// element buffer

	// glDrawArrays(GL_TRIANGLES, 0, 6);

	// use element buffer.

	m_vertices = {
	//  position     color              Texture coords
		-0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,// Top Left
		 0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,// Top Right
		 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,// Bottom Right
		-0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f// Bottom Left
	};

	GLuint vbo = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices.data()) * m_vertices.size(), m_vertices.data(), GL_STATIC_DRAW);

	// We know we're rendering triangles, so specify which vertices connect to triangles.
	GLuint elements[] = {
		0, 1, 2,
		2, 3, 0
	};

	// Create a vertex buffer and store its ID for use with our ebo (element buffer object).
	GLuint ebo;
	glGenBuffers(1, &ebo);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		sizeof(elements), elements, GL_STATIC_DRAW);

	//-----------------------------------------------------------------------
	// Init basic vertex shader.
	m_basicVertexShader = LoadShader("test-shader.vert");

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	const char* vShaderCStr[] = { m_basicVertexShader.c_str() };
	glShaderSource(vertexShader, 1, vShaderCStr, nullptr);
	glCompileShader(vertexShader);

	ValidateShader(vertexShader);

	//-----------------------------------------------------------------------
	// Init basic fragment shader.
	m_basicFragmentShader = LoadShader("test-shader.frag");

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	const char* fShaderCStr[] = { m_basicFragmentShader.c_str() };
	glShaderSource(fragmentShader, 1, fShaderCStr, nullptr);
	glCompileShader(fragmentShader);

	ValidateShader(fragmentShader);

	// Need to bind the shaders to a single program so they work together.
	// Pipeline:
	// vertex shader -> rasterizer -> fragment shader
	m_shaderProgramId = glCreateProgram();
	glAttachShader(m_shaderProgramId, vertexShader);
	glAttachShader(m_shaderProgramId, fragmentShader);

	// fragment shader is allowed to write to multiple buffers. Specify which one.
	// use glDrawBuffers when rendering to multiple buffers, otherwise only first
	// output will be enabled by default.
	glBindFragDataLocation(m_shaderProgramId, 0, "outColor");

	// link the program
	glLinkProgram(m_shaderProgramId);

	// Only one program can be active at a time, just like vertex buffers VBO).
	glUseProgram(m_shaderProgramId);

	//------------------------------------------------------------------------------
	// Need to link the attributes from the vertex shader source, because OpenGL has
	// no idea what that actually is.

	// pos is a number depending on the order of input definitions. So this should be 0.
	//GLint posAttribute = glGetAttribLocation(m_shaderProgramId, "position");

	// Specify how the input should be retrieved.
	// input reference
	// number of values for the input - its a vec2 so there's 2
	// type of each component
	// whether to normalize the values (-1.0 - 1.0)
	// stride: how many bytes are between each position attribute in the array. This is currently 0
	// offset: How many bytes before the start of the array, also currently 0.
	// glVertexAttribPointer(posAttribute, 2, GL_FLOAT, GL_FALSE, 0, 0);

	// Now we have another attribute to accommodate for.
	// We need to specify how each attribute should be retrieved.
	GLint posAttribId = glGetAttribLocation(m_shaderProgramId, "position");
	glEnableVertexAttribArray(posAttribId);
	glVertexAttribPointer(posAttribId, 2, GL_FLOAT, GL_FALSE,
		7 * sizeof(float), 0);

	GLint colorAttribId = glGetAttribLocation(m_shaderProgramId, "color");
	glEnableVertexAttribArray(colorAttribId);
	glVertexAttribPointer(
		// The attribute ID of the attribute in the vertex.
		colorAttribId,
		// Number of values the attribute contains (ex. vec3 has 3 values).
		3,
		// Type of each value.
		GL_FLOAT,
		// Whether to normalize or not.
		GL_FALSE,
		// Offset of each vertex.
		7 * sizeof(float),
		// Offset of the attribute within the vertex.
		(void*)(2 * sizeof(float)));

	// IMPORTAT
	// The above function stores the stride, offset, and the VBO that is currently bound to GL_ARRAY_BUFFER
	// This means that
	// - You don't have to explicitly bind the correct VBO for drawing
	// - You can use a different VBO for each attribute.



	//------------------------------------------------------------------------------------
	// Change the color of the triangle by modifying the shader attribute.

	//// snags the id of a uniform called triangleColor;
	//GLint unicolor = glGetUniformLocation(m_shaderProgramId, "triangleColor");

	//// Tells OpenGL to set the uniform at that id to the specified value.
	//glUniform3f(unicolor, 1.0f, 0.0f, 0.0f);

	//-------------------------------------------------------------------------------------

	GLint texAttribId = glGetAttribLocation(m_shaderProgramId, "texcoord");
	glEnableVertexAttribArray(texAttribId);
	glVertexAttribPointer(texAttribId, 2, GL_FLOAT, GL_FALSE,
		7 * sizeof(float), (void*)(5 * sizeof(float)));
}

void RenderEngine::ValidateShader(unsigned int shader)
{
	GLint status = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE)
	{
		const int size = 512;
		std::string errMessage;
		errMessage.resize(size);
		glGetShaderInfoLog(shader, size, nullptr, errMessage.data());

		SPDLOG_LOGGER_ERROR(s_logger, "Error compiling shader error={} message={}", status, errMessage);
	}
}

void RenderEngine::InitTextureTest()
{
	GLuint textureId;

	// Hey OpenGL create a 2D array of pixels and give me the ID for it.
	glGenTextures(1, &textureId);

	// Bind the 2D aray of pixels.
	glBindTexture(GL_TEXTURE_2D, textureId);

	int width = 0;
	int height = 0;
	unsigned char* image = SOIL_load_image("resources/test-textures/pika.png",
		&width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	// (0.0, 0.0), is bottom left corner (1.0, 1.0) is upper right corner.
	// "Sampling" is the operation that uses texture coordinates to retrieve color info from pixels.
	// How will we handle when a coord outside of 0 to 1 is given?

	// OpenGL gives us 4 ways
	// GL_REPEAT, GL_MIRRORED_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER

	// Pos coords are x, y, z texture coords are s, t, r.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// set the border color to red.
	//float color[] = { 1.0f, 0.0f, 0.0f, 1.0f };
	//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);

	// Black and white checkers
	//float pixels[] = {
	//	0.0f, 0.0f, 0.0f,	1.0f, 1.0f, 1.0f,
	//	1.0f, 1.0f, 1.0f,	0.0f, 0.0f, 0.0f
	//};

	//glTexImage2D(
	//	// Target type
	//	GL_TEXTURE_2D,
	//	// Level of detail where 0 is base image. You can use this to load mipmaps.
	//	0,
	//	// Internal pixel format that indicates how they're stored on the GPU
	//	GL_RGB,
	//	// Width
	//	2,
	//	// Height
	//	2,
	//	// Border, must always be 0.
	//	0,
	//	// Format of the pixel data.
	//	GL_RGB,
	//	// Pixel data's data type.
	//	GL_FLOAT,
	//	// Actual pixel data.
	//	pixels);
}

// Helpers
void RenderEngine::UpdateDrawOrder()
{
	// Stable insertion sort. Should profile this, but I expect this to be quick
	// since our list of Views should always be mostly sorted.
	for (auto it = std::begin(m_views); it != std::end(m_views); ++it)
	{

		auto insertIndex = std::upper_bound(std::begin(m_views), it,
			*it,
			[](const View* lhs, const View* rhs)
		{
			if (lhs->GetZOrder() == rhs->GetZOrder())
			{
				return false;
			}
			return lhs->GetZOrder() > rhs->GetZOrder();
		});

#ifdef DEBUG_BUILD
		auto reorderDistance = std::distance(insertIndex, it);

		// We want to know if we're consistently horribly out of order. If so, maybe we don't want
		// an insertion sort.
		if (reorderDistance > 5)
		{
			SPDLOG_LOGGER_TRACE(s_logger, "Zordering large distance. distance=", reorderDistance);
		}
#endif // DEBUG_BUILD

		std::rotate(insertIndex, it, it + 1);
	}
}

//===============================================================================

} // namespace Client
