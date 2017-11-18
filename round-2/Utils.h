#pragma once

#include <chrono>
#include <future>
#include <random>
#include <vector>

#include "Direction.h"
#include "Matrix.h"

namespace evil {

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

template<typename T, typename F>
Matrix<bool> floodFill(const Matrix<T>& m, Pos initial_pos, F f) {
	Matrix<bool> filled(m.rows(), m.cols());

	std::vector<Pos> queue;
	queue.push_back(initial_pos);

	while (!queue.empty()) {
		auto top = queue.back();
		queue.pop_back();

		if (top.row < 0 || top.row >= m.rows()) {
			continue;
		}

		if (top.col < 0 || top.col >= m.cols()) {
			continue;
		}

		if (filled(top.row, top.col)) {
			continue;
		}

		if (!f(top)) {
			continue;
		}

		filled(top.row, top.col) = true;

		queue.push_back(neighbor(top, Direction::kRight));
		queue.push_back(neighbor(top, Direction::kLeft));
		queue.push_back(neighbor(top, Direction::kUp));
		queue.push_back(neighbor(top, Direction::kDown));
	}

	return filled;
}

} // namespace evil
