#include "learning.h"
#include <dirent.h>
#include <cmath>
#include <stdlib.h>
#include <stdio.h>
#include <cstdlib>
#include <cstdio>
using namespace std;

struct Data
{
	float P;
	float T;
	float R;
	Data(float p=0, float t=0, float r=0) : P(p), T(t), R(r) {}
};

list<string> getTagList(string str)
{
	list<string> tags;
	int first = 0;
	for(int i=0; i<=str.length(); i++)
	{
		if(str[i] == '_')
		{
			tags.push_back(str.substr(first, (i-first)));
			first = i+1;
		} else if(str[i] == ' ' || str[i] == '\t')
		{
			first = i+1;
		}
	}

	return tags;
}

string toLowerStr(string str) {
	string out = str;
	for(int i =0; i<str.length(); i++) {
		out[i] = tolower(str[i]);
	}
	return out;
}

bool isEqual(string given, string obs) {
	for(int i=0; i<given.length(); i++) {
		if(given[i] == '-') {
			return ((toLowerStr(obs) == toLowerStr(given.substr(0, i))) || (toLowerStr(obs) == toLowerStr(given.substr(i+1, given.length()-i-1))));
		}
	}
}

void runTest(Repository r, string file_name, map<string, float> &acc, map<string, float> &tot,
				map<DimString, float, DimString> &conf, Data *d) {
	//calculating precision and recall
	ifstream file(file_name.c_str());
	file.is_open();
	if(file.fail()) {
		cout<<"file does not exist!"<<endl;
		return;
	}

	string line;
	while(file.good()) {
		getline(file, line);
		list<string> given_tags = getTagList(line);
		string sent = "";
		int first = 0;
		for(int i=0; i<=line.length(); i++) {
			if(i == line.length() || line[i] == ' ' || line[i] == '\t') {
				if(first != 0)
					sent.append(" ");
				sent.append(line.substr(first, i-first));
			} else if( line[i] == '_') {
				first = i+1;
			}
		}
		string out = r.viterbi(sent);
		list<string> ob_tags = getTagList(out);

		int size = given_tags.size();
		if(size != ob_tags.size())
		{
			cout<<"error occurred!! exiting..."<<endl;
			exit(1);
		}

		for(int i=0; i<size; i++)
		{
			d->R++;
			if(ob_tags.front() == NO_TAG) {
				given_tags.pop_front();
				ob_tags.pop_front();
				continue;
			}

			string s1 = given_tags.front();
			string s2 = ob_tags.front();
			conf[DimString(s1, s2)]++;
			tot[given_tags.front()]++;
			if(isEqual(given_tags.front(), ob_tags.front())) {
				d->P++;
				acc[given_tags.front()]++;
			}
			d->T++;
			given_tags.pop_front();
			ob_tags.pop_front();
		}
	}
}

int main()
{
	string dir_name = "BNC_SMALL";
	list<string> files;

	DIR *dir;
	struct dirent *ent;
	if((dir = opendir(dir_name.c_str())) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			if(string(ent->d_name).find(".txt") != string::npos)
				files.push_back(dir_name + string("_CLEANED/") + string(ent->d_name));
		}
	  	closedir (dir);
	}

	for(int i=0; i<5; i++)
	{
		char buffer[2];
		sprintf(buffer, "%d", i);
		Repository r(string("bnc") + string(buffer));

		int j=0;
		list<string> training, test;
		for(list<string>::iterator it=files.begin(); it!=files.end(); ++it, ++j)
		{
			if(j/(files.size()/5) == i)
				test.push_back(*it);
			else
				training.push_back(*it);
		}

		for(list<string>::iterator it=training.begin(); it!=training.end(); ++it)
		{
			r.addFile(*it);
		}
		r.dumpAll();
		cout<<"training complete..."<<endl;

		// testing begins
		map<string, float> acc;
		map<string, float> tot;
		map<DimString, float, DimString> conf;
		Data* data = new Data(0, 0, 0);
		for(list<string>::iterator it=test.begin(); it!=test.end(); ++it)
		{
			cout<<"testing "<<*it<<" ..."<<endl;
			runTest(r, *it, acc, tot, conf, data);
		}

		cout<<"precision: "<<(data->P)/(data->T)<<endl;
        cout<<"recall: "<<(data->P)/(data->R)<<endl;
        cout<<"F value: "<<2*(data->P)/((data->T)+(data->R))<<endl;
	}
}
