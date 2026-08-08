#include "glfwContext.hpp"
#include "assetManager/assetManager.hpp"
#include <assetManager/models/mesh.hpp>
#include "glm/matrix.hpp"
#include "assetManager/models/basicModel.hpp"
#include "worldMap/Block.hpp"
#include "worldMap/scene.hpp"
#include "src/camera.hpp"
#include "src/Raycast.hpp"

class Cube : public ISceneObject {
private:
    basicModel& m_model;
public:
    Cube(basicModel& model) : m_model(model){ }
    void update() override {
        m_model.update();
    }
    void draw(float alpha) {
        m_model.tryDraw(alpha);
    }
};

int main() {
    glfwContext::init();

    mainAssetManager::add<shader>("blockShader", "shaders/block.glsl", "shaders/block_frag.glsl");
    mainAssetManager::add<texture>("bricks", "assets/textures/brick.png");
    mainAssetManager::add<texture>("bricks2", "assets/textures/testbrick2.png");
    mainAssetManager::add<mesh>("block", "assets/models/block.obj");

    basicModel block("bricks", "block", "blockShader");
    basicModel block2("bricks2", "block", "blockShader");


	BlockTable::add(block, BlockMaterial::Stone, BlockStats{ 100.0f, 50.0f });
	BlockTable::add(block2, BlockMaterial::Stone, BlockStats{ 100.0f, 50.0f });

    world test("test", 0, worldRules{});
    glfwSetWindowUserPointer(glfwContext::getWindow(), &test);

	test.changeDimension(worldDimension::Overworld);
        
    glfwSetMouseButtonCallback(glfwContext::getWindow(), [](GLFWwindow* window, int button, int action, int mods) {
        auto* testptr = static_cast<world*>(glfwGetWindowUserPointer(window));
        if (!testptr) return;
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            RaycastHit hit = raycastVoxels(testptr->getCurrentDimension(), Camera::getPos(), Camera::getFront(), 5.0f);
            if (hit.type == RaycastHit::HitType::Block) {
                testptr->getCurrentDimension()->setBlock(hit.blockPos.x, hit.blockPos.y, hit.blockPos.z, 0);
            }
        }
        else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
            RaycastHit hit = raycastVoxels(testptr->getCurrentDimension(), Camera::getPos(), Camera::getFront(), 5.0f);
            if (hit.type == RaycastHit::HitType::Block) {
                testptr->getCurrentDimension()->setBlock(hit.adjacentBlockPos.x, hit.adjacentBlockPos.y, hit.adjacentBlockPos.z, 1);
            }
        }});
	glfwContext::addDrawTarget(&test);
	glfwContext::addCycleEvent([&test]() {
		test.update();
        test.loadChunksFromPos(Camera::getPos(), 8); 
		}, false);
    //glfwContext::addDrawTarget(&stone);

    glfwContext::mainGameCycle();

    return 0;
}