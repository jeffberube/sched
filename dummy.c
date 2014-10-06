#include <stdio.h>
#include <unistd.h>

int main() {

	while (1) {
		
		printf("Test\n\0");
		fflush(stdout);
		sleep(1);

	}
	return 0;

}
