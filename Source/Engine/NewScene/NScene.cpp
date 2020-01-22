#include"NScene.h"
#include"../Render/SceneRender.h"
#include<random>
#include<time.h>

using std::default_random_engine;
using std::uniform_int_distribution;

NScene::NScene()
{
    sceneRender = new SceneRender(this);
}

NScene::~NScene()
{
    if (sceneRender) delete sceneRender;
}

void NScene::SetCamera(shared_ptr<NCameraNode> _camera) 
{
    camera = _camera; 
}

const shared_ptr<NCameraNode> NScene::GetCamera() const
{ 
    return camera; 
}

bool NScene::AddSceneNode(ActorId id, shared_ptr<NSceneNode> node)
{
    if (actorMap.find(id) != actorMap.end()) return false;
    actorMap.insert(std::make_pair(id,node));
    allSceneNodes.push_back(node);
    return true;
}

bool NScene::AddSceneNode(shared_ptr<NSceneNode> node)
{
    ActorId actorId = node->actorId;
    if (actorId == INVAILD_ACOTR_ID) {
        node->actorId = GetRandomNewId();
    }
    return AddSceneNode(node->actorId, node);
}

HRESULT NScene::OnRender()
{
    return S_OK;
}

int NScene::GetRandomNewId()
{
    default_random_engine e;
    e.seed(time(0));
    uniform_int_distribution<unsigned> u(0, INT_MAX);

    int val = 0;
    do {
        val = u(e);
    } while (actorMap.find(val) != actorMap.end());

    return val;
}
