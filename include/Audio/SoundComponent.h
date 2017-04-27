#pragma once
/*!
 * @file SoundLoader.h
 * Header file containing the sound loader which loads sounds
 */

 /*
 *Audio engine for playing 3D sounds
 *Author: Natalie Downes - p12194892
 */
#ifndef SOUNDCOMPONENT_H
#define SOUNDCOMPONENT_H
#include <SFML\Audio.hpp>


/*! \class SoundComponent
	\brief The sound loader
*/

class SoundComponent
{
private:

	std::vector<sf::Sound*> m_Sounds; //!< Vector container of sound pointers
	std::vector<sf::SoundBuffer*> m_SoundBuffer; //!< Vector container of SoundBuffer pointers
	std::vector<std::string> m_sSoundFile; //!< Vector container of strings containing the names of the sound files
	static SoundComponent* m_Instance; //!< Instance pointer of a GameSoundLoader

public:

	static SoundComponent* Instance();
	SoundComponent(); //!< Default constructor
	/*
		Gets the sound
	*/
	sf::Sound* GetSound(unsigned int iIndex/*!Initial index*/);

	/*
	   Sets the volume
	*/
	void setVolume(float volume, int index);

	/*
		Sets the pitch
	*/
	void setPitch(float pitch, int index);

	/*
		Sets the position
	*/
	void setPosition(float x, float y, float z, int index);

	/*
		Sets if the sound is looping
	*/
	void setLooping(bool b, int index);

	/*
		If the sound is playing
	*/
	sf::SoundSource::Status isPlaying(int index);

	/*
		Sets sound on pause
	*/
	void pause(int index);

	/*
		Sets the sound to stop
	*/
	void stop(int index);

	/*
		Plays the sound
	*/
	void play(int index);

	/*
		Sets the attenuation 
	*/
	void setAttenuation(int attenuation, int index);

	/*
		If the sound is releative to the listener
	*/
	void setRelativeToListener(bool b, int index);

	/*
		Loads sound
	*/
	void loadSound(std::string pathname);

	/*
		Destorys sounds
	*/
	void destroySounds(int index);
};

#endif // !SOUNDCOMPONENT_H