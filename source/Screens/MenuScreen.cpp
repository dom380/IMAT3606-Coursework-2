#include <Screens\MenuScreen.h>

MenuScreen::MenuScreen(shared_ptr<Graphics>& graphics)
{
	this->graphics = graphics;
	UIManager::getInstance()->update();
}

void MenuScreen::show()
{
}

void MenuScreen::update(double dt, double currentTime)
{
}

void MenuScreen::render()
{
	for (auto ui : uiElements) {
		ui->render();
	}
}

void MenuScreen::resize(int width, int height)
{
}

void MenuScreen::dispose()
{
	auto input = Engine::g_pEngine->getInput();
	for (auto element : uiElements)
	{
		input->removeKeyListener(element->getButton());
		input->removeMouseListener(element->getButton());
	}
	uiElements.clear();
}