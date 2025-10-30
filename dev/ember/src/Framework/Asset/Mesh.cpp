#include "Framework/Asset/Mesh.h"

#include "GpuApi/GpuApiCtx.h"

#include <algorithm>

namespace ember {

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
	}
	Mesh::~Mesh() {
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

	void Mesh::MakeDynamic() {
		this->dynamic = true;

		// coreGlobal.gpuResourceDatabase->DeleteMeshGpuResource(meshId);
		// coreGlobal.gpuResourceDatabase->CreateMeshGpuResource(meshId);

		OnMeshDataUpdated();
	}
	void Mesh::MakeStatic() {
		this->dynamic = false;

		// coreGlobal.gpuResourceDatabase->DeleteMeshGpuResource(meshId);
		// coreGlobal.gpuResourceDatabase->CreateMeshGpuResource(meshId);

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
		// glm::vec3 min = glm::vec3{ 1.0f, 1.0f, 1.0f } * std::numeric_limits<float>().max();
		// glm::vec3 max = glm::vec3{ 1.0f, 1.0f, 1.0f } * std::numeric_limits<float>().min();

		numa::Vec3 min = numa::Vec3{std::numeric_limits<float>().infinity()};
		numa::Vec3 max = numa::Vec3{-std::numeric_limits<float>().infinity()};
		for (uint32_t vert = 0; vert < positions.size(); vert++) {
			min = numa::Min(positions[vert], min);
			max = numa::Max(positions[vert], max);
		}
		// Apply padding
		ApplyLocalAABBPadding(min, max);
		this->objectAABB.InitializeFromMinMax(min, max);
	}
	void Mesh::ApplyLocalAABBPadding(numa::Vec3& min, numa::Vec3& max) const {
		min -= aabbPadding;
		max += aabbPadding;
	}

}