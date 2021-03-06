#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <py_dict.h>

int main(int argc, char* argv[])
{
	int    len   = 0;
	int    code  = 0;
	int    value = 0;
	py_dict_t* pdict = NULL;
	char   buff[1024];

	pdict = pydict_load("./", "dictbin");
	assert(pdict);

	fprintf(stdout, "hassize=%d, block_pos=%d\n",pdict->hashsize, pdict->block_pos);

	while(fgets(buff, sizeof(buff), stdin)){
		len = strlen(buff);
		while(buff[len-1]=='\n'){
			buff[--len] = 0;
		}
		if(pydict_find(pdict, buff, len, &code, &value)==1){
			//fprintf(stdout, "%s : code=%d, value=%d\n", buff, code, value);
		}
	}

	return 0;
}
