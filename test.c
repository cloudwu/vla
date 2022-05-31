#include "vla.h"
#include <stdio.h>

int
main() {
	vla_open(tmp, int, 20);
	int i;
	for (i=0;i<vla_size(tmp);i++) {
		tmp[i] = i;
	}
	vla_resize(tmp, 10);
	int s = 0;
	for (i=0;i<vla_size(tmp);i++) {
		s += tmp[i];
	}
	printf("%d\n", s);
	vla_close(tmp);
	return 0;
}
