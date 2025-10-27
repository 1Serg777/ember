#pragma once

#include "Core/Util.h"

#include <string>

namespace ember {

	class Mesh {
	public:
		Mesh();
		~Mesh();

		CLASS_NO_COPY(Mesh);
		CLASS_DEFAULT_MOVE(Mesh);

		void MakeDynamic();
		void MakeStatic();

		bool IsDynamic() const;

	private:
		std::string name;
		bool dynamic{false};
	};

}