#include "Framework/Asset/Mesh.h"

#include "GpuApi/GpuApiCtx.h"

#include <algorithm>

namespace ember {

	template <typename SrcType, typename DstType>
	void WriteSrcToDst(const SrcType* src, DstType* dst) {
		DstType value = static_cast<DstType>(*src);
		*dst = value;
	}

	uint32_t GetIndexMultiplicity(MeshTopology meshTopology) {
		switch (meshTopology) {
			case MeshTopology::TRIANGLES:
				return 3;
			case MeshTopology::LINES:
				return 2;
			case MeshTopology::POINTS:
				return 1;
			default:
				// TRIANGLE_STRIP and LINE_STRIP are non-indexed topologies
				// PATCHES multiplicity can vary depending on the patch vertex number
				// All of them will return 0, is that correct?
				return 0;
		}
	}

	Mesh::Mesh() {
		GetCurrentGpuApiCtx()->CreateMeshGpuResource(this);
	}
	Mesh::~Mesh() {
		GetCurrentGpuApiCtx()->DeleteMeshGpuResource(this);
	}

	void Mesh::SetPositions(const std::vector<numa::Vec3>& positions) {
		if (positions.size() > this->positions.size()) {
			this->positions.resize(positions.size());
		}
		ResizeVertexAttribArrays();
		size_t minCount = std::min(this->positions.size(), positions.size());
		std::copy_n(positions.begin(), minCount, this->positions.begin());
		ComputeObjectAABB();
		OnVertexDataUpdated();
	}

	// 
	// For any of the optional vertex attribute arrays,
	// when we assign new data to them there can be two situations:
	// 
	// 1. The optional array exists and it is currently in use. This means its
	// size matches the size of the vertex position array.
	// 
	// 2. The optional array is currently not in use, which means the array doesn't exist.
	// 

	void Mesh::SetNormals(const std::vector<numa::Vec3>& normals) {
		if (!HasNormals()) {
			ResizeNormalVertexAttribArray(static_cast<uint32_t>(positions.size()));
			vertAttribLayout.insert({VertexAttribChannel::NORMAL, GetDefaultNormalVertexAttribDescriptor()});
		}
		size_t minCount = std::min(this->normals.size(), normals.size());
		std::copy_n(normals.begin(), minCount, this->normals.begin());
		OnVertexDataUpdated();
	}
	void Mesh::ResetNormals() {
		vertAttribLayout.erase(VertexAttribChannel::NORMAL);
		normals.clear();
		OnVertexDataUpdated();
	}
	bool Mesh::HasNormals() const {
		auto findRes = vertAttribLayout.find(VertexAttribChannel::NORMAL);
		return findRes != vertAttribLayout.end();
	}

	void Mesh::SetTangents(const std::vector<numa::Vec3>& tangents) {
		if (!HasTangents()) {
			ResizeTangentVertexAttribArray(static_cast<uint32_t>(positions.size()));
			vertAttribLayout.insert({VertexAttribChannel::TANGENT, GetDefaultTangentVertexAttribDescriptor()});
		}
		size_t minCount = std::min(this->tangents.size(), tangents.size());
		std::copy_n(tangents.begin(), minCount, this->tangents.begin());
		OnVertexDataUpdated();
	}
	void Mesh::ResetTangents() {
		vertAttribLayout.erase(VertexAttribChannel::TANGENT);
		tangents.clear();
		OnVertexDataUpdated();
	}
	bool Mesh::HasTangents() const {
		auto findRes = vertAttribLayout.find(VertexAttribChannel::TANGENT);
		return findRes != vertAttribLayout.end();
	}

	void Mesh::SetColors(const std::vector<numa::Vec3>& colors) {
		if (!HasColors()) {
			ResizeColorVertexAttribArray(static_cast<uint32_t>(positions.size()));
			vertAttribLayout.insert({VertexAttribChannel::COLOR, GetDefaultColorVertexAttribDescriptor()});
		}
		size_t minCount = std::min(this->colors.size(), colors.size());
		std::copy_n(colors.begin(), minCount, this->colors.begin());
		OnVertexDataUpdated();
	}
	void Mesh::ResetColors() {
		vertAttribLayout.erase(VertexAttribChannel::COLOR);
		colors.clear();
		OnVertexDataUpdated();
	}
	bool Mesh::HasColors() const {
		auto findRes = vertAttribLayout.find(VertexAttribChannel::COLOR);
		return findRes != vertAttribLayout.end();
	}

	void Mesh::SetUvs(const std::vector<numa::Vec2>& uvs) {
		if (!HasUvs()) {
			ResizeUvVertexAttribArray(static_cast<uint32_t>(positions.size()));
			vertAttribLayout.insert({VertexAttribChannel::UV0, GetDefaultUvVertexAttribDescriptor()});
		}
		size_t minCount = std::min(this->uvs.size(), uvs.size());
		std::copy_n(uvs.begin(), minCount, this->uvs.begin());
		OnVertexDataUpdated();
	}
	void Mesh::ResetUvs() {
		vertAttribLayout.erase(VertexAttribChannel::UV0);
		uvs.clear();
		OnVertexDataUpdated();
	}
	bool Mesh::HasUvs() const {
		auto findRes = vertAttribLayout.find(VertexAttribChannel::UV0);
		return findRes != vertAttribLayout.end();
	}

	void Mesh::SetVertices(const void* src, uint32_t vertexCount, const std::vector<VertexAttribDescriptor>& layout) {
		SetVertexAttribLayoutMap(layout);
		// The layout provided in the parameter can lack the positions vertex attribute, which is
		// probably because of the user's mistake. The SetVertexAttribLayoutMap() function tries
		// to fix such mistakes, if there are any, so that we always end up with a valid layout.
		// The possibility of the layout parameter being invalid is the reason why we request
		// the vertex attribute layout vector using the internal method. If any fixes had to be done
		// before assigning the provided layout, they will be reflected here.
		std::vector<VertexAttribDescriptor> meshLayout = GetVertexAttribLayout();
		if (vertexCount > this->positions.size()) {
			this->positions.resize(vertexCount);
		}
		ResizeVertexAttribArrays();
		SetInternalVertexAttribArrayData(src, vertexCount, meshLayout);
		ComputeObjectAABB();
		// Send this to the GPU
		SendGpuMeshVertexBufferData(src);
		SendMeshChangedEventNotifications();
	}

	// 
	// For indices we can perform two checks:
	// 
	// 1. "Out of bound indices" check.
	// This one, checks if there are any indices
	// that index non-existent vertices (index > vertexCount)
	// We assume that all vertices are unique.
	// 
	// 2. "Index completeness" check
	// This one takes into account the currently set mesh topology.
	// 
	// - If it is TRIANGLES, then the number of indices must be a
	// multiple of 3.
	// - If it is LINES, then the number of indices must be a multiple of 2.
	// - If it is POINTS, then anything goes.
	// 
	// Do we need to attempt to resolve any detected issues,
	// or do we just leave everything as is and only report
	// to the user that such issues are present (exist)?
	//

	void Mesh::SetIndices(const std::vector<uint32_t>& indices) {
		if (indices.size() > this->indices.size()) {
			this->indices.resize(indices.size());
		}
		size_t minCount = std::min(this->indices.size(), indices.size());
		std::copy_n(indices.begin(), minCount, this->indices.begin());

		std::vector<uint32_t> outOfBoundIndices;
		if (IndicesOutOfBound(&outOfBoundIndices)) {
			ReportOutOfBoundIndices(outOfBoundIndices);
		}
		std::vector<uint32_t> incompleteIndices;
		if (IndicesIncomplete(&incompleteIndices)) {
			ReportIncompleteIndices(incompleteIndices);
		}
		OnIndexDataUpdated();
	}
	void Mesh::ResetIndices() {
		this->indices.clear();
		OnIndexDataUpdated();
	}
	bool Mesh::HasIndices() const {
		return indices.size();
	}

	std::vector<char> Mesh::ConstructMeshVertexBuffer() const {
		std::vector<VertexAttribDescriptor> vertexAttribLayout = GetVertexAttribLayout();
		uint32_t vertexStride = CalculateVertexStride(vertexAttribLayout);
		uint32_t vertexCount = static_cast<uint32_t>(GetVertexCount());
		size_t vertexBufferSizeInBytes{ static_cast<size_t>(vertexCount) * vertexStride };

		std::vector<char> vertexBuffer(vertexBufferSizeInBytes);
		char* vb = vertexBuffer.data();
		ConstructMeshVertexBuffer(vb, vertexCount, vertexAttribLayout);
		return vertexBuffer;
	}
	std::vector<char> Mesh::ConstructMeshIndexBuffer() const {
		uint32_t indexCount = static_cast<uint32_t>(GetIndexCount());
		uint32_t indexFormatSize = GetIndexFormatSizeInBytes(indexFormat);
		size_t indexBufferSizeInBytes{ static_cast<size_t>(indexCount) * indexFormatSize };

		std::vector<char> indexBuffer(indexBufferSizeInBytes);
		char* ib = indexBuffer.data();
		ConstructMeshIndexBuffer(ib, indexCount, indexFormat);
		return indexBuffer;
	}

	MeshStat Mesh::GetMeshStat() const {
		MeshStat meshStat{};
		meshStat.vbInfo = GetVertexBufferInfo();
		meshStat.ibInfo = GetIndexBufferInfo();
		meshStat.meshTopology = GetMeshTopology();
		meshStat.attributesMask = GetAttributesMask();
		meshStat.patchVertexCount = GetPatchVertexCount();
		meshStat.isDynamic = IsMeshDynamic();
		meshStat.isTessellated = IsMeshTessellated();
		meshStat.cullBackFaces = CullBackFaces();
		return meshStat;
	}
	VertexBufferInfo Mesh::GetVertexBufferInfo() const {
		VertexBufferInfo vbInfo{};
		vbInfo.vertexAttribLayout = GetVertexAttribLayout();
		vbInfo.vertexCount = static_cast<uint32_t>(GetVertexCount());
		vbInfo.vertexStride = CalculateVertexStride(vbInfo.vertexAttribLayout);
		return vbInfo;
	}
	IndexBufferInfo Mesh::GetIndexBufferInfo() const {
		IndexBufferInfo ibInfo{};
		ibInfo.indexCount = static_cast<uint32_t>(GetIndexCount());
		ibInfo.indexFormat = GetIndexFormat();
		return ibInfo;
	}

	void Mesh::SetVertexAttribDescriptor(const VertexAttribDescriptor& vertAttribDesc) {
		vertAttribLayout.insert({vertAttribDesc.channel, vertAttribDesc});
		ResizeVertexAttribArrays();
		OnVertexDataUpdated();
	}
	void Mesh::ResetVertexAttribDescriptor(VertexAttribChannel channel) {
		if (channel == VertexAttribChannel::POSITION) {
			// TODO: report to the user that the position vertex attribute is mandatory and cannot be removed!
			return;
		}
		vertAttribLayout.erase(channel);
		ResizeVertexAttribArrays();
		OnVertexDataUpdated();
	}
	bool Mesh::GetVertexAttribDescriptor(VertexAttribDescriptor& attribDesc, VertexAttribChannel channel) const {
		auto searchRes = vertAttribLayout.find(channel);
		if (searchRes == vertAttribLayout.end()) {
			return false;
		}
		attribDesc = searchRes->second;
		return true;
	}

	void Mesh::SetVertexAttribLayout(const std::vector<VertexAttribDescriptor>& vertAttribLayout) {
		SetVertexAttribLayoutMap(vertAttribLayout);
		ResizeVertexAttribArrays();
		OnVertexDataUpdated();
	}
	std::vector<VertexAttribDescriptor> Mesh::GetVertexAttribLayout() const {
		std::vector<VertexAttribDescriptor> vertexAttribLayout;
		VertexAttribDescriptor attribDesc{};
		uint32_t offset{0};
		// Position vertex attribute
		GetVertexAttribDescriptor(attribDesc, VertexAttribChannel::POSITION);
		attribDesc.offset = offset;
		vertexAttribLayout.push_back(attribDesc);
		offset += attribDesc.GetVertexAttribSize();
		// Normal vertex attribute
		if (GetVertexAttribDescriptor(attribDesc, VertexAttribChannel::NORMAL)) {
			attribDesc.offset = offset;
			vertexAttribLayout.push_back(attribDesc);
			offset += attribDesc.GetVertexAttribSize();
		}
		// Tangent vertex attribute
		if (GetVertexAttribDescriptor(attribDesc, VertexAttribChannel::TANGENT)) {
			attribDesc.offset = offset;
			vertexAttribLayout.push_back(attribDesc);
			offset += attribDesc.GetVertexAttribSize();
		}
		// Color vertex attribute
		if (GetVertexAttribDescriptor(attribDesc, VertexAttribChannel::COLOR)) {
			attribDesc.offset = offset;
			vertexAttribLayout.push_back(attribDesc);
			offset += attribDesc.GetVertexAttribSize();
		}
		// Uv vertex attribute
		if (GetVertexAttribDescriptor(attribDesc, VertexAttribChannel::UV0)) {
			attribDesc.offset = offset;
			vertexAttribLayout.push_back(attribDesc);
			offset += attribDesc.GetVertexAttribSize();
		}
		return vertexAttribLayout;
	}

	VertexAttribDescriptor Mesh::GetDefaultPositionVertexAttribDescriptor() const {
		VertexAttribDescriptor posAttribDesc{
			3, 0,
			VertexAttribChannel::POSITION,
			VertexAttribFormat::FLOAT32,
		};
		return posAttribDesc;
	}
	VertexAttribDescriptor Mesh::GetDefaultNormalVertexAttribDescriptor() const {
		VertexAttribDescriptor normAttribDesc{
			3, 0,
			VertexAttribChannel::NORMAL,
			VertexAttribFormat::FLOAT32,
		};
		return normAttribDesc;
	}
	VertexAttribDescriptor Mesh::GetDefaultTangentVertexAttribDescriptor() const {
		VertexAttribDescriptor tangentAttribDesc{
			3,
			0,
			VertexAttribChannel::TANGENT,
			VertexAttribFormat::FLOAT32,
		};
		return tangentAttribDesc;
	}
	VertexAttribDescriptor Mesh::GetDefaultColorVertexAttribDescriptor() const {
		VertexAttribDescriptor tangentAttribDesc{
			3,
			0,
			VertexAttribChannel::COLOR,
			VertexAttribFormat::FLOAT32,
		};
		return tangentAttribDesc;
	}
	VertexAttribDescriptor Mesh::GetDefaultUvVertexAttribDescriptor() const {
		VertexAttribDescriptor uvAttribDesc{
			2,
			0,
			VertexAttribChannel::UV0,
			VertexAttribFormat::FLOAT32,
		};
		return uvAttribDesc;
	}

	uint32_t Mesh::GetAttributesMask() const {
		uint32_t attributeMask{0};
		attributeMask |= 1; // POSITION
		attributeMask |= HasNormals() ? 1 << 1 : 0; // NORMAL
		attributeMask |= HasTangents() ? 1 << 2 : 0; // TANGENT
		attributeMask |= HasColors() ? 1 << 3 : 0; // COLOR
		attributeMask |= HasUvs() ? 1 << 4 : 0; // UV
		return attributeMask;
	}

	const numa::AABB& Mesh::GetObjectAABB() const {
		return this->objectAABB;
	}

	numa::AABB Mesh::ComputeWorldAABBApproximate(const numa::Mat4& world) const {
		// Stackoverflow: https://gamedev.stackexchange.com/a/162824/160940
		// glm::vec3 min = glm::vec3{1.0f, 1.0f, 1.0f} * std::numeric_limits<float>().max();
		// glm::vec3 max = glm::vec3{1.0f, 1.0f, 1.0f} * std::numeric_limits<float>().min();

		numa::Vec3 min = numa::Vec3{std::numeric_limits<float>().infinity()};
		numa::Vec3 max = numa::Vec3{-std::numeric_limits<float>().infinity()};

		std::array<numa::Vec3, 8> localAABBVertexPositions = this->objectAABB.GetAABBVertexPositions();
		for (uint32_t vert = 0; vert < localAABBVertexPositions.size(); vert++) {
			numa::Vec4 worldSpaceVert = world * numa::Vec4{localAABBVertexPositions[vert], 1.0f};
			min = numa::Min(numa::Vec3{worldSpaceVert}, min);
			max = numa::Max(numa::Vec3{worldSpaceVert}, max);
		}

		// Apply padding
		// 
		// Padding is already applied to the object AABB, so
		// there's no need to do it again.
		// 
		// ApplyLocalAABBPadding(min, max);

		numa::AABB worldAABB{};
		worldAABB.InitializeFromMinMax(min, max);
		return worldAABB;
	}
	numa::AABB Mesh::ComputeWorldAABBPrecise(const numa::Mat4& world) const {
		// Stackoverflow: https://gamedev.stackexchange.com/a/162824/160940
		// glm::vec3 min = glm::vec3{ 1.0f, 1.0f, 1.0f } * std::numeric_limits<float>().max();
		// glm::vec3 max = glm::vec3{ 1.0f, 1.0f, 1.0f } * std::numeric_limits<float>().min();

		numa::Vec3 min = numa::Vec3{std::numeric_limits<float>().infinity()};
		numa::Vec3 max = numa::Vec3{-std::numeric_limits<float>().infinity()};

		for (uint32_t vert = 0; vert < positions.size(); vert++) {
			numa::Vec4 worldSpaceVert = world * numa::Vec4{positions[vert], 1.0f};
			min = numa::Min(numa::Vec3{worldSpaceVert}, min);
			max = numa::Max(numa::Vec3{worldSpaceVert}, max);
		}

		// Apply padding
		// 
		// Padding is already applied to the object AABB, so
		// there's no need to do it again.
		// 
		// ApplyLocalAABBPadding(min, max);

		numa::AABB worldAABB{};
		worldAABB.InitializeFromMinMax(min, max);
		return worldAABB;
	}

	numa::OBB Mesh::ComputeWorldOBB(const numa::Mat4& world) const {
		numa::OBB worldOBB{};
		worldOBB.radius = this->objectAABB.radius;
		worldOBB.center = world * numa::Vec4{this->objectAABB.center, 1.0f};
		worldOBB.rotMat = world;
		return worldOBB;
	}

	void Mesh::SetObjectAABBPadding(float uniformPadding) {
		SetObjectAABBPadding(numa::Vec3{uniformPadding});
	}
	void Mesh::SetObjectAABBPadding(const numa::Vec3& padding) {
		this->aabbPadding = padding;
		numa::Vec3 min = this->objectAABB.MinPoint();
		numa::Vec3 max = this->objectAABB.MaxPoint();
		ApplyObjectAABBPadding(min, max);
		this->objectAABB.InitializeFromMinMax(min, max);
		SendMeshChangedEventNotifications();
	}

	void Mesh::SetVertexCount(size_t vertexCount) {
		this->positions.resize(vertexCount);
		ResizeVertexAttribArrays();
		OnVertexDataUpdated();
	}
	size_t Mesh::GetVertexCount() const {
		return positions.size();
	}

	void Mesh::SetIndexCount(size_t indexCount) {
		this->indices.resize(indexCount);
		OnIndexDataUpdated();
	}
	size_t Mesh::GetIndexCount() const {
		return indices.size();
	}

	uint32_t Mesh::GetVertexStride() const {
		return CalculateVertexStride(GetVertexAttribLayout());
	}

	void Mesh::SetMeshTopology(MeshTopology meshTopology) {
		this->meshTopology = meshTopology;
		OnMeshSettingsUpdated();
	}
	MeshTopology Mesh::GetMeshTopology() const {
		return this->meshTopology;
	}

	void Mesh::SetCullBackFaceState(bool cullBackFaces) {
		this->cullBackFaces = cullBackFaces;
		OnMeshSettingsUpdated();
	}
	bool Mesh::CullBackFaces() const {
		return cullBackFaces;
	}

	void Mesh::SetIndexFormat(IndexFormat format) {
		this->indexFormat = format;
		OnIndexDataUpdated();
	}
	IndexFormat Mesh::GetIndexFormat() const {
		return indexFormat;
	}

	uint32_t Mesh::GetMeshId() const {
		return meshId;
	}

	void Mesh::MakeDynamic() {
		this->dynamic = true;
		OnMeshDataUpdated();
	}
	void Mesh::MakeStatic() {
		this->dynamic = false;
		OnMeshDataUpdated();
	}

	bool Mesh::IsDynamic() const {
		return dynamic;
	}

	void Mesh::SendMeshChangedEventNotifications() const {
		meshChangedCallbackStorage.Invoke();
	}

	void Mesh::OnVertexDataUpdated() const {
		UpdateGpuMeshVertexData();
		SendMeshChangedEventNotifications();
	}
	void Mesh::OnIndexDataUpdated() const {
		UpdateGpuMeshIndexData();
		SendMeshChangedEventNotifications();
	}
	void Mesh::OnMeshSettingsUpdated() const {
		UpdateGpuMeshSettings();
		SendMeshChangedEventNotifications();
	}
	void Mesh::OnMeshDataUpdated() const {
		UpdateGpuMeshSettings();
		UpdateGpuMeshVertexData();
		UpdateGpuMeshIndexData();
		SendMeshChangedEventNotifications();
	}

	void Mesh::ResizeVertexAttribArrays() {
		uint32_t normalVertexAttribArraySize = HasNormals() ? static_cast<uint32_t>(positions.size()) : 0;
		ResizeNormalVertexAttribArray(normalVertexAttribArraySize);

		uint32_t tangentVertexAttribArraySize = HasTangents() ? static_cast<uint32_t>(positions.size()) : 0;
		ResizeTangentVertexAttribArray(tangentVertexAttribArraySize);

		uint32_t colorVertexAttribArraySize = HasColors() ? static_cast<uint32_t>(positions.size()) : 0;
		ResizeColorVertexAttribArray(colorVertexAttribArraySize);

		uint32_t uvVertexAttribArraySize = HasUvs() ? static_cast<uint32_t>(positions.size()) : 0;
		ResizeUvVertexAttribArray(uvVertexAttribArraySize);
	}

	void Mesh::SetVertexAttribLayoutMap(const std::vector<VertexAttribDescriptor>& vertAttribLayout) {
		this->vertAttribLayout.clear();
		for (const VertexAttribDescriptor& vertAttrib : vertAttribLayout) {
			this->vertAttribLayout.insert({vertAttrib.channel, vertAttrib});
		}
		// Ensure that the position vertex attribute exists.
		VertexAttribDescriptor vertAttribDesc{};
		if (!GetVertexAttribDescriptor(vertAttribDesc, VertexAttribChannel::POSITION)) {
			// TODO: report to the user that the layout they provided lacks the POSITION vertex attribute,
			//       and use the default vertex attribute descriptor.
			vertAttribDesc = GetDefaultPositionVertexAttribDescriptor();
			this->vertAttribLayout.insert({VertexAttribChannel::POSITION, vertAttribDesc});
		}
	}

	void Mesh::ResizeNormalVertexAttribArray(uint32_t size) {
		normals.resize(size);
	}
	void Mesh::ResizeTangentVertexAttribArray(uint32_t size) {
		tangents.resize(size);
	}
	void Mesh::ResizeColorVertexAttribArray(uint32_t size) {
		colors.resize(size);
	}
	void Mesh::ResizeUvVertexAttribArray(uint32_t size) {
		uvs.resize(size);
	}

	bool Mesh::IndicesOutOfBound(std::vector<uint32_t>* outOfBoundIndices) {
		bool outOfBound{false};
		uint32_t biggestPossibleIndex = static_cast<uint32_t>(positions.size() - 1);
		for (uint32_t index : this->indices) {
			if (index > biggestPossibleIndex) {
				outOfBound = true;
				if (outOfBoundIndices != nullptr) {
					outOfBoundIndices->push_back(index);
				}
			}
		}
		return outOfBound;
	}
	bool Mesh::IndicesIncomplete(std::vector<uint32_t>* incompleteIndices) {
		// Takes the currently set mesh topology into account
		uint32_t multiplicity = GetIndexMultiplicity(this->meshTopology);
		if (multiplicity == 0) {
			// Non-indexed mesh, we're not even supposed to have indices
			return false;
		}
		// 
		// 23 : 3 = 7 -> 'quotient'
		// 23 % 3 = 2 -> 'remainder'
		// 
		// 23 is called 'dividend'
		//  3 is called 'divisor'
		//  7 is called 'quotient'
		//  2 is called 'remainder'
		// 
		// The 'smallest closest number' that when divided by the same 'divisor' (3 in our case)
		// gives the same 'quotient' and leaves no 'remainder' can be found as:
		// dividend - remainder, which in our case is 23 - 2 = 21; 21 : 3 = 7 with remainder = 0
		// So the number we were looking for is 21.
		// 
		// The 'next closest number' that when divided by the same 'divisor' leaves no 'remainder' can be found as:
		// dividend + (divisor - remainder), which in our case is 23 + (3 - 2) = 24; 24 : 3 = 8, with remainder = 0
		// So the number we were looking for is 24.
		// 
		// We're looking for the 'smallest closest number', which tells us where the last complete primitive ends.
		// 
		uint32_t remainder = this->indices.size() % multiplicity;
		if (remainder == 0) {
			// The indices array is complete for the current mesh topology
			return false;
		}
		if (incompleteIndices != nullptr) {
			uint32_t smallestClosestNumber = static_cast<uint32_t>(this->indices.size() - remainder);
			auto firstIncompleteIndexIter = this->indices.end() - remainder;
			incompleteIndices->assign(firstIncompleteIndexIter, this->indices.end());
		}
		return true;
	}

	void Mesh::ReportOutOfBoundIndices(const std::vector<uint32_t>& outOfBoundIndices) {
		// assert(outOfBoundIndices.size() == 0 && "Out of bound indices detected!");
		// Logger::Info("Out of bound indices detected!");
	}
	void Mesh::ReportIncompleteIndices(const std::vector<uint32_t>& incompleteIndices) {
		// assert(incompleteIndices.size() == 0 && "Incomplete indices detected!");
		// Logger::Info("Incomplete indices detected!");
	}

	void Mesh::ComputeObjectAABB() {
		// Stackoverflow: https://gamedev.stackexchange.com/a/162824/160940
		// numa::Vec3 min = numa::Vec3{1.0f, 1.0f, 1.0f} * std::numeric_limits<float>().max();
		// numa::Vec3 max = numa::Vec3{1.0f, 1.0f, 1.0f} * std::numeric_limits<float>().min();

		numa::Vec3 min = numa::Vec3{std::numeric_limits<float>().infinity()};
		numa::Vec3 max = numa::Vec3{-std::numeric_limits<float>().infinity()};
		for (uint32_t vert = 0; vert < positions.size(); vert++) {
			min = numa::Min(positions[vert], min);
			max = numa::Max(positions[vert], max);
		}
		// Apply padding
		ApplyObjectAABBPadding(min, max);
		this->objectAABB.InitializeFromMinMax(min, max);
	}
	void Mesh::ApplyObjectAABBPadding(numa::Vec3& min, numa::Vec3& max) const {
		min -= aabbPadding;
		max += aabbPadding;
	}

	bool Mesh::FormatConversionPossible(VertexAttribFormat srcFormat,
		                                VertexAttribFormat destFormat) const {
		// The following are the types available:
		// INT32
		// INT16
		// INT8
		// UINT32
		// UINT16
		// UINT8
		// FLOAT32
		// 
		// We have a couple of choices in what checks to do here.
		// 1. We can allow everything, even conversions with some loss of information.
		//    This is, for example, when we convert FLOAT32 to INT32, or UINT16 to UINT8.
		//    We could simply cast the types, accept the possible loss of data, and be done with it.
		// 2. Or we could do some sophisticated checks and warn the user about potential problems.
		// 
		// Right now, the default attribute format is FLOAT32 which can store any of the other types, sort of.
		// I'm pretty sure that the maximum integer that FLOAT32 supports
		// is much smaller than what INT32 or UINT32 support.
		// I'm not sure where I would want to use an integer for a vertex attribute,
		// so we're going to postpone this problem until a use case comes up.
		return true;
	}
	bool Mesh::DimensionalityConversionPossible(const VertexAttribDescriptor& srcDesc,
		                                        const VertexAttribDescriptor& destDesc) const {
		// Similar to format conversion, I simply don't have any use case here, so I can't say what's the best approach.
		// For example, a position vertex attribute asks for 3 components by default. What if the user provided
		// a vertex attribute layout where positions are 4-component vectors? What are we going to do in this case?
		// Well, we will copy as many components as we have and assume that
		// the code doing the copy will supply the fourth component for us.
		// Maybe we could somehow parametrize how vertex attributes are stored, but for now this is not a priority.
		// Perhaps, I could simply store them in a typeless void* array of data. Then the layout would pretty much
		// tell us what data is stored there and how it should be interpreted. But how would we supply the data?
		// Say I wanted to add more normals or tangents. What would the interface accepting the data be?
		// Need more time to think about this problem. For now, we simply always return 'true'.
		return true;
	}
	bool Mesh::WritePossible(const VertexAttribDescriptor& srcDesc,
		                     const VertexAttribDescriptor& destDesc) const {
		bool formatConversionPossible = FormatConversionPossible(srcDesc.format, destDesc.format);
		bool destSizeFitsSrcSize = DimensionalityConversionPossible(srcDesc, destDesc);
		return formatConversionPossible && destSizeFitsSrcSize;
	}

	void Mesh::UpdateGpuMeshVertexData() const {
		GetCurrentGpuApiCtx()->OnMeshVertexBufferUpdate(this);
	}
	void Mesh::UpdateGpuMeshIndexData() const {
		GetCurrentGpuApiCtx()->OnMeshIndexBufferUpdate(this);
	}

	void Mesh::ConstructMeshVertexBuffer(char* vb, uint32_t vertexCount,
		                                 const std::vector<VertexAttribDescriptor>& layout) const {
		for (uint32_t vert = 0; vert < vertexCount; vert++) {
			for (const VertexAttribDescriptor& vertexAttrib : layout) {
				WriteSrcAttribToDstBuffer(vertexAttrib, vb, vert);
				vb = vb + vertexAttrib.GetVertexAttribSize();
			}
		}
	}
	void Mesh::ConstructMeshIndexBuffer(char* ib, uint32_t indexCount,
		                                IndexFormat ibFormat) const {
		uint32_t indexFormatSize = GetIndexFormatSizeInBytes(ibFormat);
		for (uint32_t idx = 0; idx < indexCount; idx++) {
			WriteSrcIndexToDstBuffer(ibFormat, ib, idx);
			ib = ib + indexFormatSize;
		}
	}

	void Mesh::UpdateGpuMeshSettings() const {
		GetCurrentGpuApiCtx()->OnMeshSettingsChange(this);
	}

	void Mesh::WriteSrcAttribToDstBuffer(const VertexAttribDescriptor& dstAttribDesc, char* dstBuffer, uint32_t vert) const {
		// TODO
	}
	void Mesh::WriteSrcAttribToDstBuffer(const VertexAttribDescriptor& srcAttribDesc, const char* srcBuffer,
		                                 const VertexAttribDescriptor& dstAttribDesc, char* dstBuffer) const {
		// TODO
	}

	void Mesh::WriteSrcIndexToDstBuffer(IndexFormat indexFormat, char* dstBuffer, uint32_t index) const {
		// TODO
	}

	void Mesh::SetInternalVertexAttribArrayData(const void* src, uint32_t vertexCount,
		                                        const std::vector<VertexAttribDescriptor>& layout) {
		const char* vb = reinterpret_cast<const char*>(src);
		for (uint32_t vert = 0; vert < vertexCount; vert++) {
			for (const VertexAttribDescriptor& attrib : layout) {
				WriteSrcDataToAttribArray(attrib, vert, vb);
				vb = vb + attrib.GetVertexAttribSize();
			}
		}
	}

	void Mesh::WriteSrcDataToAttribArray(const VertexAttribDescriptor& srcAttribDesc,
			                             uint32_t vert, const char* srcBuffer) {
		switch (srcAttribDesc.channel) {
			case VertexAttribChannel::POSITION: {
				VertexAttribDescriptor dstAttribDesc = GetDefaultPositionVertexAttribDescriptor();
				if (WritePossible(srcAttribDesc, dstAttribDesc)) {
					numa::Vec3* dstDataPtr = reinterpret_cast<numa::Vec3*>(positions.data());
					dstDataPtr += vert;
					char* dstBuffer = reinterpret_cast<char*>(dstDataPtr);
					WriteSrcDataToAttribArray(srcAttribDesc, srcBuffer, dstAttribDesc, dstBuffer);
				}
				break;
			}
			case VertexAttribChannel::NORMAL: {
				VertexAttribDescriptor dstAttribDesc = GetDefaultNormalVertexAttribDescriptor();
				if (WritePossible(srcAttribDesc, dstAttribDesc)) {
					numa::Vec3* dstDataPtr = reinterpret_cast<numa::Vec3*>(normals.data());
					dstDataPtr += vert;
					char* dstBuffer = reinterpret_cast<char*>(dstDataPtr);
					WriteSrcDataToAttribArray(srcAttribDesc, srcBuffer, dstAttribDesc, dstBuffer);
				}
				break;
			}
			case VertexAttribChannel::TANGENT: {
				VertexAttribDescriptor dstAttribDesc = GetDefaultTangentVertexAttribDescriptor();
				if (WritePossible(srcAttribDesc, dstAttribDesc)) {
					numa::Vec3* dstDataPtr = reinterpret_cast<numa::Vec3*>(tangents.data());
					dstDataPtr += vert;
					char* dstBuffer = reinterpret_cast<char*>(dstDataPtr);
					WriteSrcDataToAttribArray(srcAttribDesc, srcBuffer, dstAttribDesc, dstBuffer);
				}
				break;
			}
			case VertexAttribChannel::COLOR: {
				VertexAttribDescriptor dstAttribDesc = GetDefaultColorVertexAttribDescriptor();
				if (WritePossible(srcAttribDesc, dstAttribDesc)) {
					numa::Vec3* dstDataPtr = reinterpret_cast<numa::Vec3*>(colors.data());
					dstDataPtr += vert;
					char* dstBuffer = reinterpret_cast<char*>(dstDataPtr);
					WriteSrcDataToAttribArray(srcAttribDesc, srcBuffer, dstAttribDesc, dstBuffer);
				}
				break;
			}
			case VertexAttribChannel::UV0: {
				VertexAttribDescriptor dstAttribDesc = GetDefaultUvVertexAttribDescriptor();
				if (WritePossible(srcAttribDesc, dstAttribDesc)) {
					numa::Vec2* dstDataPtr = reinterpret_cast<numa::Vec2*>(uvs.data());
					dstDataPtr += vert;
					char* dstBuffer = reinterpret_cast<char*>(dstDataPtr);
					WriteSrcDataToAttribArray(srcAttribDesc, srcBuffer, dstAttribDesc, dstBuffer);
				}
				break;
			}
			default: {
				assert(true && "Unidentified vertex attribute type provided!");
			}
		}
	}
	void Mesh::WriteSrcDataToAttribArray(const VertexAttribDescriptor& srcAttribDesc, const char* srcBuffer,
		                                 const VertexAttribDescriptor& dstAttribDesc, char* dstBuffer) {
		// All of our destination attributes are floats, they only differ in dimension.
		float* dstAttribBuffer = reinterpret_cast<float*>(dstBuffer);
		// We take the minimum dimension between the one offered in 'srcAttribDesc',
		// and the one requested by the user in 'dstAttribDesc'.
		uint32_t minDim = std::min(srcAttribDesc.dimension, dstAttribDesc.dimension);
		// Source vertex attributes need to be converted to the type of the destination attributes (floats in our case).
		switch (srcAttribDesc.format) {
			case VertexAttribFormat::FLOAT32: {
				const float* src = reinterpret_cast<const float*>(srcBuffer);
				float* dst = dstAttribBuffer;
				for (uint32_t componentIdx = 0; componentIdx < minDim; componentIdx++) {
					WriteSrcToDst(src, dst);
					src++;
					dst++;
				}
				break;
			}
			case VertexAttribFormat::UINT32: {
				for (uint32_t componentIdx = 0; componentIdx < minDim; componentIdx++) {
					const uint32_t* src = reinterpret_cast<const uint32_t*>(srcBuffer);
					float* dst = dstAttribBuffer;
					for (uint32_t componentIdx = 0; componentIdx < minDim; componentIdx++) {
						WriteSrcToDst(src, dst);
						src++;
						dst++;
					}
				}
				break;
			}
			case VertexAttribFormat::UINT16: {
				for (uint32_t componentIdx = 0; componentIdx < minDim; componentIdx++) {
					const uint16_t* src = reinterpret_cast<const uint16_t*>(srcBuffer);
					float* dst = dstAttribBuffer;
					for (uint32_t componentIdx = 0; componentIdx < minDim; componentIdx++) {
						WriteSrcToDst(src, dst);
						src++;
						dst++;
					}
				}
				break;
			}
			case VertexAttribFormat::UINT8: {
				for (uint32_t componentIdx = 0; componentIdx < minDim; componentIdx++) {
					const uint8_t* src = reinterpret_cast<const uint8_t*>(srcBuffer);
					float* dst = dstAttribBuffer;
					for (uint32_t componentIdx = 0; componentIdx < minDim; componentIdx++) {
						WriteSrcToDst(src, dst);
						src++;
						dst++;
					}
				}
				break;
			}
			case VertexAttribFormat::INT32: {
				for (uint32_t componentIdx = 0; componentIdx < minDim; componentIdx++) {
					const int32_t* src = reinterpret_cast<const int32_t*>(srcBuffer);
					float* dst = dstAttribBuffer;
					for (uint32_t componentIdx = 0; componentIdx < minDim; componentIdx++) {
						WriteSrcToDst(src, dst);
						src++;
						dst++;
					}
				}
				break;
			}
			case VertexAttribFormat::INT16: {
				for (uint32_t componentIdx = 0; componentIdx < minDim; componentIdx++) {
					const int16_t* src = reinterpret_cast<const int16_t*>(srcBuffer);
					float* dst = dstAttribBuffer;
					for (uint32_t componentIdx = 0; componentIdx < minDim; componentIdx++) {
						WriteSrcToDst(src, dst);
						src++;
						dst++;
					}
				}
				break;
			}
			case VertexAttribFormat::INT8: {
				for (uint32_t componentIdx = 0; componentIdx < minDim; componentIdx++) {
					const int8_t* src = reinterpret_cast<const int8_t*>(srcBuffer);
					float* dst = dstAttribBuffer;
					for (uint32_t componentIdx = 0; componentIdx < minDim; componentIdx++) {
						WriteSrcToDst(src, dst);
						src++;
						dst++;
					}
				}
				break;
			}
			default: {
				assert(true && "Unknown vertex attribute format provided!");
			}
		}
	}

	void Mesh::SendGpuMeshVertexBufferData(const void* vb) const {
		// TODO
		// VertexBufferInfo vbInfo = GetVertexBufferInfo();
		// coreGlobal.gpuResourceDatabase->UpdateMeshVertexBufferGpuResource(GetMeshId(), vb, vbInfo);
	}
	void Mesh::SendGpuMeshIndexBufferData(const void* ib) const {
		// TODO
		// IndexBufferInfo ibInfo = GetIndexBufferInfo();
		// coreGlobal.gpuResourceDatabase->UpdateMeshIndexBufferGpuResource(GetMeshId(), ib, ibInfo);
	}

}