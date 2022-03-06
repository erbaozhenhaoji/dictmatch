#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <cassert>
#include <string>
#include <vector>

using namespace std;

void show_help(const char* progname);

int main(int argc, char* argv[])
{
	char buff[2*1024*1024];
	char linebuf[1*1024*1024];
	char filename[256];
	const char* path = NULL;
	char arg = '\0';
	vector<string> record_vec;
	int len = 0;
	int dcnt = 0;
	int fno = 0;
	FILE* fp = NULL;

	if(argc == 1){
		show_help(argv[0]);
		exit(-1);
	}

	while((arg=getopt(argc, argv, "p:"))!=-1){
		switch(arg){
			case 'p':
				path = optarg;
				break;
			default:
				show_help(argv[0]);
				exit(-1);
		}
	}

	buff[0] = 0;
	while(fgets(linebuf, sizeof(linebuf), stdin)){
		len = strlen(buff);
		if(strncmp(linebuf, "</doc>\001\n", len)==0){
			memcpy(buff+dcnt, linebuf, len);
			dcnt += len;
			buff[dcnt] = '\0';
			record_vec.push_back(linebuf);
			buff[0]  = 0;
			dcnt = 0;
			if(record_vec.size()==3000){
				sprintf(filename, "%s/%d.raw", path, fno);
				fp = fopen(filename, "wb");
				assert(fp);
				for(unsigned int i=0;i<record_vec.size();i++){
					fprintf(fp, "%s\n", record_vec[i].c_str());
				}
				fclose(fp);
				fno++;
				record_vec.clear();

			}
		}
		else{
			memcpy(buff+dcnt, buff, len);
			dcnt += len;
			buff[dcnt] = '\0';
		}


	}



	return 0;
}


void show_help(const char* progname)
{
	fprintf(stderr, "usage : %s -p path <stdin>\n", progname);
}

