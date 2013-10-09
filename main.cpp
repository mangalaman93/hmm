#include "learning.h"
#include <dirent.h>
using namespace std;

int main()
{
	Repository r("bnc");
	string dir_name = "BNC/";

	DIR *dir;
	struct dirent *ent;
	if((dir = opendir(dir_name.c_str())) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			if(string(ent->d_name).find(".txt") != string::npos)
				r.addFile(dir_name + string(ent->d_name));
		}
	  	closedir (dir);
	}

	r.dumpAll();
}
