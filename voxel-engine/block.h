#pragma once
#include "stdafx.h"
#include "util.h"

const std::map<Face, std::vector<Vertex>> baseCube = {
	{ minusZ, std::vector<Vertex>{
	//X, Y, Z, R, G, B, U, V, normX, normY, normZ
		{ -0.5f, -0.5f, -0.5f, 1, 1, 1, 255, 0, 0, 0, -128 },
		{ -0.5f,  0.5f, -0.5f, 1, 1, 1, 255, 255, 0, 0, -128 },
		{ 0.5f,  0.5f, -0.5f, 1, 1, 1, 0, 255, 0, 0, -128 },
		{ 0.5f,  0.5f, -0.5f, 1, 1, 1, 0, 255, 0, 0, -128 },
		{ 0.5f, -0.5f, -0.5f, 1, 1, 1, 0, 0, 0, 0, -128 },
		{ -0.5f, -0.5f, -0.5f, 1, 1, 1, 255, 0, 0, 0, -128 }
		}
	},
	{ plusZ, std::vector<Vertex>{
		{ -0.5f, -0.5f,  0.5f,  1, 1, 1, 0, 0, 0, 0, 127 },
		{ 0.5f, -0.5f,  0.5f,  1, 1, 1, 255, 0, 0, 0, 127 },
		{ 0.5f,  0.5f,  0.5f,  1, 1, 1, 255, 255, 0, 0, 127 },
		{ 0.5f,  0.5f,  0.5f,  1, 1, 1, 255, 255, 0, 0, 127 },
		{ -0.5f,  0.5f,  0.5f,  1, 1, 1, 0, 255, 0, 0, 127 },
		{ -0.5f, -0.5f,  0.5f,  1, 1, 1, 0, 0, 0, 0, 127 }}
	},
	{ minusX, std::vector<Vertex>{
		{ -0.5f, -0.5f, -0.5f, 1, 1, 1, 0, 0, -128, 0, 0 },
		{ -0.5f, -0.5f,  0.5f, 1, 1, 1, 255, 0, -128, 0, 0 },
		{ -0.5f,  0.5f,  0.5f, 1, 1, 1, 255, 255, -128, 0, 0 },
		{ -0.5f,  0.5f,  0.5f, 1, 1, 1, 255, 255, -128, 0, 0 },
		{ -0.5f,  0.5f, -0.5f, 1, 1, 1, 0, 255, -128, 0, 0 },
		{ -0.5f, -0.5f, -0.5f, 1, 1, 1, 0, 0, -128, 0, 0 }
		}
	},
	{ plusX, std::vector<Vertex>{
		{ 0.5f, -0.5f,  0.5f, 1, 1, 1, 0, 0, 127, 0, 0 },
		{ 0.5f, -0.5f, -0.5f, 1, 1, 1, 255, 0, 127, 0, 0 },
		{ 0.5f,  0.5f, -0.5f, 1, 1, 1, 255, 255, 127, 0, 0 },
		{ 0.5f,  0.5f, -0.5f, 1, 1, 1, 255, 255, 127, 0, 0 },
		{ 0.5f,  0.5f,  0.5f, 1, 1, 1, 0, 255, 127, 0, 0 },
		{ 0.5f, -0.5f,  0.5f, 1, 1, 1, 0, 0, 127, 0, 0 }
		
		}
	},
	{ minusY, std::vector<Vertex>{
		{ -0.5f, -0.5f, -0.5f, 1, 1, 1, 0, 0, 0, -128, 0 },
		{ 0.5f,  -0.5f, -0.5f, 1, 1, 1, 255, 0, 0, -128, 0 },
		{ 0.5f,  -0.5f,  0.5f, 1, 1, 1, 255, 255, 0, -128, 0 },
		{ 0.5f,  -0.5f,  0.5f, 1, 1, 1, 255, 255, 0, -128, 0 },
		{ -0.5f, -0.5f,  0.5f, 1, 1, 1, 0, 255, 0, -128, 0 },
		{ -0.5f, -0.5f, -0.5f, 1, 1, 1, 0, 0, 0, -128, 0 }}
	},
	{ plusY, std::vector<Vertex>{
		{ 0.5f,  0.5f,  0.5f, 1, 1, 1, 255, 0, 0, 127, 0 },
		{ 0.5f,  0.5f, -0.5f, 1, 1, 1, 255, 255, 0, 127, 0 },
		{ -0.5f,  0.5f, -0.5f, 1, 1, 1, 0, 255, 0, 127, 0 },
		{ -0.5f,  0.5f, -0.5f, 1, 1, 1, 0, 255, 0, 127, 0 },
		{ -0.5f,  0.5f,  0.5f, 1, 1, 1, 0, 0, 0, 127, 0 },
		{ 0.5f,  0.5f,  0.5f, 1, 1, 1, 255, 0, 0, 127, 0 },

		}
	}
};