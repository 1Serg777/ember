#include "GpuApi/Vulkan/Memory/VulkanMemoryAllocator.h"

#include <algorithm>
#include <cassert>
#include <stdexcept>

namespace ember {

	size_t VulkanMemoryBlock::GetPayloadOffset() const {
		return offset + padding;
	}

	void VulkanMemoryAllocator::Initialize(VkDevice device, size_t allocationSize, uint32_t memoryTypeIndex) {
		assert(!initialized && "Allocator is already initialized! "
			   "Call 'Destroy' first if you need to change the allocation size or the memory type!");

		VkMemoryAllocateInfo allocationInfo{};
		allocationInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocationInfo.allocationSize = allocationSize;
		allocationInfo.memoryTypeIndex = memoryTypeIndex;
		if (vkAllocateMemory(device, &allocationInfo, nullptr, &deviceMemory) != VK_SUCCESS) {
			throw std::runtime_error{"Failed to allocate device memory!"};
		}

		this->allocationSize = allocationSize;
		this->memoryTypeIndex = memoryTypeIndex;
		InitFirstBlock();
		initialized = true;
	}
	void VulkanMemoryAllocator::Destroy(VkDevice device) {
		assert(initialized && "Memory must be allocated first!");
		memoryBlocks.clear();
		allocationSize = 0;
		memoryTypeIndex = 0;
		vkFreeMemory(device, deviceMemory, nullptr);
		deviceMemory = VK_NULL_HANDLE;
		initialized = false;
	}

	VulkanMemoryMarker VulkanMemoryAllocator::Alloc(size_t size, uint32_t alignment) {
		std::list<VulkanMemoryBlock>::iterator iter = FindSuitableBlock(size, alignment);
		if (iter == memoryBlocks.end()) {
			// TODO: think about different error reporting strategies.
			// Is throwing a runtime exception really the best possible approach?
			throw std::runtime_error{"Allocation failed: there is not enough memory!"};
		}
		std::list<VulkanMemoryBlock>::iterator newBlockIter = ClaimMemoryBlock(iter, size, alignment);
		return newBlockIter->GetPayloadOffset();
	}
	void VulkanMemoryAllocator::Free(VulkanMemoryMarker marker) {
		// Free the memory block, potentially joining it with the nearby ones.
		std::list<VulkanMemoryBlock>::iterator memoryBlockIter = FindBlock(marker);
		memoryBlockIter->free = true;

		// First we look if we can join it with the ones to the left.
		std::list<VulkanMemoryBlock>::iterator leftmostFreeBlock = FindFirstFreeBlockRangeLeft(memoryBlockIter);
		// Before we delete the range of free memory blocks to the left, we must capture all the information
		// that will be required to update the marker block.
		size_t newSize = (memoryBlockIter->offset - leftmostFreeBlock->offset) + memoryBlockIter->size;
		memoryBlockIter->size = newSize;
		memoryBlockIter->offset = leftmostFreeBlock->offset;
		memoryBlockIter->padding = 0;
		memoryBlocks.erase(leftmostFreeBlock, memoryBlockIter);

		// And then we try to see if it can be joined with the blocks to the right.
		std::list<VulkanMemoryBlock>::iterator rightmostFreeBlock = FindFirstFreeBlockRangeRight(memoryBlockIter);
		newSize = (rightmostFreeBlock->offset - memoryBlockIter->offset) + rightmostFreeBlock->size;
		memoryBlockIter->size = newSize;
		memoryBlocks.erase(++memoryBlockIter, rightmostFreeBlock); // can potentially be [end, end), is that ok?
	}
	VulkanMemoryMarker VulkanMemoryAllocator::Realloc(VulkanMemoryMarker marker, size_t newSize, uint32_t alignment) {
		assert(false && "Not implemented!");
		return VulkanMemoryMarker();
	}

	VkDeviceMemory VulkanMemoryAllocator::GetDeviceMemory() const {
		return deviceMemory;
	}
	size_t VulkanMemoryAllocator::GetAllocationSize() const {
		return allocationSize;
	}
	uint32_t VulkanMemoryAllocator::GetMemoryTypeIndex() const {
		return memoryTypeIndex;
	}
	bool VulkanMemoryAllocator::IsInitialized() const {
		return initialized;
	}

	void VulkanMemoryAllocator::InitFirstBlock() {
		// The specification guarantees that every memory allocation obtained from
		// the 'vkAllocateMemory' function is going to be properly aligned
		// for every possible resource.
		// In other words, the start address of the allocation is suitable for every resource.
		VulkanMemoryBlock firstBlock{};
		firstBlock.size = allocationSize;
		firstBlock.offset = 0;
		firstBlock.padding = 0;
		firstBlock.free = true;
		memoryBlocks.push_back(std::move(firstBlock));
	}

	bool VulkanMemoryAllocator::IsBlockSuitable(const VulkanMemoryBlock& block, size_t size, uint32_t alignment) {
		if (!block.free)
			return false;
		const size_t sizeRequested = size;
		size_t sizeAvailable{0};
		// Because of the alignment requirement, the available size in the block might change.
		// More precisely, it can only decrease when the block's offset isn't alligned properly.
		size_t paddingRequired = AlignOffset(block.offset, alignment) - block.offset;
		sizeAvailable = block.size - paddingRequired;
		if (sizeRequested <= sizeAvailable)
			return true;
		else
			return false;
	}

	std::list<VulkanMemoryBlock>::iterator VulkanMemoryAllocator::FindSuitableBlock(size_t size, uint32_t alignment) {
		std::list<VulkanMemoryBlock>::iterator iter;
		for (iter = memoryBlocks.begin(); iter != memoryBlocks.end(); iter++) {
			if (IsBlockSuitable(*iter, size, alignment)) {
				break;
			}
		}
		return iter;
	}
	std::list<VulkanMemoryBlock>::iterator VulkanMemoryAllocator::ClaimMemoryBlock(std::list<VulkanMemoryBlock>::iterator iter,
		                                                                           size_t size, uint32_t alignment) {
		// Found the first appropriate block.
		// Now we need to break it up into two parts and alter the list to reflect the changes.
		// The first part will be a new block of the requested size and alignment, whereas the second part
		// will have what's left of the old block.
		// Remember, a new element in a list is inserted before the iterator provided in the argument.
		// What this means is that the old block will become the second part block, and the new one
		// inserted into the list is going to be the first part block.

		uint32_t firstBlockPadding = CalculatePadding(iter->offset, alignment);
		VulkanMemoryBlock firstPartBlock{};
		firstPartBlock.size = size + firstBlockPadding;
		firstPartBlock.offset = iter->offset;
		firstPartBlock.padding = firstBlockPadding;
		firstPartBlock.free = false;

		VulkanMemoryBlock secondPartBlock{};
		secondPartBlock.size = iter->size - firstPartBlock.size;
		secondPartBlock.offset = firstPartBlock.offset + firstPartBlock.size;
		secondPartBlock.padding = 0;
		secondPartBlock.free = true;

		// As an optimization we could also check how big the second part block is going to end up to be.
		// If it's smaller than some threshold, such as smaller than the alignment requested, we could simply
		// add the size of the second block to the size of the first one and avoid breaking the old block into parts at all.
		// Just alter the old block according to the requested parameters and that's it.

		std::list<VulkanMemoryBlock>::iterator newBlockIter = memoryBlocks.insert(iter, firstPartBlock);
		// "No iterators or references are invalidated."
		// https://en.cppreference.com/w/cpp/container/list/insert.html
		*iter = secondPartBlock;

		return newBlockIter;
	}

	std::list<VulkanMemoryBlock>::iterator VulkanMemoryAllocator::FindBlock(VulkanMemoryMarker marker) {
		auto pred = [marker](const VulkanMemoryBlock& memoryBlock) {
			return marker == memoryBlock.GetPayloadOffset();
		};
		std::list<VulkanMemoryBlock>::iterator searchRes = std::find_if(memoryBlocks.begin(), memoryBlocks.end(), pred);
		if (searchRes == memoryBlocks.end()) {
			// TODO: think about different error reporting strategies.
			// Is throwing a runtime exception really the best possible approach?
			throw std::runtime_error{"Failed to find the memory block! Is the marker provided correct?"};
		}
		return searchRes;
	}
	std::list<VulkanMemoryBlock>::iterator VulkanMemoryAllocator::FindFirstFreeBlockRangeLeft(std::list<VulkanMemoryBlock>::iterator iter) {
		// The assumption is that the iterator provided in the argument refers to a free memory block.
		assert(iter->free && "The memory block referred to by the iterator provided in the argument must exist and be free!");
		// We're basically searching for the first free block in a chain of free blocks leading to the one given.
		// The search goes to the left.
		for (; iter != memoryBlocks.begin(); iter--) {
			if (!iter->free) {
				// Go back to the one that was 'free'.
				iter++;
				break;
			}
		}
		// Handle the edge case when we've reached the very first element of the list.
		// Notice that because of the loop condition, the first list element wasn't checked.
		// Also, if we're at 'memoryBlocks.begin()' then this means that all of the other memory blocks,
		// up to the one referenced by the argument iterator, are 'free'.
		if (iter == memoryBlocks.begin()) {
			if (iter->free) {
				return memoryBlocks.begin();
			} else {
				iter++;
			}
		}
		return iter;
	}
	std::list<VulkanMemoryBlock>::iterator VulkanMemoryAllocator::FindFirstFreeBlockRangeRight(std::list<VulkanMemoryBlock>::iterator iter) {
		// The assumption is that the iterator provided in the argument refers to a free memory block.
		assert(iter != memoryBlocks.end() && "The algorithm doesn't accept the end iterator!");
		assert(iter->free && "The memory block referred to by the iterator provided in the argument must exist and be free!");
		// We're basically searching for the first free block in a chain of free blocks leading to the one given.
		// The search goes to the right.
		for (; iter != memoryBlocks.end(); iter++) {
			if (!iter->free) {
				// Go back to the one that was 'free'.
				iter--;
				break;
			}
		}
		// Handle the edge case when we've reached the end iterator.
		if (iter == memoryBlocks.end()) {
			iter--;
		}
		return iter;
	}

	size_t AlignOffset(size_t offset, uint32_t alignment) {
		// "Game Engine Architecture" 3rd edition, Jason Gregory
		// https://www.amazon.com/Engine-Architecture-Third-Jason-Gregory/dp/1138035459
		// 6.2.1.3 Aligned Allocations
		assert(alignment == 0 && "Alignment can't be 0!");
		const size_t mask = static_cast<size_t>(alignment) - 1;
		assert((alignment & mask) == 0 && "Alignment must be a power of 2!");
		return (offset + mask) & ~mask;
	}

	uint32_t CalculatePadding(size_t offset, uint32_t alignment) {
		size_t newOffset = AlignOffset(offset, alignment);
		return static_cast<uint32_t>(newOffset - offset);
	}

}