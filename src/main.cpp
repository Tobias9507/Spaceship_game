#include "core.h"

int main() {
	coreInit(1980, 1080, "Floor is lava!");
	while(coreIsOpen()){
		coreUpdateAndRender();
	}
	coreDestroy();
}