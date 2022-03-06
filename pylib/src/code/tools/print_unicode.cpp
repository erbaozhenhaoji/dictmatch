#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <py_code.h>

static void show_help(const char* progname);

int main(int argc, char* argv[])
{
	char buff[1024];
	int len = 0;
	int word_num = 0;
	char arg = '\0';
	bool decimal_out = false;
	bool hex_out     = false;
	unsigned short unicode_array[1024];
	
	while((arg=getopt(argc, argv, "dx"))!=-1){
		switch(arg){
			case 'd':
				decimal_out = true;
				break;
			case 'x':
				hex_out = true;
				break;
			default:
				show_help(argv[0]);
				exit(-1);
		}
	}

	if(decimal_out==false && hex_out==false){
		show_help(argv[0]);
		exit(-1);
	}

	while(fgets(buff, sizeof(buff), stdin)){
		len = strlen(buff);
		while(buff[len-1]=='\n'){
			buff[--len] = '\0';
		}
		word_num = utf8_to_unicode(buff, len, unicode_array, sizeof(unicode_array));

		for(int i=0;i<word_num;i++){

			if(hex_out==true){
				fprintf(stdout, "0x%X\t", unicode_array[i]);
			}
			else if(decimal_out == true){
				fprintf(stdout, "%d\t", unicode_array[i]);
			}
			else{
				assert(0);
			}
		}

		fprintf(stdout, "\n");
	}

	return 0;
}


void show_help(const char* progname)
{
	fprintf(stderr, "usage: %s -d|x <stdin >stdout\n", progname);
	fprintf(stderr, "     :    -d, for decimal output\n");
	fprintf(stderr, "     :    -x, for hex output\n");
}
