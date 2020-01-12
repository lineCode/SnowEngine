#include"EngineStd.h"
#include"ShaderManager.h"

ShaderManager* ShaderManager::instance = nullptr;

shared_ptr<MeshShaderInstance> ShaderManager::AddMetrial(shared_ptr<SceneShaderNode> node, const std::string& fileName)
{
	auto iter = shaderMap.find(fileName);
	if (iter != shaderMap.end()) {
		std::shared_ptr<MeshShader> shader = iter->second;
		if (shader->hasDefaultRes) {
			std::shared_ptr<MeshShaderInstance> instance=defaultInstanceMap.find(shader)->second;
			return instance;
		}
		else {
			return std::make_shared<MeshShaderInstance>(shader);
		}
	}
	else {
		ShaderComplier complier(fileName);
		complier.PreProcess();
		complier.Compile();
		shared_ptr<MeshShader> shader = std::make_shared<MeshShader>();;
		void* buff=nullptr;
		shader->hasDefaultRes = complier.HasDefaultRes(&buff);
		shader->shaderState = complier.GetShaderState();
		shader->shaderResourceMeta = complier.GetResourceMeta();
		shared_ptr<MeshShaderInstance> instance=std::make_shared<MeshShaderInstance>(shader);
		instance->buffData = buff;
		instance->textures = complier.GetShaderTextures();
		shaderMap.insert(std::make_pair(fileName, shader));
		instanceMap.insert(std::make_pair(node, instance));

		if (shader->hasDefaultRes) {
			defaultInstanceMap.insert(std::make_pair(shader,instance));
		}

		return instance;
	}
}

shared_ptr<MeshShaderInstance> ShaderManager::AddMetrial(shared_ptr<SceneShaderNode> node, const std::string& fileName, const std::string& configFileName)
{
	auto iter = shaderMap.find(fileName);
	if (iter != shaderMap.end()) {
		std::shared_ptr<MeshShader> shader = iter->second;
		shared_ptr<MeshShaderInstance> instance = std::make_shared<MeshShaderInstance>(shader);
		instance->shader = shader;

		ShaderResourceConfig config(configFileName, shader->shaderResourceMeta);
		instance->buffData = config.GetBuff();
		instance->textures = config.GetTextures();

		return instance;
	}
	else {
		ShaderComplier complier(fileName);
		complier.PreProcess();
		complier.Compile();
		shared_ptr<MeshShader> shader = std::make_shared<MeshShader>();;
		void* buff = nullptr;
		shader->hasDefaultRes = complier.HasDefaultRes(&buff);
		shader->shaderState = complier.GetShaderState();
		shader->shaderResourceMeta = complier.GetResourceMeta();
		shared_ptr<MeshShaderInstance> instance = std::make_shared<MeshShaderInstance>(shader);
		//Get From Config
		ShaderResourceConfig config(configFileName, shader->shaderResourceMeta);
		instance->buffData = config.GetBuff();
		instance->textures = config.GetTextures();

		shaderMap.insert(std::make_pair(fileName, shader));
		instanceMap.insert(std::make_pair(node, instance));

		return instance;
	}
}
