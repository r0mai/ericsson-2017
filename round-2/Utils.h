#pragma once

#include <chrono>
#include <future>

template<class T>
bool IsFutureReady(const std::future<T>& future) {
	return future.valid() &&
		future.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}
