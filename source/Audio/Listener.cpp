#include "Audio\Listener.h"

//Singleton design pattern
Listener* Listener::m_Instance = NULL;

Listener::Listener()
{
	//https://www.sfml-dev.org/tutorials/2.0/audio-spatialization.php
	//Set the default listener
	setGlobalVolume(50);
	listener = new sf::Listener();

}

//!< Instance contructor to ensure that only one Listener is created
Listener* Listener::Instance()
{
	//if the pointer is pointing to nothing make an instance of a Listener
	if (m_Instance == NULL)
	{
		m_Instance = new Listener();
	}

	return m_Instance;
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

