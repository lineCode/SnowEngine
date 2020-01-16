#pragma once

namespace Render {

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
