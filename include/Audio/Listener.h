#pragma once
/*
	Audio engine for playing 3D sounds
	Author: Natalie Downes - p12194892
*/

#include <SFML\Audio.hpp>

/*
	Uses OpenAL wrapped in SFML Audio
	Only ever one listener in the project (Such as a moving main character
*/

class Listener
{
	public:
		/*
			Default constructor.
		*/
		Listener();

		/*
		Singleton
		*/
		static Listener* Instance();


		/*
			Sets the position
		*/
		void setPosition(int x, int y, int z);

		/*
			Sets the direction of the listener
		*/
		void setDirection(int x, int y, int z);

		/*
			Set Global Volume
		*/

		void setGlobalVolume(int volume);


	private:
		sf::Listener* listener; 
		static Listener* m_Instance; //!< Instance pointer of a Listener
};
