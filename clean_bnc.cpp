#include <iostream>
#include <dirent.h>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdlib>
using namespace std;

void cleanFile(string From, string To)
{
    cout<<"cleaning "<<From<<" into "<<To<<" ..."<<endl;

    //read file
    ifstream file(From.c_str());
    file.is_open();
    if(file.fail()) {
        cout<<"\tfile: "<<From<<" does not exist!"<<endl;
        return;
    }

    //read file
    ofstream out(To.c_str());
    out.is_open();
    if(out.fail()) {
        cout<<"\tfile: "<<To<<" can't be created!"<<endl;
        return;
    }

    string word, acc="";
    while(file.good()) {
        file >> word;
        if(word[word.length()-1] == '.' || word[word.length()-1] == '?' || word[word.length()-1] == '!')
        {
            acc = acc + word;
            out << acc << endl;
            acc = "";
        }
        else
            acc = acc + word + " ";
    }

    file.close();
    out.close();
}

int main()
{
    string dir_name = "BNC/";
    string new_dir_name = "BNC_CLEANED/";

    DIR *dir;
    struct dirent *ent;
    if((dir = opendir(dir_name.c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if(string(ent->d_name).find(".txt") != string::npos)
                cleanFile(dir_name + string(ent->d_name), new_dir_name + string(ent->d_name));
        }
        closedir (dir);
    }
}
