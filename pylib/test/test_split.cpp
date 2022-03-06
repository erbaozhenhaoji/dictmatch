#include <stdio.h>
#include <string.h>
#include <py_utils.h>

int main(int argc, char* argv[])
{
	const char* mess = "hahahah\r\n1111\r";
	char* parts[1024];
	char tmp[1024];
	int num = 0;

	strcpy(tmp, mess);

	num = split_c(tmp, parts, 1024, "\r\n");

	for(int i=0;i<num;i++){
		fprintf(stdout, "%s\n", parts[i]);
	}




	return 0;
}
