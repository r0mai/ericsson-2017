#pragma once

#include <chrono>
#include <future>

template<class T>
bool isFutureReady(const std::future<T>& future) {
	return future.valid() &&
		future.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready;
}
