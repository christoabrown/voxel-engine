// FastNoiseSIMD.h
//
// MIT License
//
// Copyright(c) 2017 Jordan Peck
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// The developer's email is jorzixdan.me2@gzixmail.com (for great email, take
// off every 'zix'.)
//

#ifndef FASTNOISE_SIMD_H
#define FASTNOISE_SIMD_H
#include "stdafx.h"
// Comment out lines to not compile for certain instruction sets
#if defined(__arm__) || defined(__aarch64__)
#define FN_ARM
#define FN_COMPILE_NEON
#else

#define FN_COMPILE_SSE2
#define FN_COMPILE_SSE41

// To compile AVX2 set C++ code generation to use /arch:AVX(2) on FastNoiseSIMD_avx2.cpp
#define FN_COMPILE_AVX2
// Note: This does not break support for pre AVX CPUs, AVX code is only run if support is detected

// Using aligned sets of memory for float arrays allows faster storing of SIMD data
// Comment out to allow unaligned float arrays to be used as sets
#define FN_ALIGNED_SETS
#endif

// SSE2/NEON support is guaranteed on 64bit CPUs so no fallback is needed
#if !(defined(_WIN64) || defined(__x86_64__) || defined(__ppc64__) || defined(__aarch64__)) || defined(_DEBUG)
#define FN_COMPILE_NO_SIMD_FALLBACK
#endif

// Using FMA instructions with AVX2/NEON provides a small performance increase but can cause 
// minute variations in noise output compared to other SIMD levels due to higher calculation precision
#ifndef __arm__
#define FN_USE_FMA
#endif

// Reduced minimum of zSize from 8 to 4 when not using a vector set
// Causes slightly performance loss on non-"mulitple of 8" zSize
#define FN_MIN_Z_4

/*
Tested Compilers:
-MSVC v120/v140
-Intel 16.0
-GCC 4.7 Linux
-Clang MacOSX

CPU instruction support:

SSE2
Intel Pentium 4 - 2001
AMD Opteron/Athlon - 2003

SEE4.1
Intel Penryn - 2007
AMD Bulldozer - Q4 2011

AVX
Intel Sandy Bridge - Q1 2011
AMD Bulldozer - Q4 2011

AVX2
Intel Haswell - Q2 2013
AMD Carrizo - Q2 2015

FMA3
Intel Haswell - Q2 2013
AMD Piledriver - 2012
*/

struct FastNoiseVectorSet;

class FastNoiseSIMD
{
public:

	enum NoiseType { Value, ValueFractal, Perlin, PerlinFractal, Simplex, SimplexFractal, WhiteNoise, Cellular };
	enum FractalType { FBM, Billow, RigidMulti };
	enum PerturbType { None, Gradient, GradientFractal };

	enum CellularDistanceFunction { Euclidean, Manhattan, Natural };
	enum CellularReturnType { CellValue, Distance, Distance2, Distance2Add, Distance2Sub, Distance2Mul, Distance2Div };

	// Creates new FastNoiseSIMD for the highest supported instuction set of the CPU 
	static FastNoiseSIMD* NewFastNoiseSIMD(int seed = 1337);

	// Returns highest detected level of CPU support
	// 5: ARM NEON
	// 3: AVX2 & FMA3
	// 2: SSE4.1
	// 1: SSE2
	// 0: Fallback, no SIMD support
	static int GetSIMDLevel(void);

	// Sets the SIMD level for newly created FastNoiseSIMD objects
	// 5: ARM NEON
	// 3: AVX2 & FMA3
	// 2: SSE4.1
	// 1: SSE2
	// 0: Fallback, no SIMD support
	// -1: Auto-detect fastest supported (Default)
	// Caution: Setting this manually can cause crashes on CPUs that do not support that level
	// Caution: Changing this after creating FastNoiseSIMD objects has undefined behaviour
	static void SetSIMDLevel(int level) { s_currentSIMDLevel = level; }

	// Free a noise set from memory
	static void FreeNoiseSet(float* noiseSet);

	// Create an empty (aligned) noise set for use with FillNoiseSet()
	static float* GetEmptySet(int size);

	// Create an empty (aligned) noise set for use with FillNoiseSet()
	static float* GetEmptySet(int xSize, int ySize, int zSize) { return GetEmptySet(xSize*ySize*zSize); }

	static int AlignedSize(int size);


	// Returns seed used for all noise types
	int GetSeed(void) const { return m_seed; }

	// Sets seed used for all noise types
	// Default: 1337
	void SetSeed(int seed) { m_seed = seed; }

	// Sets frequency for all noise types
	// Default: 0.01
	void SetFrequency(float frequency) { m_frequency = frequency; }

	// Sets noise return type of (Get/Fill)NoiseSet()
	// Default: Simplex
	void SetNoiseType(NoiseType noiseType) { m_noiseType = noiseType; }

	// Sets scaling factor for individual axis
	// Defaults: 1.0
	void SetAxisScales(float xScale, float yScale, float zScale) { m_xScale = xScale; m_yScale = yScale; m_zScale = zScale; }


	// Sets octave count for all fractal noise types
	// Default: 3
	void SetFractalOctaves(int octaves) { m_octaves = octaves; m_fractalBounding = CalculateFractalBounding(m_octaves, m_gain);	}

	// Sets octave lacunarity for all fractal noise types
	// Default: 2.0
	void SetFractalLacunarity(float lacunarity) { m_lacunarity = lacunarity; }

	// Sets octave gain for all fractal noise types
	// Default: 0.5
	void SetFractalGain(float gain) { m_gain = gain; m_fractalBounding = CalculateFractalBounding(m_octaves, m_gain); }

	// Sets method for combining octaves in all fractal noise types
	// Default: FBM
	void SetFractalType(FractalType fractalType) { m_fractalType = fractalType; }


	// Sets return type from cellular noise calculations
	// Default: Distance
	void SetCellularReturnType(CellularReturnType cellularReturnType) { m_cellularReturnType = cellularReturnType; }

	// Sets distance function used in cellular noise calculations
	// Default: Euclidean
	void SetCellularDistanceFunction(CellularDistanceFunction cellularDistanceFunction) { m_cellularDistanceFunction = cellularDistanceFunction; }


	// Enables position perturbing for all noise types
	// Default: None
	void SetPerturbType(PerturbType perturbType) { m_perturbType = perturbType; }

	// Sets the maximum distance the input position can be perturbed
	// Default: 1.0
	void SetPerturbAmp(float perturbAmp) { m_perturbAmp = perturbAmp / 511.5f; }

	// Set the relative frequency for the perturb gradient
	// Default: 0.5
	void SetPerturbFrequency(float perturbFrequency) { m_perturbFrequency = perturbFrequency; }


	// Sets octave count for perturb fractal types
	// Default: 3
	void SetPerturbFractalOctaves(int perturbOctaves) { m_perturbOctaves = perturbOctaves; m_perturbFractalBounding = CalculateFractalBounding(m_perturbOctaves, m_perturbGain); }

	// Sets octave lacunarity for perturb fractal types 
	// Default: 2.0
	void SetPerturbFractalLacunarity(float perturbLacunarity) { m_perturbLacunarity = perturbLacunarity; }
	
	// Sets octave gain for perturb fractal types 
	// Default: 0.5
	void SetPerturbFractalGain(float perturbGain) { m_perturbGain = perturbGain; m_perturbFractalBounding = CalculateFractalBounding(m_perturbOctaves, m_perturbGain);	}

	static FastNoiseVectorSet* GetVectorSet(int xSize, int ySize, int zSize);
	static FastNoiseVectorSet* GetSamplingVectorSet(int sampleScale, int xSize, int ySize, int zSize);
	static void FillVectorSet(FastNoiseVectorSet* vectorSet, int xSize, int ySize, int zSize);
	static void FillSamplingVectorSet(FastNoiseVectorSet* vectorSet, int sampleScale, int xSize, int ySize, int zSize);

	float* GetNoiseSet(int xStart, int yStart, int zStart, int xSize, int ySize, int zSize, float scaleModifier = 1.0f);
	void FillNoiseSet(float* noiseSet, int xStart, int yStart, int zStart, int xSize, int ySize, int zSize, float scaleModifier = 1.0f);
	void FillNoiseSet(float* noiseSet, FastNoiseVectorSet* vectorSet, float xOffset = 0.0f, float yOffset = 0.0f, float zOffset = 0.0f);

	float* GetSampledNoiseSet(int xStart, int yStart, int zStart, int xSize, int ySize, int zSize, int sampleScale);
	virtual void FillSampledNoiseSet(float* noiseSet, int xStart, int yStart, int zStart, int xSize, int ySize, int zSize, int sampleScale) = 0;
	virtual void FillSampledNoiseSet(float* noiseSet, FastNoiseVectorSet* vectorSet, float xOffset = 0.0f, float yOffset = 0.0f, float zOffset = 0.0f) = 0;

	float* GetWhiteNoiseSet(int xStart, int yStart, int zStart, int xSize, int ySize, int zSize, float scaleModifier = 1.0f);
	virtual void FillWhiteNoiseSet(float* noiseSet, int xStart, int yStart, int zStart, int xSize, int ySize, int zSize, float scaleModifier = 1.0f) = 0;
	virtual void FillWhiteNoiseSet(float* noiseSet, FastNoiseVectorSet* vectorSet, float xOffset = 0.0f, float yOffset = 0.0f, float zOffset = 0.0f) = 0;

	float* GetValueSet(int xStart, int yStart, int zStart, int xSize, int ySize, int zSize, float scaleModifier = 1.0f);
	float* GetValueFractalSet(int xStart, int yStart, int zStart, int xSize, int ySize, int zSize, float scaleModifier = 1.0f);
	virtual void FillValueSet(float* noiseSet, int xStart, int yStart, int zStart, int xSize, int ySize, int zSize, float scaleModifier = 1.0f) = 0;
	virtual void FillValueFractalSet(float* noiseSet, int xStart, int yStart, int zStart, int xSize, int ySize, int zSize, float scaleModifier = 1.0f) = 0;
	virtual void FillValueSet(float* noiseSet, FastNoiseVectorSet* vectorSet, float xOffset = 0.0f, float yOffset = 0.0f, float zOffset = 0.0f) = 0;
	virtual void FillValueFractalSet(float* noiseSet, FastNoiseVectorSet* vectorSet, float xOffset = 0.0f, float yOffset = 0.0f, float zOffset = 0.0f) = 0;

	float* GetPerlinSet(int xStart, int yStart, int zStart, int xSize, int ySize, int zSize, float scaleModifier = 1.0f);
	float* GetPerlinFractalSet(int xStart, int yStart, int zStart, int xSize, int ySize, int zSize, float scaleModifier = 1.0f);
	virtual void FillPerlinSet(float* noiseSet, int xStart, int yStart, int zStart, int xSize, int ySize, int zSize, float scaleModifier = 1.0f) = 0;
	virtual void FillPerlinFractalSet(float* noiseSet, int xStart, int yStart, int zStart, int xSize, int ySize, int zSize, float scaleModifier = 1.0f) = 0;
	virtual void FillPerlinSet(float* noiseSet, FastNoiseVectorSet* vectorSet, float xOffset = 0.0f, float yOffset = 0.0f, float zOffset = 0.0f) = 0;
	virtual void FillPerlinFractalSet(float* noiseSet, FastNoiseVectorSet* vectorSet, float xOffset = 0.0f, float yOffset = 0.0f, float zOffset = 0.0f) = 0;

	float* GetSimplexSet(int xStart, int yStart, int zStart, int xSize, int ySize, int zSize, float scaleModifier = 1.0f);
	float* GetSimplexFractalSet(int xStart, int yStart, int zStart, int xSize, int ySize, int zSize, float scaleModifier = 1.0f);
	virtual void FillSimplexSet(float* noiseSet, int xStart, int yStart, int zStart, int xSize, int ySize, int zSize, float scaleModifier = 1.0f) = 0;
	virtual void FillSimplexFractalSet(float* noiseSet, int xStart, int yStart, int zStart, int xSize, int ySize, int zSize, float scaleModifier = 1.0f) = 0;
	virtual void FillSimplexSet(float* noiseSet, FastNoiseVectorSet* vectorSet, float xOffset = 0.0f, float yOffset = 0.0f, float zOffset = 0.0f) = 0;
	virtual void FillSimplexFractalSet(float* noiseSet, FastNoiseVectorSet* vectorSet, float xOffset = 0.0f, float yOffset = 0.0f, float zOffset = 0.0f) = 0;

	float* GetCellularSet(int xStart, int yStart, int zStart, int xSize, int ySize, int zSize, float scaleModifier = 1.0f);
	virtual void FillCellularSet(float* noiseSet, int xStart, int yStart, int zStart, int xSize, int ySize, int zSize, float scaleModifier = 1.0f) = 0;
	virtual void FillCellularSet(float* noiseSet, FastNoiseVectorSet* vectorSet, float xOffset = 0.0f, float yOffset = 0.0f, float zOffset = 0.0f) = 0;

	virtual ~FastNoiseSIMD() { }

protected:
	int m_seed = 1337;
	float m_frequency = 0.01f;
	NoiseType m_noiseType = SimplexFractal;

	float m_xScale = 1.0f;
	float m_yScale = 1.0f;
	float m_zScale = 1.0f;

	int m_octaves = 3;
	float m_lacunarity = 2.0f;
	float m_gain = 0.5f;
	FractalType m_fractalType = FBM;	
	float m_fractalBounding;	

	CellularDistanceFunction m_cellularDistanceFunction = Euclidean;
	CellularReturnType m_cellularReturnType = Distance;

	PerturbType m_perturbType = None;
	float m_perturbAmp = 1.0f;
	float m_perturbFrequency = 0.5f;

	int m_perturbOctaves = 3;
	float m_perturbLacunarity = 2.0f;
	float m_perturbGain = 0.5f;
	float m_perturbFractalBounding;

	static int s_currentSIMDLevel;
	static float CalculateFractalBounding(int octaves, float gain);
};

struct FastNoiseVectorSet
{
public:
	int size = -1;
	float* xSet = nullptr;
	float* ySet = nullptr;
	float* zSet = nullptr;

	// Only used for sampled vector sets
	int sampleScale = 0;
	int sampleSizeX = -1;
	int sampleSizeY = -1;
	int sampleSizeZ = -1;

	FastNoiseVectorSet() {}

	FastNoiseVectorSet(int _size) { SetSize(_size); }

	~FastNoiseVectorSet() { Free(); }

	void Free();

	void SetSize(int _size);
};

#define FN_NO_SIMD_FALLBACK 0
#define FN_SSE2 1
#define FN_SSE41 2
#define FN_AVX2 3
#define FN_AVX512 4
#define FN_NEON 5
#endif