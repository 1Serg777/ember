#pragma once

#include <glm.hpp>

#include <array>
#include <cstdint>
#include <vector>

namespace ember {

	enum class VertexAttribType {
		POSITION = 1,
		NORMAL = 2,
		TANGENT = 4,
		COLOR = 8,
		UV = 16,
	};

	enum class VertexAttribFormat {
		FLOAT32,

		UINT32,
		UINT16,
		UINT8,

		INT32,
		INT16,
		INT8,
	};

	uint32_t GetVertexAttributeFormatSizeInBytes(VertexAttribFormat vertexAttribFormat);

	bool IsVertexAttribFormatFloat(VertexAttribFormat vertexAttribFormat);
	bool IsVertexAttribFormatUint(VertexAttribFormat vertexAttribFormat);
	bool IsVertexAttribFormatInt(VertexAttribFormat vertexAttribFormat);

	struct VertexAttribDescriptor {
		uint32_t GetVertexAttributeSize() const;

		uint32_t dimension{0}; // Attribute's dimensionality. 3 if vec3, 2 if vec2 and so on.
		uint32_t offset{0}; // Attribute's offset within the vertex.
		VertexAttribType type{};
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
		glm::vec3 vertexPosition;
		static constexpr uint32_t stride = sizeof(glm::vec3);
		static const std::vector<VertexAttribDescriptor> attributes;
	};
	// P - position, N - normal.
	struct VertexPN {
		glm::vec3 vertexPosition;
		glm::vec3 vertexNormal;
		static constexpr uint32_t stride = 2 * sizeof(glm::vec3);
		static const std::vector<VertexAttribDescriptor> attributes;
	};
	// P - position, C - color.
	struct VertexPC {
		glm::vec3 vertexPosition;
		glm::vec3 vertexColor;
		static constexpr uint32_t stride = 2 * sizeof(glm::vec3);
		static const std::vector<VertexAttribDescriptor> attributes;
	};
	// P - position, U - uv texture coordinates.
	struct VertexPU {
		glm::vec3 vertexPosition;
		glm::vec2 vertexUv;
		static constexpr uint32_t stride = sizeof(glm::vec3) + sizeof(glm::vec2);
		static const std::vector<VertexAttribDescriptor> attributes;
	};
	// P - position, N - normal, T - tangent.
	struct VertexPNT {
		glm::vec3 vertexPosition;
		glm::vec3 vertexNormal;
		glm::vec3 vertexTangent;
		static constexpr uint32_t stride = 3 * sizeof(glm::vec3);
		static const std::vector<VertexAttribDescriptor> attributes;
	};
	// P - position, N - normal, C - color.
	struct VertexPNC {
		glm::vec3 vertexPosition;
		glm::vec3 vertexNormal;
		glm::vec3 vertexColor;
		static constexpr uint32_t stride = 3 * sizeof(glm::vec3);
		static const std::vector<VertexAttribDescriptor> attributes;
	};
	// P - position, N - normal, U - uv
	struct VertexPNU {
		glm::vec3 vertexPosition;
		glm::vec3 vertexNormal;
		glm::vec2 vertexUv;
		static constexpr uint32_t stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);
		static const std::vector<VertexAttribDescriptor> attributes;
	};
	// P - position, N - normal, T - tangent, U - uv
	struct VertexPNTU {
		glm::vec3 vertexPosition;
		glm::vec3 vertexNormal;
		glm::vec3 vertexTangent;
		glm::vec2 vertexUv;
		static constexpr uint32_t stride = 3 * sizeof(glm::vec3) + sizeof(glm::vec2);
		static const std::vector<VertexAttribDescriptor> attributes;
	};
	// P - position, N - normal, T - tangent, C - color, U - uv
	struct VertexPNTCU {
		glm::vec3 vertexPosition;
		glm::vec3 vertexNormal;
		glm::vec3 vertexTangent;
		glm::vec3 vertexColor;
		glm::vec2 vertexUv;
		static constexpr uint32_t stride = 4 * sizeof(glm::vec3) + sizeof(glm::vec2);
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