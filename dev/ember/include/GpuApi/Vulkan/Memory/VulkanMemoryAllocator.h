#include <vulkan/vulkan.h>

#include <cstdint>
#include <list>

namespace ember {

	using VulkanMemoryMarker = size_t;

	struct VulkanMemoryBlock {
		size_t GetPayloadOffset() const;

		size_t size{0};
		size_t offset{0};
		uint32_t padding{0};

		bool free{true};
	};

	class VulkanMemoryAllocator {
	public:
		void Initialize(VkDevice device, size_t allocationSize, uint32_t memoryTypeIndex);
		void Destroy(VkDevice device);

		VulkanMemoryMarker Alloc(size_t size, uint32_t alignment);
		void Free(VulkanMemoryMarker marker);
		VulkanMemoryMarker Realloc(VulkanMemoryMarker marker, size_t newSize, uint32_t alignment);

		VkDeviceMemory GetDeviceMemory() const;
		size_t GetAllocationSize() const;
		uint32_t GetMemoryTypeIndex() const;
		bool IsInitialized() const;

	private:
		void InitFirstBlock();
		bool IsBlockSuitable(const VulkanMemoryBlock& block, size_t size, uint32_t alignment);

		std::list<VulkanMemoryBlock>::iterator FindSuitableBlock(size_t size, uint32_t alignment);
		std::list<VulkanMemoryBlock>::iterator ClaimMemoryBlock(std::list<VulkanMemoryBlock>::iterator iter,
			                                                    size_t size, uint32_t alignment);

		std::list<VulkanMemoryBlock>::iterator FindBlock(VulkanMemoryMarker marker);
		std::list<VulkanMemoryBlock>::iterator FindFirstFreeBlockRangeLeft(std::list<VulkanMemoryBlock>::iterator iter);
		std::list<VulkanMemoryBlock>::iterator FindFirstFreeBlockRangeRight(std::list<VulkanMemoryBlock>::iterator iter);

		std::list<VulkanMemoryBlock> memoryBlocks;
		VkDeviceMemory deviceMemory;
		size_t allocationSize{0};
		uint32_t memoryTypeIndex{0};
		bool initialized{false};
	};

	size_t AlignOffset(size_t offset, uint32_t alignment);
	uint32_t CalculatePadding(size_t offset, uint32_t alignment);

}