#pragma once

#include "Core/Util.h"
#include "Framework/Asset/Vertex.h"

#include "Vec.hpp"
#include "Shape.h"

#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace ember {

	enum class MeshTopology {
		TRIANGLES,
		TRIANGLE_STRIP,
		LINES,
		LINE_STRIP,
		POINTS,
		PATCHES,
	};

	struct MeshStat {
		bool IsIndexed() const;

		VertexBufferInfo vbInfo{};
		IndexBufferInfo ibInfo{};

		MeshTopology meshTopology{};

		uint32_t attributesMask{ 0 };

		uint32_t patchVertexCount{}; // tessellation, if used

		bool isDynamic{ false };
		bool isTessellated{ false };
		bool cullBackFaces{ true };
	};

	uint32_t GetIndexMultiplicity(MeshTopology meshTopology);

	class Mesh {
	public:
		Mesh();
		~Mesh();
		CLASS_NO_COPY(Mesh);
		CLASS_DEFAULT_MOVE(Mesh);

		void SetPositions(const std::vector<numa::Vec3>& positions);

		void SetNormals(const std::vector<numa::Vec3>& normals);
		void ResetNormals();
		bool HasNormals() const;

		void SetTangents(const std::vector<numa::Vec3>& tangents);
		void ResetTangents();
		bool HasTangents() const;

		void SetColors(const std::vector<numa::Vec3>& colors);
		void ResetColors();
		bool HasColors() const;

		void SetUvs(const std::vector<numa::Vec2>& uvs);
		void ResetUvs();
		bool HasUvs() const;

		template <typename Vertex>
		void SetVertices(const std::vector<Vertex>& vertices) {
			SetVertices(vertices.data(), static_cast<uint32_t>(vertices.size()), Vertex::attributes);
		}
		void SetVertices(const void* src, uint32_t vertexCount, const std::vector<VertexAttribDescriptor>& layout);

		void SetIndices(const std::vector<uint32_t>& indices);
		void ResetIndices();
		bool HasIndices() const;

		std::vector<char> ConstructMeshVertexBuffer() const;
		std::vector<char> ConstructMeshIndexBuffer() const;

		MeshStat GetMeshStat() const;
		VertexBufferInfo GetVertexBufferInfo() const;
		IndexBufferInfo GetIndexBufferInfo() const;

		void SetVertexAttribDescriptor(const VertexAttribDescriptor& vertAttribDesc);
		void ResetVertexAttribDescriptor(VertexAttribChannel channel);
		bool GetVertexAttribDescriptor(VertexAttribDescriptor& attribDesc, VertexAttribChannel channel) const;

		void SetVertexAttribLayout(const std::vector<VertexAttribDescriptor>& vertAttribLayout);
		std::vector<VertexAttribDescriptor> GetVertexAttribLayout() const;

		// Doesn't set the 'offset' member variable.
		VertexAttribDescriptor GetDefaultPositionVertexAttribDescriptor() const;
		// Doesn't set the 'offset' member variable.
		VertexAttribDescriptor GetDefaultNormalVertexAttribDescriptor() const;
		// Doesn't set the 'offset' member variable.
		VertexAttribDescriptor GetDefaultTangentVertexAttribDescriptor() const;
		// Doesn't set the 'offset' member variable.
		VertexAttribDescriptor GetDefaultColorVertexAttribDescriptor() const;
		// Doesn't set the 'offset' member variable.
		VertexAttribDescriptor GetDefaultUvVertexAttribDescriptor() const;

		uint32_t GetAttributesMask() const;

		const numa::AABB& GetObjectAABB() const;

		numa::AABB ComputeWorldAABBApproximate(const numa::Mat4& world) const;
		numa::AABB ComputeWorldAABBPrecise(const numa::Mat4& world) const;

		numa::OBB ComputeWorldOBB(const numa::Mat4& world) const;

		void SetObjectAABBPadding(float padding);
		void SetObjectAABBPadding(const numa::Vec3& padding);

		void SetVertexCount(size_t vertexCount);
		size_t GetVertexCount() const;

		void SetIndexCount(size_t indexCount);
		size_t GetIndexCount() const;

		uint32_t GetVertexStride() const;

		void SetMeshTopology(MeshTopology meshTopology);
		MeshTopology GetMeshTopology() const;

		void SetCullBackFaceState(bool cullBackFaces);
		bool CullBackFaces() const;

		void SetIndexFormat(IndexFormat format);
		IndexFormat GetIndexFormat() const;

		uint32_t GetMeshId() const;

		void MakeDynamic();
		void MakeStatic();

		bool IsDynamic() const;

	private:
		void SendMeshChangedEventNotifications() const;

		void OnVertexDataUpdated() const;
		void OnIndexDataUpdated() const;
		void OnMeshSettingsUpdated() const;
		void OnMeshDataUpdated() const;

		void ResizeVertexAttribArrays();

		// Sets the attribute descriptors and ensures that the layout is valid.
		// If there's an invalid attribute descriptor, its corresponding default version is used instead.
		void SetVertexAttribLayoutMap(const std::vector<VertexAttribDescriptor>& vertAttribLayout);

		void ResizeNormalVertexAttribArray(uint32_t size);
		void ResizeTangentVertexAttribArray(uint32_t size);
		void ResizeColorVertexAttribArray(uint32_t size);
		void ResizeUvVertexAttribArray(uint32_t size);

		bool IndicesOutOfBound(std::vector<uint32_t>* outOfBoundIndices = nullptr);
		bool IndicesIncomplete(std::vector<uint32_t>* incompleteIndices = nullptr);

		void ReportOutOfBoundIndices(const std::vector<uint32_t>& outOfBoundIndices);
		void ReportIncompleteIndices(const std::vector<uint32_t>& incompleteIndices);

		void ComputeObjectAABB();
		void ApplyObjectAABBPadding(numa::Vec3& min, numa::Vec3& max) const;

		// I/O methods.

		bool FormatConversionPossible(VertexAttribFormat srcFormat,
			                          VertexAttribFormat destFormat) const;
		bool DimensionalityConversionPossible(const VertexAttribDescriptor& srcDesc,
			                                  const VertexAttribDescriptor& destDesc) const;
		bool WritePossible(const VertexAttribDescriptor& srcDesc,
			               const VertexAttribDescriptor& destDesc) const;

		void UpdateGpuMeshVertexData() const;
		void UpdateGpuMeshIndexData() const;

		void ConstructMeshVertexBuffer(char* vb, uint32_t vertexCount,
			                           const std::vector<VertexAttribDescriptor>& layout) const;
		void ConstructMeshIndexBuffer(char* ib, uint32_t indexCount,
			                          IndexFormat ibFormat) const;

		void UpdateGpuMeshSettings() const;

		void WriteSrcAttribToDstBuffer(const VertexAttribDescriptor& dstAttribDesc, char* dstBuffer, uint32_t vert) const;
		void WriteSrcAttribToDstBuffer(const VertexAttribDescriptor& srcAttribDesc, const char* srcBuffer,
			                           const VertexAttribDescriptor& dstAttribDesc, char* dstBuffer) const;

		void WriteSrcIndexToDstBuffer(IndexFormat indexFormat, char* dstBuffer, uint32_t index) const;

		void SetInternalVertexAttribArrayData(const void* src, uint32_t vertexCount,
			                                  const std::vector<VertexAttribDescriptor>& layout);

		void WriteSrcDataToAttribArray(const VertexAttribDescriptor& srcAttribDesc,
			                           uint32_t vert, const char* srcBuffer);
		void WriteSrcDataToAttribArray(const VertexAttribDescriptor& srcAttribDesc, const char* srcBuffer,
			                           const VertexAttribDescriptor& dstAttribDesc, char* dstBuffer);

		void SendGpuMeshVertexBufferData(const void* vb) const;
		void SendGpuMeshIndexBufferData(const void* ib) const;

		std::unordered_map<VertexAttribChannel, VertexAttribDescriptor> vertAttribLayout;

		CallbackStorage<uint32_t, std::function<void()>> meshChangedCallbackStorage;

		std::string name;

		std::vector<numa::Vec3> positions;
		std::vector<numa::Vec3> normals;
		std::vector<numa::Vec3> tangents;
		std::vector<numa::Vec3> colors;
		std::vector<numa::Vec2> uvs;

		std::vector<uint32_t> indices;

		numa::AABB objectAABB{};
		numa::Vec3 aabbPadding{0.0f};

		IndexFormat indexFormat{IndexFormat::UINT32};
		MeshTopology meshTopology{MeshTopology::TRIANGLES};

		uint32_t meshId{0};

		bool dynamic{false};
		bool cullBackFaces{true};
	};

}