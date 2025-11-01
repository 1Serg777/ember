#include "GpuApi/Vulkan/Memory/VulkanMemoryAllocator.h"

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
		iter = ClaimMemoryBlock(iter, size, alignment);
		return iter->GetPayloadOffset();
	}
	void VulkanMemoryAllocator::Free(VulkanMemoryMarker marker) {
		// TODO
	}
	VulkanMemoryMarker VulkanMemoryAllocator::Realloc(VulkanMemoryMarker marker, size_t newSize, uint32_t alignment) {
		// TODO
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
		uint32_t startBlockPadding = CalculatePadding(block.offset, alignment);
		VulkanMemoryBlock startBlock{};
		startBlock.size = size + startBlockPadding;
		startBlock.offset = block.offset;
		startBlock.padding = startBlockPadding;
		startBlock.free = false;

		VulkanMemoryBlock endBlock{};
		endBlock.size = block.size - startBlock.size;
		endBlock.offset = startBlock.offset + startBlock.size;
		endBlock.padding = 0;
		endBlock.free = true;

		// Remember, the new element is inserted before the iterator provided in the argument.
		// TODO

		return std::list<VulkanMemoryBlock>::iterator();
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