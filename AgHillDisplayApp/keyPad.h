#pragma once
#include "Button.h"

struct key {
	char* keyName;
	keyRelation* attached;
};

struct keyRelation {
	attachDir anchorDir;
	key* attachedKey;
	attachDir attachedDir;
	unsigned int distance;
};

struct rootKeyRelation {
	attachDir anchorDir;
	attachDir attachedDir;
	unsigned int distance;
};

struct keypadInfo {
	unsigned int x;
	unsigned int y;
	unsigned int w;
	rootKeyRelation rootkeyPos;
	key* keys;
	unsigned int margin;
};

struct keypadBlueprint {
	char* attachedKey;
};

class keypad : public attachableUI {
public:
	keypad() {
		
	}
private:

};
