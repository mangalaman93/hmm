#include "learning.h"

/******************* TagMap class *******************/
TagMap::TagMap() {}

int TagMap::getValueAt(string tag) {
	if(table.count(tag) > 0)
		return table[tag];
	else
		return 0;
}

float TagMap::getProbAt(string tag) {
	if(table.count(tag) == 0)
		return 0;

	float sum = 0;
	for(map<string, int>::iterator it=table.begin(); it!=table.end(); ++it)
	{
		sum += it->second;
	}
 	return (((float)table[tag])/sum);
 }

int TagMap::incrementAt(string tag) {
	if(table.count(tag) == 0)
		table[tag] = 1;
	else
		table[tag]++;

	return table[tag];
}

void TagMap::print(ofstream& wfile)
{
	for(map<string, int>::iterator it=table.begin(); it!=table.end(); ++it)
	{
		wfile << "\t" << it->first << ":" << it->second;
	}
}

int TagMap::getNumTags()
{
	return table.size();
}

/******************* Repository class *******************/
Repository::Repository(string name) {
	this->name = name;
	tot_trans = 0;
	tot_obs = 0;
}

string Repository::toLowerStr(string str) {
	string out = str;
	for(int i =0; i<str.length(); i++) {
		out[i] = tolower(str[i]);
	}
	return out;
}

void Repository::increTransTable(string tag1, string tag2)
{
	DimString ds(tag1, tag2);
	if(trans_table.count(ds) > 0)
		trans_table[ds]++;
	else
		trans_table[ds] = 1;
}

void Repository::modifyTables(string word, string old_tag, string tag)
{
	if(old_tag == "" || tag == "")
		return;

	tags[tag] = true;

	//incrementing observation table
	if(obs_table.count(word) > 0) {
		TagMapPtr tapt = obs_table[word];
		tapt->incrementAt(tag);
	} else {
		TagMapPtr ta = new TagMap();
		ta->incrementAt(tag);
		obs_table[word] = ta;
	}

	//incrementing transition table
	increTransTable(old_tag, tag);
	tot_trans++;
	tot_obs++;
}

int Repository::getTransVal(string tag1, string tag2)
{
	DimString ds(tag1, tag2);
	if(trans_table.count(ds) > 0)
		return trans_table[ds];
	else
		return 0;
}

void Repository::addFile(string file_name) {
	//if the file is already processed
	list<string>::iterator it;
	bool found_flag = false;
	for(it = corpus.begin(); it != corpus.end(); it++) {
		if(strcmp(it->c_str(), file_name.c_str()) == 0) {
			found_flag = true;
			break;
		}
	}

	if(!found_flag)
		corpus.push_back(file_name);

	//read file and build observation table
	ifstream file(file_name.c_str());
	file.is_open();
	if(file.fail()) {
		cout<<"file: "<<file_name<<" does not exist!"<<endl;
		return;
	}

	while(file.good()) {
		string old_tag[2] = {START_TAG, ""};
		string tagged_word;

		while(file.good()) {
			file >> tagged_word;

			if(tagged_word[0] == ' ' || tagged_word[0] == '\n' || tagged_word[0] == '\t')
				continue;

			tagged_word = toLowerStr(tagged_word);
			int pos_ = tagged_word.find_first_of('_');
			if(pos_ == string::npos) { continue;}
			string comb_tag = tagged_word.substr(0, pos_);
			string word = tagged_word.substr(pos_+1, string::npos);

			int pos_hyphen = comb_tag.find_first_of('-');
			string tag[2];
			tag[0] = comb_tag.substr(0, pos_hyphen);
			tag[1] = comb_tag.substr(pos_hyphen+1, string::npos);

			if(TO_MERGE)
			{
				tag[0] = tag[0] == "to0" ? "pni":tag[0];
				tag[1] = tag[1] == "to0" ? "pni":tag[0];
			}

			for(int i=0; i<2; i++)
				for(int j=0; j<2; j++)
					modifyTables(word, old_tag[i], tag[j]);

			old_tag[0] = tag[0];
			old_tag[1] = tag[1];

			if(tagged_word[tagged_word.length()-1] == '.' || tagged_word[tagged_word.length()-1] == '?' ||
			  tagged_word[tagged_word.length()-1] == '!')
				break;
		}

		for(int i=0; i<2; i++)
			if(old_tag[i] != "")
			{
				increTransTable(old_tag[i], END_TAG);
				tot_trans++;
			}

	}
	file.close();
	cout<<"file: "<<file_name<<" is processed!\n";
}

void Repository::dumpAll() {
	string file_name = name;
	ofstream wfile(file_name.append(".corp").c_str());
	wfile.is_open();
	wfile << name << "\n";

	//printing file names
	wfile << "/* Files */\n";
	list<string>::iterator it;
	for(it = corpus.begin(); it != corpus.end(); it++) {
		wfile << *it << "\n";
	}
	wfile <<"\n";

	//printing overall data
	wfile << "/* other data */\n";
	wfile << "Total tags: " << tags.size() << "\n";
	wfile << "Total words: " << tot_obs << "\n";
	wfile << "Total trans: " << tot_trans << "\n\n";

	//printing transition table
	wfile << "/* Transition Table */\n" ;
	for(map<DimString, int, DimString>::iterator it=trans_table.begin(); it!=trans_table.end(); ++it)
	{
		wfile << it->first.horiz_string << ", " << it->first.vert_string << " => " << it->second << "\n";
	}
	wfile << "\n";

	//printing observation table
	wfile << "/* Observation Table */\n";
	for(map<string, TagMapPtr>::iterator mit=obs_table.begin(); mit!=obs_table.end(); mit++) {
		wfile << mit->first << " => ";
		mit->second->print(wfile);
		wfile << "\n";
	}

	wfile.close();
	cout<<"Repository "<<name<<" dumped in file "<<file_name<<"!\n";
}

string Repository::viterbi(string sent) {
	//splitting the sentence into words
	list<string> *words = new list<string>();
	int first = 0;
	for(int i=0; i<=sent.length(); i++) {
		if(i == sent.length() || sent[i] == ' ' || sent[i] == '\t') {
			if(first == i) {
				first = i+1;
				continue;
			}
			words->push_back(toLowerStr(sent.substr(first, i-first)));
			first = i+1;
		}
	}

	//applying viterbi algorithm
	map<string, list<string> > tag_list;
	map<string, list<string> > temp_tag_list;
	map<string, float> state;
	map<string, float> temp_state;
	for(map<string, bool>::iterator it=tags.begin(); it!=tags.end(); it++) {
		state[it->first] = ((float) (getTransVal(START_TAG, it->first)))/((float) tot_trans);
	}
	
	float max_prob, temp, trans_prob;
	string max_index;
	int loop_count = 1;
	bool tag_flag = false;
	for(list<string>::iterator it = words->begin(); it != words->end(); it++) {
		for(map<string, bool>::iterator jit=tags.begin(); jit!=tags.end(); jit++) {
			max_prob = -1;
			for(map<string, bool>::iterator iit=tags.begin(); iit!=tags.end(); iit++) {
				//P(word being Tag) * P(Tag followed by Tag)
				if(loop_count == words->size()) {
					trans_prob = ((float) (getTransVal(iit->first, END_TAG)))/((float) tot_trans);
				} else {
					trans_prob = ((float) (getTransVal(iit->first, jit->first)))/((float) tot_trans);
				}
				float state_prob;
				if(obs_table.count(*it) > 0) {
					state_prob = (obs_table[*it])->getProbAt(iit->first);
				} else {
					state_prob = 1;
/*					TagMapPtr ta = new TagMap();
					ta->incrementAt(i);
					obs_table[*it] = ta;
					state_prob = (obs_table[*it])->getProbAt(i); */
					tag_flag = true;
				}
				temp = state[iit->first] * state_prob * trans_prob;
				if(max_prob < temp) {
					max_index = iit->first;
					max_prob = temp;
				}
			}
			temp_state[jit->first] = max_prob;
			temp_tag_list[jit->first] = tag_list[max_index];
			if(tag_flag) {
				temp_tag_list[jit->first].push_back(NO_TAG);
				tag_flag = false;
			}
			else {
				temp_tag_list[jit->first].push_back(max_index);
			}
		}
		for(map<string, bool>::iterator tag_it=tags.begin(); tag_it!=tags.end(); tag_it++) {
			state[tag_it->first] = temp_state[tag_it->first];
			tag_list[tag_it->first] = temp_tag_list[tag_it->first];
		}
		loop_count++;
	}

	max_prob = -1;
	for(map<string, bool>::iterator tag_it=tags.begin(); tag_it!=tags.end(); tag_it++) {
		if(state[tag_it->first] > max_prob) {
			max_prob = state[tag_it->first];
			max_index = tag_it->first;
		}
	}

	string out;
	for(list<string>::iterator iter=tag_list[max_index].begin(); iter!=tag_list[max_index].end(); iter++) {
		if(*iter == NO_TAG)
			out.append("X");
		else
			out.append(*iter);
		out.append("_");
		out.append(words->front());
		words->pop_front();
		out.append(" ");
	}

	return out;
}
