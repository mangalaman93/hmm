#ifndef LEARNING_H
#define LEARNING_H

#include <map>
#include <list>
#include <cstring>
#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <cstdlib>
using namespace std;

#define START_TAG "start"
#define END_TAG "end"
#define NO_TAG "notag"
#define TO_MERGE true

class TagMap {
	map<string, int> table;

  public:
	TagMap();
	int getValueAt(string tag);
	int incrementAt(string tag);
	float getProbAt(string tag);
	void print(ofstream& wfile);
};
typedef TagMap* TagMapPtr;

struct DimString
{
    string horiz_string;
    string vert_string;
    DimString(string hs="", string vs="") : horiz_string(hs), vert_string(vs) {}
    bool operator() (const DimString& lhs, const DimString& rhs) const
    {
    	if(lhs.horiz_string == rhs.horiz_string)
    		return (lhs.vert_string < rhs.vert_string);
    	else
    		lhs.horiz_string < rhs.horiz_string;
    }
};

class Repository {
	int tot_trans, tot_obs;
	string name;
	list<string> corpus;
	map<DimString, int, DimString> trans_table;
	map<string, TagMapPtr> obs_table;
	
	string toLowerStr(string str);
	void increTransTable(string tag1, string tag2);
	void modifyTables(string word, string old_tag, string tag);

  public:
	Repository(string name);
	void addFile(string file_name);
	void dumpAll();
	// string viterbi(string sent);
};

#endif
