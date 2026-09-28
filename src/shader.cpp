#include "shader.hpp"
#include "glfwContext.hpp"

static std::string readShader(std::filesystem::path path) {
	std::ifstream code(path);
	if (!code.is_open()) {
		std::cerr << "Failed to open shader! Path: " << path << std::endl;
		return "";
	}

	std::stringstream ss;
	ss << code.rdbuf();

	return ss.str();
}

unsigned int shader::compile(std::string code, shader::types type)
{
	const char* code_cstr = code.c_str();
	unsigned int shader = glCreateShader((int)type);
	glShaderSource(shader, 1, &code_cstr, NULL);
	glCompileShader(shader);
	checkErrors(shader, (shader::types)type);

	return shader;
}

void shader::checkErrors(uint32_t shader, shader::types type)
{
	int success;
	char log[1024];

	if (type != shader::types::program) {
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(shader, 1024, NULL, log);
			std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << (int)type << "\n" << log << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
	else {
		glGetShaderiv(shader, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shader, 1024, NULL, log);
			std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << (int)type << "\n" << log << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
}

shader::shader(std::filesystem::path vertexShader, std::filesystem::path fragmentShader)
{
	std::string vertexCode = readShader(vertexShader);
	std::string fragmentCode = readShader(fragmentShader);
	auto vertex = compile(vertexCode, types::vertex);
	auto fragment = compile(fragmentCode, types::fragment);

	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	checkErrors(ID, types::program);

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

shader::~shader()
{
	if (ID != 0) glDeleteProgram(ID);


}


int shader::getUniformLocation(std::string_view name)
{
	std::string name_str(name);
	if (m_uniformLocationCache.find(name_str) != m_uniformLocationCache.end()) {
		return m_uniformLocationCache[name_str];
	}
	int loc = glGetUniformLocation(ID, name.data());
	m_uniformLocationCache[name_str] = loc;
	if (loc == -1) {
		std::cerr << "Warning, uniform " << name << " not found!" << std::endl;
		return -1;
	}
	return loc;
}

FrameBuffer::FrameBuffer()
{
	auto res = glfwContext::getScreenSize();
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO); 
	glGenTextures(1, &colorTextures);
	glBindTexture(GL_TEXTURE_2D, colorTextures);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, res.x, res.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTextures, 0);

	glGenRenderbuffers(1, &depthRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, depthRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, res.x, res.y);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthRBO);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FrameBuffer::~FrameBuffer()
{
	glDeleteTextures(1, &colorTextures);
	glDeleteRenderbuffers(1, &FBO);
	glDeleteRenderbuffers(1, &depthRBO);
}

void postProcessorBuffer::drawQuad()
{
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

postProcessorBuffer::postProcessorBuffer()
{

	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

postProcessorBuffer::~postProcessorBuffer()
{
	glDeleteVertexArrays(1, &quadVAO);
	glDeleteBuffers(1, &quadVBO);
}

void postProcessorBuffer::draw(unsigned int fboTextureID, FrameBuffer* pingPongFBO[2])
{
	glDisable(GL_DEPTH_TEST);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fboTextureID);

	unsigned int currentInputTexture = fboTextureID;
	bool pingPongIndex = 0;

	for (std::size_t i{}; i < pipeline.size(); i++) {
		bool isFinalPass = (i == pipeline.size() - 1);
		if (isFinalPass) {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
		else {
			glBindFramebuffer(GL_FRAMEBUFFER, pingPongFBO[pingPongIndex]->getFBO());
		}
		auto& sh = pipeline[i];
		glfwContext::useShader(*sh);
		sh->set<int>("screenTexture", 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, currentInputTexture);

		drawQuad();

		if (!isFinalPass) {
			currentInputTexture = pingPongFBO[pingPongIndex]->getFrameTextureID();
			pingPongIndex = !pingPongIndex;
		}
	}

	glEnable(GL_DEPTH_TEST);
}

void postProcessorBuffer::addShader(shader* sh)
{
	pipeline.push_back(sh);
}
