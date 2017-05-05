/*!
 * @file SoundComponent.cpp
 */

#include "Audio\SoundComponent.h"
#include <iostream>

//Singleton design pattern
SoundComponent* SoundComponent::m_Instance = NULL;


/*
	IMPORTANT FOR SPATIALIZED SOUNDS (Using direction and position)
	A sound can be spatialized only if it has a single channel, i.e. if it's a mono sound.
	Spatialization is disabled for sounds with more channels, since they already explicitly 
	decide how to use the speakers. This is very important to keep in mind.

	*USE AUDACITY INSTALLED IN GAMES LABS AND CONVERT SOUNDS INTO WAV MONO SOUNDS
	https://www.youtube.com/watch?v=ZJ-Nq-os2YI
*/


//!< Default constructor
SoundComponent::SoundComponent()
{
	//m_sSoundFile.push_back("resources/audio/Background Music/menu.wav");

	//for (unsigned int i = 0; i < m_sSoundFile.size(); i++)
	//{
	//	m_SoundBuffer.push_back(new sf::SoundBuffer);
	//	m_SoundBuffer.at(i)->loadFromFile(m_sSoundFile.at(i));
	//	m_Sounds.push_back(new sf::Sound);
	//	m_Sounds.at(i)->setBuffer(*m_SoundBuffer.at(i));
	//}
}

//!< Instance contructor to ensure that only one GameTextureLoader is created
SoundComponent* SoundComponent::Instance()
{
	//if the pointer is pointing to nothing make an instance of a GameSoundLoader
	if (m_Instance == NULL)
	{
		m_Instance = new SoundComponent();
	}

	return m_Instance;
}

//!< Gets the sound of the sent index and returns a sound pointer 
sf::Sound* SoundComponent::GetSound(unsigned int iIndex)
{	
	//if guard to protect potential memory leak
	if (iIndex < m_Sounds.size())
		return m_Sounds.at(iIndex);
	
	else
	{
		std::cout<<"Error, the index being sent has no member in the vector";
		return 0;
	}
}

void SoundComponent::setVolume(float volume, int index)
{
	m_Sounds.at(index)->setVolume(volume);
}

void SoundComponent::setPitch(float pitch, int index)
{
	m_Sounds.at(index)->setPitch(pitch);
}

void SoundComponent::setPosition(float x, float y, float z, int index)
{
	m_Sounds.at(index)->setPosition(x, y, z);
}

void SoundComponent::setLooping(bool b, int index)
{
	m_Sounds.at(index)->setLoop(b);
}

bool SoundComponent::isPlaying(int index)
{
	return m_Sounds.at(index)->getStatus() == sf::Sound::Status::Playing;
}

sf::SoundSource::Status SoundComponent::getStatus(int index)
{
	return m_Sounds.at(index)->getStatus();
}

void SoundComponent::pause(int index)
{
	m_Sounds.at(index)->pause();
}

void SoundComponent::stop(int index)
{
	m_Sounds.at(index)->stop();
}

void SoundComponent::play(int index)
{
	m_Sounds.at(index)->play();
}

void SoundComponent::setAttenuation(int attenuation, int index)
{
	m_Sounds.at(index)->setAttenuation((float)attenuation);

	/*
	MinDistance   is the sound's minimum distance, set with setMinDistance
	Attenuation   is the sound's attenuation, set with setAttenuation
	Distance      is the distance between the sound and the listener
	Volume factor is the calculated factor, in range [0 .. 1], that will be applied to the sound's volume

	Volume factor = MinDistance / (MinDistance + Attenuation * (max(Distance, MinDistance) - MinDistance))
	*/
}

void SoundComponent::setRelativeToListener(bool b, int index)
{
	m_Sounds.at(index)->setRelativeToListener(b);
}

int SoundComponent::loadSound(std::string pathname)
{
	/*
		Loads sounds - need to test this if it works
	*/
	for (int i = 0; i < m_sSoundFile.size(); ++i)
	{
		if (m_sSoundFile.at(i) == pathname) return i; //If we've already loaded the sound don't load it again.
	}

	int indexBuffer = static_cast<int>(m_SoundBuffer.size());
	int indexFile = static_cast<int>(m_sSoundFile.size());
	int indexSound = static_cast<int>(m_Sounds.size());

	m_sSoundFile.push_back(pathname);

	m_SoundBuffer.push_back(new sf::SoundBuffer);
	m_SoundBuffer.at(indexBuffer)->loadFromFile(m_sSoundFile.at(indexFile));

	m_Sounds.push_back(new sf::Sound);
	m_Sounds.at(indexSound)->setBuffer(*m_SoundBuffer.at(indexBuffer));
	return indexSound;
}

void SoundComponent::destroySounds(int index)
{
	/*
		Another source of error is when you try to create a huge number of sounds. SFML internally has a limit; 
		it can vary depending on the OS, but you should never exceed 256. This limit is the number of sf::Sound and
		sf::Music instances that can exist simultaneously. A good way to stay below the limit is to destroy (or recycle)
		unused sounds when they are no longer needed. This only applies if you have to manage
		a really large amount of sounds and music, of course.
	*/
	
	/*
	 TO-DO
	*/
}