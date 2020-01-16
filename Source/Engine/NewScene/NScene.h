#pragma once

#include"NSceneNode.h"
#include<vector>
#include<map>
#include<memory>

using ActorId = int;

class NScene {
protected:
	std::map<ActorId, std::shared_ptr<NSceneNode> > actorMap;
	std::vector<std::shared_ptr<NSceneNode>> allSceneNodes;

public:
};