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
    Player* player = nullptr;
    int currentBlock = 1;
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
    mainAssetManager::add<texture>("dirt", "assets/textures/dirt.png", false);
    mainAssetManager::add<texture>("grassBlock", "assets/textures/grass_block_full.png", false);
    mainAssetManager::add<texture>("skybox", "assets/textures/skybox.png", false);
    //mainAssetManager::add<texture>("test", "assets/textures/test.png", false);
    mainAssetManager::add<mesh>("steve", "assets/models/steve.obj");
    mainAssetManager::add<mesh>("block", "assets/models/block.obj");
    mainAssetManager::add<mesh>("multiTextureBlock", "assets/models/multiTextureBlock.obj");
    mainAssetManager::add<mesh>("skybox", "assets/models/skybox.obj");

    basicModel stone("stone", "block", "blockShader"),
        cobblestone("cobblestone", "block", "blockShader"),
        bedrock("bedrock", "block", "blockShader"),
        dirt("dirt", "block", "blockShader"),
        grassBlock("grassBlock", "multiTextureBlock", "blockShader");
       
	BlockTable::add(stone, BlockMaterial::Stone, BlockStats{ 100.0f, 50.0f });
	BlockTable::add(cobblestone, BlockMaterial::Stone, BlockStats{ 100.0f, 50.0f });
	BlockTable::add(bedrock, BlockMaterial::Stone, BlockStats{ -1.0f, -1.0f });
    BlockTable::add(dirt, BlockMaterial::Grass, BlockStats{ 100.0f, 50.0f });
    BlockTable::add(grassBlock, BlockMaterial::Grass, BlockStats{ 100.0f, 50.0f });

    worldManager::createWorld("test", 0, worldRules{});
    worldManager::loadWorld(0);
    world* currentWorld = worldManager::getCurrentWorld();
    currentWorld->changeDimension(worldDimension::Overworld);

    auto player = std::make_unique<Player>(glm::vec3(0.0f, 16.0f, 0.0f));
    currentWorld->addGlobalObject(player.get());
    GameState gameState{ currentWorld, player.get() };
        
    glfwSetCursorPosCallback(glfwContext::getWindow(), Camera::mouseCallback);

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