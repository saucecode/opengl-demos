#include "Magic.hpp"

#include <cstdlib>
#include <cstdio>


char* loadFile(const char *fname){
	FILE *f = fopen(fname, "rb");
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);  //same as rewind(f);

	char *string = (char*) malloc(fsize + 1);
	fread(string, fsize, 1, f);
	fclose(f);

	string[fsize] = 0;
	return string;
}
