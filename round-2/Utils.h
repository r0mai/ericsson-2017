#pragma once

#include <chrono>
#include <future>
#include <random>

template<typename T>
bool isFutureReady(const std::future<T>& future) {
	return future.valid() &&
		future.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready;
}

// inclusive
template<typename T>
int getRandom(T& engine , int low, int high) {
	std::uniform_int_distribution<int> dist(low, high);
	return dist(engine);
}
