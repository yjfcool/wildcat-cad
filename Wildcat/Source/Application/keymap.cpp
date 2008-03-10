//Included Header Files
#include "Application/keymap.h"
#include "Kernel/workbench.h"


/***********************************************~***************************************************/


WCKeyMap::WCKeyMap() {
	//Nothing for now
}


WCKeyMap::~WCKeyMap() {
	//Nothing for now
}


void WCKeyMap::AddMapping(const WCKeyEvent &event, const WCUserMessage &message) {
	//Insert into the map
	this->_keyMap.insert( std::make_pair(event._key, std::make_pair(event, message) ) );
}


WCUserMessage WCKeyMap::MessageFromKeyEvent(const WCKeyEvent &event) {
	WCKeyEvent tmpEvent(' ');
	
	//Iterate through map and test all matching char
	std::multimap<char, std::pair<WCKeyEvent,WCUserMessage> >::iterator mapIter;
	mapIter = this->_keyMap.find(event._key);
	//If mapIter exists
	if (mapIter != this->_keyMap.end()) {
		//Test to see if the same
		tmpEvent = (*mapIter).second.first;
		if (tmpEvent == event ) 
			//Return the message
			return (*mapIter).second.second;
	}
	//No such key found
	return ""; 
}


/***********************************************~***************************************************/

