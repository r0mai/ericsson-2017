#pragma once

#include <chrono>
#include <future>
#include <random>
#include <vector>
#include <deque>

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

template<typename Func>
Matrix<int> distanceFill(const Pos& origin, const Pos& end, Func fn) {
	Matrix<int> dst_matrix(end.row, end.col, -1);
	std::deque<std::pair<int, Pos>> queue;

	queue.push_back({0, origin});

	while (!queue.empty()) {
		auto dst = queue.front().first;
		auto pos = queue.front().second;
		queue.pop_front();

		if (pos.row < 0 || pos.row >= end.row ||
			pos.col < 0 || pos.col >= end.col)
		{
			continue;
		}

		auto& value = dst_matrix(pos.row, pos.col);
		if (value != -1) {
			continue;
		}

		if (!fn(pos)) {
			continue;
		}

		value = dst;
		queue.push_back({dst + 1, neighbor(pos, Direction::kRight)});
		queue.push_back({dst + 1, neighbor(pos, Direction::kLeft)});
		queue.push_back({dst + 1, neighbor(pos, Direction::kUp)});
		queue.push_back({dst + 1, neighbor(pos, Direction::kDown)});
	}
}


} // namespace evil
