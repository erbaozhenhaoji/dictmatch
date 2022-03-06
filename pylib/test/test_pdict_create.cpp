#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <py_dict.h>

int main(int argc, char* argv[])
{
	py_dict_t* pdict = NULL;
	
	pdict = pydict_create(100, 555);
	assert(pdict);

  pydict_add(pdict, "hongkong1", 9, 111, 1111);
  pydict_add(pdict, "hongkong2", 9, 222, 2222);
  pydict_add(pdict, "hongkong3", 9, 333, 3333);
  
  pydict_save(pdict, "./", "dictbin");

	return 0;
}
