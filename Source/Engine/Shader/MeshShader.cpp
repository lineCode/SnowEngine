#define _CRT_SECURE_NO_WARNINGS
#include"../Msvc/EngineStd.h"
#include"MeshShader.h"

using std::string;

#pragma region Construct
MeshShader::MeshShader()
{
	
}

MeshShader::MeshShader(const char* _fileName,int _shaderModel) :MeshShader()
{
	
}

MeshShader::~MeshShader() 
{
	SAFE_RELEASE(vertexLayout11);
	SAFE_RELEASE(vertexShader);
	SAFE_RELEASE(pixelShader);
}

#pragma endregion

