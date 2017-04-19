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
	for (auto ui : uiElements) {
		ui->render();
	}
}

void MenuScreen::resize(int width, int height)
{
}

void MenuScreen::dispose()
{
	disposeButtons();
	uiElements.clear();
}