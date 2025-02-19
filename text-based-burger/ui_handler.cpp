#include "ui_handler.h"
#include "json.hpp"
#include <fstream> // Include this header to use ifstream

using namespace std;
using json = nlohmann::json; // for convenience

UIHandler::UIHandler(std::string filename, int scrn_w, int scrn_h)
	: error_log(), root(error_log) {
	// Set error log to empty
	error_log = ErrorReporter();

	// Initialize the screen to all 0s
	screen = vector<vector<uint32_t>>(scrn_h, vector<uint32_t>(scrn_w, 0));

	// Everything is stored in a json file

	// Load the json file
	ifstream f(filename);
	if (!f.is_open()) {
		throw runtime_error("Could not open file " + filename);
	}
	json data = json::parse(f);

	// Everything is in an array names "root". Those are the children of the root component
	json root_children = data["root"];

	for (json child : root_children) {
		root.contains(UIComponent(child, error_log));
	}

	// Set the root component
}

void UIHandler::update(update_data data) {
	// Update the root component
	vector<UIComponent> leaves = iterate_leaves(root);
	for (UIComponent leaf : leaves) {
		bool should_rerender = leaf.update(data);
		if (should_rerender) {
			leaf.render(screen);
		}
	}
}

void UIHandler::rerender_all() {
	// Rerender all components
	vector<UIComponent> leaves = iterate_leaves(root);
	for (UIComponent leaf : leaves) {
		leaf.render(screen);
	}
}

std::vector<std::vector<uint32_t>> UIHandler::get_screen() {
	return screen;
}