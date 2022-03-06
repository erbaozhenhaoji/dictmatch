#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include "dictmatch.h"
#include "dm_prop.h"

static void show_help(const char* progname);

int main(int argc, char* argv[])
{
        char        arg     = '\0';
        const char* rawfile = NULL;
        const char* binfile = NULL;

        // parse command line
        if(argc==1){
                show_help(argv[0]);
                exit(-1);
        }
        while((arg=getopt(argc, argv, "r:b:"))!=-1){
                switch(arg){
                        case 'r':
                                rawfile = optarg;
                                break;
                        case 'b':
                                binfile = optarg;
                                break;
                        default:
                                show_help(argv[0]);
                                exit(-1);

                }
        }

	if(dm_dict_build(rawfile, binfile, dm_prop_str2int) != 0){
		assert(0);
	}

     
        return 0;
}


void show_help(const char* progname)
{
        fprintf(stderr, "usage: %s -r rawfile -b binfile\n", progname);
}
