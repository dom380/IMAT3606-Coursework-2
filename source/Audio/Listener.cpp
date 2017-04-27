#include "Audio\Listener.h"

Listener::Listener()
{
	//https://www.sfml-dev.org/tutorials/2.0/audio-spatialization.php
	//Set the default listener
	setGlobalVolume(50);
	listener = new sf::Listener();

}

void Listener::setPosition(int x, int y, int z)
{
	listener->setPosition(x, y, z);
}

void Listener::setDirection(int x, int y, int z)
{
	listener->setDirection(x, y, z);
}

void Listener::setGlobalVolume(int volume)
{
	listener->setGlobalVolume(volume);
}

