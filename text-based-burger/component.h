#pragma once

#include "json.hpp"
using json = nlohmann::json;

#include <vector>
#include <string>


struct update_data {
	int mouse_char_x;
	int mouse_char_y;
	int time;
	bool is_clicking;
};

// Error reporter class
class ErrorReporter {
public:
	// Constructor
	ErrorReporter();
	// Destructor
	~ErrorReporter();
	// Report an error
	void report_error(std::string error);
	// Get the error log
	std::vector<std::string> get_log();
private:
	std::vector<std::string> error_log;
};

// Generates a grid fragment from character, bg and fg color
uint32_t gen_frag(int character, int bg, int fg);

// The root ui component class. All ui components inherit from this class.

class UIComponent {

public:
	// Constructor
	UIComponent(ErrorReporter& the_error_reporter); // Construct an empty component
	UIComponent(json data, ErrorReporter& the_error_reporter); // Construct from json data

	// Destructor
	~UIComponent();

	// Update function, returns true if the component should rerender
	virtual bool update(update_data data);

	// Tell the component to render to the given screen
	// If it runs out of bounds it just clips
	virtual void render(std::vector<std::vector<uint32_t>>&screen);

	// This may be overridden by some components that cannot contain children
	virtual void contains(UIComponent component);

	std::vector<UIComponent> get_children();

	std::string name;

protected:
	// These are all common to all components

	std::pair<int, int> position;

	std::vector<UIComponent> children;

	ErrorReporter& error_reporter;

};

// Function to iterate over every leaf in component tree from some given component
// Returns iterator that returns leaves in bfs order
std::vector<UIComponent> iterate_leaves(UIComponent component);

UIComponent type_selector(json data, ErrorReporter& reporter);


class Container : public UIComponent {
public:
	Container(json data, ErrorReporter& the_error_reporter);
};


// One very important feature: Because keyboards do not have 256 characters, we would need a
// look up table to do that, but because of microsoft's dedication to backwards compatibility,
// the characters can be entered using alt codes, and they are 1:1 to cp437. This means that 
// to interact with labels as a human, you only need to hold alt and type the number on the numpad.
class Label : public UIComponent {
public:
	Label(json data, ErrorReporter& the_error_reporter);

	virtual void contains(UIComponent component) override;

	virtual void render(std::vector<std::vector<uint32_t>>& screen) override;
	
	void update_text(std::string new_text);
	void update_text(std::vector<int> new_text);
private:
	std::vector<int> text;
	int foreground_color;
	int background_color;
};