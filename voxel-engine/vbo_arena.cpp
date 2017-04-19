#include "stdafx.h"
#include "vbo_arena.h"
#include "util.h"
#include "camera.h"

VboArena::VboArena() : capacity(DEFAULT_VBOARENA_SIZE), size(0)
{
	glGenBuffers(1, &vboID);
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	glBufferData(GL_ARRAY_BUFFER, DEFAULT_VBOARENA_SIZE * sizeof(glm::vec3), NULL, GL_STREAM_DRAW);
	addFreeBlock(DataBlock{ 0, (size_t)DEFAULT_VBOARENA_SIZE });
}

void VboArena::deleteData()
{
	glDeleteBuffers(1, &vboID);
}

DataBlock VboArena::getFreeBlock()
{
	auto freeBlock = freeStarts.begin();
	DataBlock block;
	if (freeBlock != freeStarts.end() && freeBlock->first < capacity && freeBlock->second > capacity)
	{
		block = { freeBlock->first, capacity - freeBlock->first };
		freeEnds.erase(freeBlock->second);
		freeStarts.erase(freeBlock->first);
	}
	else if (freeBlock != freeStarts.end() && freeBlock->second <= capacity)
	{
		block = { freeBlock->first, freeBlock->second - freeBlock->first };
		freeEnds.erase(freeBlock->second);
		freeStarts.erase(freeBlock->first);
	}
	else //no more free blocks
	{
		resizeArena(capacity * 2);
		freeBlock = freeStarts.begin();
		block = { freeBlock->first, freeBlock->second - freeBlock->first };
		freeEnds.erase(freeBlock->second);
		freeStarts.erase(freeBlock->first);
	}
	return(block);
}

void VboArena::addFreeBlock(DataBlock &block)
{
	if (block.size == 0) return;
	mergeFreeBlock(block);
	//resize by half if only a quarter of space is used
	//if ((double)size / space < 0.25 && space > DEFAULT_VBOARENA_SIZE)
	//	resizeArena(space * 0.5);
}

void VboArena::mergeFreeBlock(DataBlock &block)
{
	auto pushBack = freeEnds.find(block.startIndex);
	auto pushFront = freeStarts.find(block.startIndex + block.size);
	//check if this can merged with another block
	//if start is the end of another block...
	if (pushBack != freeEnds.end())
	{
		DataBlock newBlock = DataBlock{ pushBack->second, pushBack->first - pushBack->second + block.size };
		freeEnds.erase(pushBack);
		mergeFreeBlock(newBlock);
	}
	//if end is start of another block..
	else if (pushFront != freeStarts.end())
	{
		DataBlock newBlock = DataBlock{ block.startIndex, pushFront->second - pushFront->first + block.size };
		freeStarts.erase(pushFront);
		mergeFreeBlock(newBlock);
	}
	//otherwise its good on its own
	else
	{
		freeStarts[block.startIndex] = block.startIndex + block.size;
		freeEnds[block.startIndex + block.size] = block.startIndex;
	}
}

void VboArena::resizeArena(size_t newCapacity)
{
	//Create temporary vbo
	GLuint newVbo;
	glGenBuffers(1, &newVbo);
	glBindBuffer(GL_ARRAY_BUFFER, newVbo);
	glBufferData(GL_ARRAY_BUFFER, capacity * sizeof(glm::vec3), NULL, GL_STREAM_DRAW);
	//Copy contents to temp vbo
	glBindBuffer(GL_COPY_WRITE_BUFFER, newVbo);
	glBindBuffer(GL_COPY_READ_BUFFER, vboID);
	glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, capacity * sizeof(glm::vec3));
	//Resize original vbo
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	glBufferData(GL_ARRAY_BUFFER, newCapacity * sizeof(glm::vec3), NULL, GL_STREAM_DRAW);
	//Copy back
	glBindBuffer(GL_COPY_WRITE_BUFFER, vboID);
	glBindBuffer(GL_COPY_READ_BUFFER, newVbo);
	glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, capacity * sizeof(glm::vec3));
	glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
	glBindBuffer(GL_COPY_READ_BUFFER, 0);
	glDeleteBuffers(1, &newVbo);

	if (newCapacity > capacity) //upscale
	{
		addFreeBlock(DataBlock{capacity, (newCapacity - capacity)});
	}
	else //downscale
	{
		
	}
	capacity = newCapacity;
}
/*
void VboArena::addRegion(ChunkMeshInfo &meshInfo)
{
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	auto it = meshInfo.meshData.begin();
	size_t remainingInputSize = meshInfo.meshData.size();
	size_t amountWritten;
	while (it != meshInfo.meshData.end())
	{
		if (remainingInputSize == 0)
			break;
		DataBlock block = getFreeBlock();
		if (block.size > remainingInputSize) //freeBlock is big enough for all the new data
		{
			glm::vec3 *offsetsPointer = (glm::vec3*)glMapBufferRange(GL_ARRAY_BUFFER, block.startIndex * sizeof(glm::vec3), remainingInputSize * sizeof(glm::vec3),
				GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
			std::copy(it, it + remainingInputSize, offsetsPointer);
			amountWritten = remainingInputSize;
			DataBlock remainingBlock = { block.startIndex + amountWritten, block.size - amountWritten };
			addFreeBlock(remainingBlock);
		}
		else //freeBlock can only fit a subset of the new data
		{
			glm::vec3 *offsetsPointer = (glm::vec3*)glMapBufferRange(GL_ARRAY_BUFFER, block.startIndex * sizeof(glm::vec3), block.size * sizeof(glm::vec3),
				GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
			std::copy(it, it + block.size, offsetsPointer);
			amountWritten = block.size;
		}
		regions[block.startIndex] = DataBlock{ block.startIndex, amountWritten, *it };
		meshInfo.vboRegions.push_back(block.startIndex);

		size += amountWritten;
		it += amountWritten;
		remainingInputSize -= amountWritten;
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}
}

void VboArena::freeRegion(ChunkMeshInfo &meshData)
{
	glBindBuffer(GL_ARRAY_BUFFER, vboID);

	for (auto &blockI : meshData.vboRegions)
	{
		DataBlock *block = &regions[blockI];
		glm::vec3 *offsetsPointer = (glm::vec3*)glMapBufferRange(GL_ARRAY_BUFFER, block->startIndex * sizeof(glm::vec3), block->size * sizeof(glm::vec3),
			GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
		for (int i = 0; i < block->size; ++i)
			offsetsPointer[i] = NULL_LOCATION;
		glUnmapBuffer(GL_ARRAY_BUFFER);
		addFreeBlock(*block);
		regions.erase(blockI);
		size -= block->size;
	}
}*/

size_t VboArena::getSize()
{
	return(size);
}

void VboArena::drawInstances()
{
	for (auto &block : regions)
	{
		if (pointInFrustum(block.second.startData))
			glDrawArraysInstancedBaseInstance(GL_TRIANGLES, 0, 6, (GLsizei)block.second.size, (GLuint)block.second.startIndex);
	}
}