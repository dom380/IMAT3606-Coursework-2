#include <Screens\MenuScreen.h>

MenuScreen::MenuScreen(shared_ptr<Graphics>& graphics)
{
	this->graphics = graphics;
	UIManager::getInstance()->update();
}

void MenuScreen::update(double dt)
{
}

void MenuScreen::render()
{
	for (auto ui : uiElements) {
		ui->render();
	}
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
	disposeButtons();
	uiElements.clear();
}