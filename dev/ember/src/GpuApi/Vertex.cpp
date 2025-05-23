#include "GpuApi/Vertex.h"

namespace ember {

	uint32_t GetVertexAttributeFormatSizeInBytes(VertexAttribFormat vertexAttribFormat) {
		switch (vertexAttribFormat) {
			case VertexAttribFormat::FLOAT32:
			case VertexAttribFormat::UINT32:
			case VertexAttribFormat::INT32: {
				uint32_t expectedSize{ 4 };
				bool floatCheck = sizeof(float) == expectedSize;
				bool uintCheck = sizeof(uint32_t) == expectedSize;
				bool intCheck = sizeof(int32_t) == expectedSize;
				assert(floatCheck && uintCheck && intCheck && "Types' sizes don't match!");
				return expectedSize;
			}
			break;
			case VertexAttribFormat::UINT16:
			case VertexAttribFormat::INT16: {
				uint32_t expectedSize{ 2 };
				bool uintCheck = sizeof(uint16_t) == expectedSize;
				bool intCheck = sizeof(int16_t) == expectedSize;
				assert(uintCheck && intCheck && "Types' sizes don't match!");
				return expectedSize;
			}
			break;
			case VertexAttribFormat::UINT8:
			case VertexAttribFormat::INT8: {
				uint32_t expectedSize{ 1 };
				bool uintCheck = sizeof(uint8_t) == expectedSize;
				bool intCheck = sizeof(int8_t) == expectedSize;
				assert(uintCheck && intCheck && "Types' sizes don't match!");
				return expectedSize;
			}
			break;
			default:
				assert(true && "Unknown vertex attribute format provided!");
				return 0;
			break;
		}
	}

	bool IsVertexAttribFormatFloat(VertexAttribFormat vertexAttribFormat) {
		if (vertexAttribFormat == VertexAttribFormat::FLOAT32) {
			return true;
		}
		return false;
	}
	bool IsVertexAttribFormatUint(VertexAttribFormat vertexAttribFormat) {
		if (vertexAttribFormat == VertexAttribFormat::UINT32 ||
			vertexAttribFormat == VertexAttribFormat::UINT16 ||
			vertexAttribFormat == VertexAttribFormat::UINT8) {
			return true;
		}
		return false;
	}
	bool IsVertexAttribFormatInt(VertexAttribFormat vertexAttribFormat) {
		if (vertexAttribFormat == VertexAttribFormat::INT32 ||
			vertexAttribFormat == VertexAttribFormat::INT16 ||
			vertexAttribFormat == VertexAttribFormat::INT8) {
			return true;
		}
		return false;
	}
    
	uint32_t VertexAttribDescriptor::GetVertexAttributeSize() const {
		return dimension * GetVertexAttributeFormatSizeInBytes(format);
	}

	uint32_t CalculateVertexStride(const std::vector<VertexAttribDescriptor>& vertAttribLayout) {
		uint32_t stride{0};
		for (const VertexAttribDescriptor& vertAttribDesc : vertAttribLayout) {
			stride += vertAttribDesc.GetVertexAttributeSize();
		}
		return stride;
	}

	const std::vector<VertexAttribDescriptor> VertexP::attributes {
		{3, offsetof(VertexP,  vertexPosition),    VertexAttribType::POSITION, VertexAttribFormat::FLOAT32}
	};
	const std::vector<VertexAttribDescriptor> VertexPN::attributes {
		{3, offsetof(VertexPN,  vertexPosition),   VertexAttribType::POSITION, VertexAttribFormat::FLOAT32},
		{3, offsetof(VertexPN,  vertexNormal),     VertexAttribType::NORMAL,   VertexAttribFormat::FLOAT32}
	};
	const std::vector<VertexAttribDescriptor> VertexPC::attributes {
		{3, offsetof(VertexPC,  vertexPosition),   VertexAttribType::POSITION, VertexAttribFormat::FLOAT32},
		{3, offsetof(VertexPC,  vertexColor),      VertexAttribType::COLOR,    VertexAttribFormat::FLOAT32}
	};
	const std::vector<VertexAttribDescriptor> VertexPU::attributes {
		{3, offsetof(VertexPU,  vertexPosition),   VertexAttribType::POSITION, VertexAttribFormat::FLOAT32},
		{2, offsetof(VertexPU,  vertexUv),         VertexAttribType::UV,       VertexAttribFormat::FLOAT32}
	};
	const std::vector<VertexAttribDescriptor> VertexPNT::attributes {
		{3, offsetof(VertexPNT,  vertexPosition),  VertexAttribType::POSITION, VertexAttribFormat::FLOAT32},
		{3, offsetof(VertexPNT,  vertexNormal),    VertexAttribType::NORMAL,   VertexAttribFormat::FLOAT32},
		{3, offsetof(VertexPNT,  vertexTangent),   VertexAttribType::TANGENT,  VertexAttribFormat::FLOAT32}
	};
	const std::vector<VertexAttribDescriptor> VertexPNC::attributes {
		{3, offsetof(VertexPNC,  vertexPosition),  VertexAttribType::POSITION, VertexAttribFormat::FLOAT32},
		{3, offsetof(VertexPNC,  vertexNormal),    VertexAttribType::NORMAL,   VertexAttribFormat::FLOAT32},
		{3, offsetof(VertexPNC,  vertexColor),     VertexAttribType::COLOR,    VertexAttribFormat::FLOAT32}
	};
	const std::vector<VertexAttribDescriptor> VertexPNU::attributes {
		{3, offsetof(VertexPNU,  vertexPosition),  VertexAttribType::POSITION, VertexAttribFormat::FLOAT32},
		{3, offsetof(VertexPNU,  vertexNormal),    VertexAttribType::NORMAL,   VertexAttribFormat::FLOAT32},
		{2, offsetof(VertexPNU,  vertexUv),        VertexAttribType::UV,       VertexAttribFormat::FLOAT32}
	};
	const std::vector<VertexAttribDescriptor> VertexPNTU::attributes {
		{3, offsetof(VertexPNTU,  vertexPosition), VertexAttribType::POSITION, VertexAttribFormat::FLOAT32},
		{3, offsetof(VertexPNTU,  vertexNormal),   VertexAttribType::NORMAL,   VertexAttribFormat::FLOAT32},
		{3, offsetof(VertexPNTU,  vertexTangent),  VertexAttribType::TANGENT,  VertexAttribFormat::FLOAT32},
		{2, offsetof(VertexPNTU,  vertexUv),       VertexAttribType::UV,       VertexAttribFormat::FLOAT32}
	};
	const std::vector<VertexAttribDescriptor> VertexPNTCU::attributes {
		{3, offsetof(VertexPNTCU, vertexPosition), VertexAttribType::POSITION, VertexAttribFormat::FLOAT32},
		{3, offsetof(VertexPNTCU, vertexNormal),   VertexAttribType::NORMAL,   VertexAttribFormat::FLOAT32},
		{3, offsetof(VertexPNTCU, vertexTangent),  VertexAttribType::TANGENT,  VertexAttribFormat::FLOAT32},
		{3, offsetof(VertexPNTCU, vertexColor),    VertexAttribType::COLOR,    VertexAttribFormat::FLOAT32},
		{2, offsetof(VertexPNTCU, vertexUv),       VertexAttribType::UV,       VertexAttribFormat::FLOAT32}
	};

	uint32_t GetIndexFormatSizeInBytes(IndexFormat indexFormat) {
		switch (indexFormat) {
			case IndexFormat::UINT32:
				assert(sizeof(uint32_t) == 4 && "uint32_t is not 4 bytes in size!");
				return 4;
			break;
			case IndexFormat::UINT16:
				assert(sizeof(uint16_t) == 2 && "uint16_t is not 2 bytes in size!");
				return 2;
			break;
			case IndexFormat::UINT8:
				assert(sizeof(uint8_t) == 1 && "uint8_t is not 1 byte in size!");
				return 1;
			break;
			default:
				assert(true && "Unknown index format provided!");
				return 0;
			break;
		}
	}

}