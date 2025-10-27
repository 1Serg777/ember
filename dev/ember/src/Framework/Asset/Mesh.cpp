#include "Framework/Asset/Mesh.h"

#include "GpuApi/GpuApiCtx.h"

namespace ember {

	Mesh::Mesh() {
	}
	Mesh::~Mesh() {
	}

	void Mesh::MakeDynamic() {
		this->dynamic = true;

		// coreGlobal.gpuResourceDatabase->DeleteMeshGpuResource(meshId);
		// coreGlobal.gpuResourceDatabase->CreateMeshGpuResource(meshId);

		// OnMeshDataUpdated();
	}
	void Mesh::MakeStatic() {
		this->dynamic = false;

		// coreGlobal.gpuResourceDatabase->DeleteMeshGpuResource(meshId);
		// coreGlobal.gpuResourceDatabase->CreateMeshGpuResource(meshId);

		// OnMeshDataUpdated();
	}

	bool Mesh::IsDynamic() const {
		return dynamic;
	}

}