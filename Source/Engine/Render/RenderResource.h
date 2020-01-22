#pragma once

namespace Render {
	enum  RenderType {
		StaticMesh,
		SkeletonMesh,
		SkyBox,
		Particle,
		Terrain
	};

	enum  RenderPass {
		BackGround = 1000,
		Opaque = 2000,
		AlphaTest = 2450,
		Overlay = 4000,
		GeometryLast = 9000
	};

	enum AlphaType
	{
		AlphaOpaque,
		AlphaTexture,
		AlphaMaterial,
		AlphaVertex
	};
}

extern const int INVAILD_ACOTR_ID = 0x7fffffff;