#pragma once
#include <vector>

#include "../shader/shader.hpp"
#include "../Interfaces/Idrawable.hpp"

class renderer {
private:
	static inline std::vector<Idrawable*> m_drawableObjects;

	static inline FrameBuffer* fbo[2] = { nullptr, nullptr };
	static inline FrameBuffer* mainFBO = nullptr;
	static inline postProcessorBuffer* ppbuf = nullptr;
	static inline GLuint currentShaderID{};
	static inline glm::mat4 projection = glm::perspective(glm::radians(80.0f), 16.0f / 9.0f, 0.1f, 100.0f);
public:
	~renderer();

	static void init();

	static void addDrawTarget(Idrawable* object);
	static void deleteAllRenderTargets();
	static void useShader(shader& _shader);
	static void drawAll(float alpha);

	static glm::mat4 getProjection() {
		return projection;
	}

	static void setProjection(glm::mat4 proj) {
		projection = proj;
	}

	static GLuint getCurrentShaderID() {
		return currentShaderID;
	}

	static void addPostProcessorShader(shader* sh) {
		ppbuf->addShader(sh);
	}

	static postProcessorBuffer* getPostProcessorBuffer() {
		return ppbuf;
	}
};