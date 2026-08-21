#include "glfwContext.hpp"
#include "assetManager/assetManager.hpp"
#include <assetManager/models/mesh.hpp>
#include "glm/matrix.hpp"
#include "assetManager/models/basicModel.hpp"
#include "worldMap/Block.hpp"
#include "worldMap/scene.hpp"
#include "src/camera.hpp"
#include "src/Raycast.hpp"
#include "src/player/player.hpp"

struct GameState {
    world* currentWorld = nullptr;
    //Player* player = nullptr;
};

int main() {
    glfwContext::init();
    mainAssetManager::add<shader>("debugShader", "shaders/debug.glsl", "shaders/debug_frag.glsl");
    mainAssetManager::add<shader>("blockShader", "shaders/block.glsl", "shaders/block_frag.glsl");
    mainAssetManager::add<shader>("skyboxShader", "shaders/skybox.glsl", "shaders/skybox_frag.glsl");
    mainAssetManager::add<texture>("steve", "assets/textures/steve.png", false);
    mainAssetManager::add<texture>("stone", "assets/textures/stone.png", false);
    mainAssetManager::add<texture>("cobblestone", "assets/textures/cobblestone.png", false);
    mainAssetManager::add<texture>("bedrock", "assets/textures/bedrock.png", false);
    mainAssetManager::add<texture>("skybox", "assets/textures/skybox.png", false);
    mainAssetManager::add<mesh>("steve", "assets/models/steve.obj");
    mainAssetManager::add<mesh>("block", "assets/models/block.obj");
    mainAssetManager::add<mesh>("skybox", "assets/models/skybox.obj");

    basicModel stone("stone", "block", "blockShader"),
        cobblestone("cobblestone", "block", "blockShader"),
        bedrock("bedrock", "block", "blockShader");

	BlockTable::add(stone, BlockMaterial::Stone, BlockStats{ 100.0f, 50.0f });
	BlockTable::add(cobblestone, BlockMaterial::Stone, BlockStats{ 100.0f, 50.0f });
	BlockTable::add(bedrock, BlockMaterial::Stone, BlockStats{ -1.0f, -1.0f });

    worldManager::createWorld("test", 0, worldRules{});
    worldManager::loadWorld(0);
    world* currentWorld = worldManager::getCurrentWorld();
    currentWorld->changeDimension(worldDimension::Overworld);

    auto player = std::make_unique<Player>(glm::vec3(0.0f, 15.0f, 0.0f));
    currentWorld->addGlobalObject(player.get());
    GameState gameState{ currentWorld, /*player.get()*/ };
    glfwSetWindowUserPointer(glfwContext::getWindow(), &gameState);
        
    glfwSetCursorPosCallback(glfwContext::getWindow(), Camera::mouseCallback);
    glfwSetMouseButtonCallback(glfwContext::getWindow(), [](GLFWwindow* window, int button, int action, int mods) {
        auto* state = static_cast<GameState*>(glfwGetWindowUserPointer(window));
        if (!state || !state->currentWorld) return;

        dimensionBase* currentDim = state->currentWorld->getCurrentDimension();
        if (!currentDim) return;

        if (action == GLFW_PRESS) {
            RaycastHit hit = raycastVoxels(currentDim, Camera::getPos(), Camera::getFront(), 5.0f);

            if (hit.type == RaycastHit::HitType::Block) {
                if (button == GLFW_MOUSE_BUTTON_LEFT) {
                    currentDim->setBlock(hit.blockPos.x, hit.blockPos.y, hit.blockPos.z, 0);
                }
                else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
                    currentDim->setBlock(hit.adjacentBlockPos.x, hit.adjacentBlockPos.y, hit.adjacentBlockPos.z, 1);
                }
            }
        }
        });


   /* glfwContext::addCycleEvent([player = player.get()]() {
        world* currentWorld = worldManager::getCurrentWorld();
        if (!currentWorld) return;

        dimensionBase* dimension = currentWorld->getCurrentDimension();
        if (!dimension) return;

        GLFWwindow* window = glfwContext::getWindow();

        player->handleInput(window);
        player->updatePlayer(dimension);
        }, true); */
   glfwContext::mainGameCycle();

    return 0;
}