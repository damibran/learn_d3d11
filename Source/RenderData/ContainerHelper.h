#pragma once

#include <type_traits>

namespace dmbrn
{
	template <typename Container>
	using data_t = decltype(std::declval<Container>().data());

	template <typename Container, typename = std::void_t<>>
	struct has_data : std::false_type {};

	template <typename Container>
	struct has_data<Container, std::void_t<data_t<Container>>> :
		std::true_type {};
}