#pragma once
#include "framework.h"

class touchManager {
	bool isTouch;
	int touchCount;
	POINT points[5];

	touchManager() {
		isTouch = false;
		touchCount = 0;
	}

	void update() {

	}

	bool isTouch() {

	}

	int touchCount() {

	}

	POINT* getTouch(int index) {

	}
};