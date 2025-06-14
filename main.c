#include "sayHello.h"

// interfaces to read
struct Configuration {
	int interval;
	char* log_tag;
};

int main(){
	sayHello("Jesus");
	return 0;
}
