#pragma once
#include "Model.h"
#include <vector>
#include <utility>


namespace evil {

class Player {
public:
	virtual ~Player() = default;

	/**
	 * Called once after each server response.
	 */
	virtual void update(const Model& model) = 0;

	/**
	 * Called multiple times after `update()`, until returns `true`.
	 */
	virtual bool isReady() const = 0;

	/**
	 * Called once per `update()`, just after `isReady()` returned `true`.
	 */
	virtual Model::Moves getMoves() = 0;
};

} // namespace evil
