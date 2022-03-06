#include <assert.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <sys/time.h>
#include "dictmatch.h"
#include "dm_prop.h"

#define MAX_MATCHED_KEYWORD 1024

using namespace std;

static void show_help(const char* progname);


int main(int argc, char* argv[])
{
        char arg;
        char buff[1024];
	int  len = 0;
	int  print_flag = 0;
        const char* dictname = NULL;
        const char* infile   = NULL;
        dm_dict_t* dm_dict  = NULL;
        vector<string> vec;

        // parse command line
        if(argc==1){
                show_help(argv[0]);
                exit(-1);
        }
        while((arg=getopt(argc, argv, "d:f:p"))!=-1){
                switch(arg){
                        case 'd':
                                dictname = optarg;
                                break;
                        case 'f':
                                infile = optarg;
                                break;
			case 'p':
				print_flag = 1;
				break;
                        default:
                                show_help(argv[0]);
                                exit(-1);
                }
        }
        // load infile into vector
        FILE* fp = fopen(infile, "rb");
        assert(fp);
        while(fgets(buff, sizeof(buff), fp)){
		len = strlen(buff);
		buff[--len] = '\0';
                vec.push_back(buff);
        }

        // load dict
        dm_dict = dm_dict_load(dictname);
        assert(dm_dict);

        struct timeval tv_begin;
        struct timeval tv_end;
        double sec = 0.0;
        double usec  = 0.0;

	dm_result_t* dm_result = NULL;
	dm_result = dm_result_create(MAX_MATCHED_KEYWORD);
	assert(dm_result);

        gettimeofday(&tv_begin, NULL);
        for(unsigned int i=0;i<vec.size();i++){
		const char* input = NULL;
		len = vec[i].size();
		input = vec[i].c_str();

                if(dm_match(dm_dict, input, len, dm_result)!=0){
                        assert(0);
                }
		if(print_flag==0){
			continue;
		}
		char outbuf[40960];
		outbuf[0] = '\0';
		if(dm_result->lemma_num > 0){
			strcat(outbuf, input);
			strcat(outbuf, "\t");

			for(int j=0;j<dm_result->lemma_num;j++){
				lemma_t* lemma = dm_result->lemma_array[j];
				char* keyword = dm_dict_get_lemma_string(dm_dict, lemma);
				strcat(outbuf, keyword);
				strcat(outbuf, ":");

				if(DM_IS_HONGGE(lemma->prop)){
					strcat(outbuf,  "(红歌);");
				}
				if(DM_IS_MINGZHU(lemma->prop)){
					strcat(outbuf,  "(名著);");
				}
				if(DM_IS_SANJI(lemma->prop)){
					strcat(outbuf, "(三级);");
				}

			}	
			fprintf(stdout, "%s\n", outbuf);
		}
		else{
			fprintf(stdout, "%s\t(不包含关键词)\n", input);
		}
        }
        gettimeofday(&tv_end, NULL);

        if(tv_end.tv_usec>tv_begin.tv_usec){
                sec = tv_end.tv_sec - tv_begin.tv_sec;
                usec = tv_end.tv_usec - tv_begin.tv_usec;
                sec += usec/1000000;
        }
        else{
                usec = 1000000 + tv_end.tv_usec - tv_begin.tv_usec;
                sec  = tv_end.tv_sec - 1 - tv_begin.tv_sec;
                sec += usec/1000000;
        }
        fprintf(stderr, "time used :%f\n", sec);


        dm_result_free(dm_result);
        dm_dict_free(dm_dict);

        return 0;
}
                                
void show_help(const char* progname)
{
        fprintf(stderr, "usage: %s -d dictname -f infile -p\n", progname);
}
