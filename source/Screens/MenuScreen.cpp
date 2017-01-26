#include <Screens\MenuScreen.h>

MenuScreen::MenuScreen(shared_ptr<Graphics>& graphics, Engine* engine)
{
	this->engine = engine;
	this->graphics = graphics;
}

void MenuScreen::update(double dt)
{
}

void MenuScreen::render()
{
	for (auto button : buttons) {
		button->render();
	}
	for (auto text : textBoxes) {
		text->render();
	}
}

void MenuScreen::resize(int width, int height)
{
}

void MenuScreen::dispose()
{
	for (auto button : buttons) {
		button.reset();
	}
}

void MenuScreen::addButton(shared_ptr<Button> button)
{
	buttons.push_back(button);
}

void MenuScreen::addTextBox(shared_ptr<TextBox> textbox)
{
	textBoxes.push_back(textbox);
}
