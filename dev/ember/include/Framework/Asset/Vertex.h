#pragma once

#include <Vec.hpp>

#include <array>
#include <cstdint>
#include <vector>

namespace ember {

	enum class VertexAttribChannel {
		UNDEFINED = -1,
		POSITION  =  0,
		NORMAL    =  1,
		TANGENT   =  2,
		COLOR     =  3,
		UV0       =  4,
		UV1       =  5,
		COUNT
	};

	enum class VertexAttribFormat {
		INT32,
		INT16,
		INT8,
		UINT32,
		UINT16,
		UINT8,
		FLOAT32,
	};

	uint32_t GetVertexAttribFormatSizeInBytes(VertexAttribFormat vertexAttribFormat);

	bool IsVertexAttribFormatInt(VertexAttribFormat vertexAttribFormat);
	bool IsVertexAttribFormatUint(VertexAttribFormat vertexAttribFormat);
	bool IsVertexAttribFormatFloat(VertexAttribFormat vertexAttribFormat);

	struct VertexAttribDescriptor {
		uint32_t GetVertexAttribSize() const;

		uint32_t dimension{0};
		uint32_t offset{0};
		VertexAttribChannel channel{};
		VertexAttribFormat format{};
	};

	uint32_t CalculateVertexStride(const std::vector<VertexAttribDescriptor>& vertAttribLayout);

	// Why is it here? Shouldn't it be in the Mesh class?
	template <typename SrcType, typename DstType>
	void WriteSrcToDst(const SrcType* src, DstType* dst) {
		DstType value = static_cast<DstType>(*src);
		*dst = value;
	}

	// P - position.
	struct VertexP {
		numa::Vec3 vertexPosition;
		static constexpr uint32_t stride = sizeof(numa::Vec3);
		static const std::vector<VertexAttribDescriptor> attributes;
	};
	// P - position, N - normal.
	struct VertexPN {
		numa::Vec3 vertexPosition;
		numa::Vec3 vertexNormal;
		static constexpr uint32_t stride = 2 * sizeof(numa::Vec3);
		static const std::vector<VertexAttribDescriptor> attributes;
	};
	// P - position, C - color.
	struct VertexPC {
		numa::Vec3 vertexPosition;
		numa::Vec3 vertexColor;
		static constexpr uint32_t stride = 2 * sizeof(numa::Vec3);
		static const std::vector<VertexAttribDescriptor> attributes;
	};
	// P - position, U - uv texture coordinates.
	struct VertexPU {
		numa::Vec3 vertexPosition;
		numa::Vec2 vertexUv;
		static constexpr uint32_t stride = sizeof(numa::Vec3) + sizeof(numa::Vec2);
		static const std::vector<VertexAttribDescriptor> attributes;
	};
	// P - position, N - normal, T - tangent.
	struct VertexPNT {
		numa::Vec3 vertexPosition;
		numa::Vec3 vertexNormal;
		numa::Vec3 vertexTangent;
		static constexpr uint32_t stride = 3 * sizeof(numa::Vec3);
		static const std::vector<VertexAttribDescriptor> attributes;
	};
	// P - position, N - normal, C - color.
	struct VertexPNC {
		numa::Vec3 vertexPosition;
		numa::Vec3 vertexNormal;
		numa::Vec3 vertexColor;
		static constexpr uint32_t stride = 3 * sizeof(numa::Vec3);
		static const std::vector<VertexAttribDescriptor> attributes;
	};
	// P - position, N - normal, U - uv
	struct VertexPNU {
		numa::Vec3 vertexPosition;
		numa::Vec3 vertexNormal;
		numa::Vec2 vertexUv;
		static constexpr uint32_t stride = 2 * sizeof(numa::Vec3) + sizeof(numa::Vec2);
		static const std::vector<VertexAttribDescriptor> attributes;
	};
	// P - position, N - normal, T - tangent, U - uv
	struct VertexPNTU {
		numa::Vec3 vertexPosition;
		numa::Vec3 vertexNormal;
		numa::Vec3 vertexTangent;
		numa::Vec2 vertexUv;
		static constexpr uint32_t stride = 3 * sizeof(numa::Vec3) + sizeof(numa::Vec2);
		static const std::vector<VertexAttribDescriptor> attributes;
	};
	// P - position, N - normal, T - tangent, C - color, U - uv
	struct VertexPNTCU {
		numa::Vec3 vertexPosition;
		numa::Vec3 vertexNormal;
		numa::Vec3 vertexTangent;
		numa::Vec3 vertexColor;
		numa::Vec2 vertexUv;
		static constexpr uint32_t stride = 4 * sizeof(numa::Vec3) + sizeof(numa::Vec2);
		static const std::vector<VertexAttribDescriptor> attributes;
	};

	struct VertexBufferInfo {
		std::vector<VertexAttribDescriptor> vertexAttribLayout;
		uint32_t vertexCount{ 0 };
		uint32_t vertexStride{ 0 };
	};

	enum class IndexFormat {
		UINT32,
		UINT16,
		UINT8,
	};

	uint32_t GetIndexFormatSizeInBytes(IndexFormat indexFormat);

	struct IndexBufferInfo {
		uint32_t indexCount{0};
		IndexFormat indexFormat{};
	};

}