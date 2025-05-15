#pragma once

#include <vec2.hpp>

#include <cstdint>
#include <type_traits>

namespace ember {

	template<typename T,
		std::enable_if_t<std::is_floating_point_v<T> || std::is_integral_v<T>, bool> = true>
	struct Region2D {
		constexpr Region2D() = default;
		constexpr Region2D(const glm::vec<2, T>& min, const glm::vec<2, T>& max)
			: min(min), max(max) {}
		constexpr Region2D(T width, T height)
			: min(T(0), T(0)), max(width, height) {}

		inline double AspectRatio() const {
			return static_cast<double>(Width()) / static_cast<double>(Height());
		}

		inline T Width() const {
			return max.x - min.x;
		}
		inline T Height() const {
			return max.y - min.y;
		}

		template <typename U>
		inline bool CoordinateInside(const glm::vec<2, U>& c) const {
			return ((c.x >= min.x) && (c.x <= max.x)) &&
				((c.y >= min.y) && (c.y <= max.y));
		}

		glm::vec<2, T> min{ T(0), T(0) };
		glm::vec<2, T> max{ T(0), T(0) };
	};

	using Dimensions2D = Region2D<uint32_t>;

}