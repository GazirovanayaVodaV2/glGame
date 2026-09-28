#include <iostream>
#include <string_view>

#include "gameSettings.hpp"
#include "glfwContext.hpp"
#include "timer.hpp"

#include "camera.hpp"

#include "global.hpp"

#include "assetManager/assetManager.hpp"


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    gameSettings::resolution = { width, height };
    gameSettings::ratio = (float)gameSettings::resolution.first / (float)gameSettings::resolution.second;
    glfwContext::projection = glm::perspective(glm::radians(gameSettings::fov), gameSettings::ratio, 0.1f, 100.0f);
}

void glfwContext::init()
{
    loadSettings();

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        std::exit(-1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGLVERSION_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGLVERSION_MINOR);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    if (gameSettings::fullscreen) {
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        gameSettings::resolution = { mode->width, mode->height };
    }
    else {
        if (gameSettings::resizeableWindow) {
            glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        }
        else {
            glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        }
    }

    m_window = glfwCreateWindow(gameSettings::resolution.first,
        gameSettings::resolution.second, gameSettings::windowName.c_str(),
        NULL, NULL);
    if (!m_window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        std::exit(-1);
    }

    if (gameSettings::fullscreen)
        glfwSetWindowPos(m_window, 0, 0);

    

    glfwMakeContextCurrent(m_window);
    glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);
    glfwSwapInterval(gameSettings::vsync);
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(m_window, Camera::mouseCallback);
    int initRes = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    if (!initRes) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        std::exit(-1);
    }

    GLint major = 0, minor = 0;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    openGLVersion = (major * 10) + minor;
    if (openGLVersion < OPENGLVERSION) {
        std::cerr << "Bro Ur PC SUCKS!!!" << std::endl;
        std::exit(-1);
    }
    //

    glfwContext::projection = glm::perspective(glm::radians(gameSettings::fov), gameSettings::ratio, 0.1f, 1000.0f);


    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    addKeyboardInputEvent(Camera::keyCallback);
    addKeyboardInputEvent([](GLFWwindow* window) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        });

    UniformBuffer<globalUniforms_t>::init();
    UniformBuffer<globalUniforms_blackHolesData_t>::init(1);

    fbo[0] = new FrameBuffer();
    fbo[1] = new FrameBuffer();
    mainFBO = new FrameBuffer();
    ppbuf = new postProcessorBuffer();
}

void glfwContext::updateConfiguration()
{
}

glfwContext::~glfwContext()
{
    delete fbo[0];
    delete fbo[1];
    delete mainFBO;
    delete ppbuf;
    glfwTerminate();
}

void glfwContext::mainGameCycle()
{
    timer logicTimer, renderTimer;
    float accumulator = 0.0f, alpha = 0.0f;

    auto& ppshader = mainAssetManager::get<shader>("basicPostProcess");
    ppbuf->addShader(&ppshader);

    while (!glfwWindowShouldClose(m_window)) {
        const float targetFpsTime = 1000.0f / gameSettings::maxFps;
        
        accumulator += logicTimer.getDeltaTicks();
        glfwPollEvents();



        while (accumulator >= 1.0f) {
            Camera::SaveStateForInterpolation();
            for (auto& event : m_inCycleEvents) {
                event();
            }
            accumulator -= 1.0f;
            for (auto& event : m_inCycleEvents_Undeletable) {
                event();
            }
            for (auto& event : m_keyboardEvents) {
                event(m_window);
            }
            for (auto& event : m_mouseEvents) {
                event(m_window);
            }
        }

        alpha = accumulator;
        Camera::update();
        Camera::updateInterpolatedMatrix(alpha);


        if (gameSettings::vsync || (renderTimer.getDeltaMS(false) >= targetFpsTime)) {
            renderTimer.reset();

            globalUniforms.view = Camera::getView();
            globalUniforms.cameraPos = glm::vec4(Camera::getPos(),1.0f);
            globalUniforms.projection = glfwContext::projection; 
             
            UniformBuffer<globalUniforms_t>::update(globalUniforms);

            glBindFramebuffer(GL_FRAMEBUFFER, mainFBO->getFBO());
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            for (auto& object : m_drawableObjects) {
                object->tryDraw(alpha);
            }

            
            auto size = glfwContext::getScreenSize();
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

            ppbuf->draw(mainFBO->getFrameTextureID(), fbo);

            glfwSwapBuffers(m_window); 
        }
    }
}

void glfwContext::deleteAllRenderTargets()
{
    m_drawableObjects.clear();
}

void glfwContext::deleteAllGameEvents()
{
    m_inCycleEvents.clear();
}

void glfwContext::useShader(shader& _shader)
{
    auto id = _shader.getID();
    if (currentShaderID != id) {
        glUseProgram(id);
        currentShaderID = id;
    }
}

glm::vec2 glfwContext::getScreenSize()
{
    int x = 0, y = 0;
    glfwGetWindowSize(m_window, &x, &y);

    return glm::vec2(static_cast<float>(x), static_cast<float>(y));
}


GLFWwindow* glfwContext::getWindow()
{
    return m_window;
}

void glfwContext::addCycleEvent(voidFunction event, bool canDelete)
{
    if (canDelete) {
        m_inCycleEvents.push_back(event);
    }
    else {
        m_inCycleEvents_Undeletable.push_back(event);
    }
}

void glfwContext::addDrawTarget(Idrawable* object)
{
    m_drawableObjects.push_back(object);
}

void glfwContext::addKeyboardInputEvent(inputEvent event)
{
    m_keyboardEvents.push_back(event);
}

void glfwContext::addMouseInputEvent(inputEvent event)
{
    m_mouseEvents.push_back(event);
}
