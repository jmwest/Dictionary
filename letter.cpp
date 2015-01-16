//
//  letter.cpp
//  EECS 281 Project 1
//
//  Created by John West on 1/13/15.
//  Copyright (c) 2015 John West. All rights reserved.
//

#include "letter.h"

#include "DictionaryEntry.h"

#include <cstdlib>
#include <iostream>
#include <getopt.h>

#include <vector>
#include <string>
#include <deque>
#include <sstream>


#include <fstream>

using namespace std;

enum Routing {noROUT, STACK, QUEUE};
enum Modification {noMOD, CHANGE, LENGTH, BOTH};
enum Output {DEFAULTOUTPUT, WOUT, MOUT};

void parseCommandLineInput(int & argc, char *argv[], string &begin, string &end, Routing &rout, Modification &modify, Output &output);
//

vector<string>* storeDictionaryInDataStructure(string* begin, string* end);
//

vector<string>* findLettermansPath(Routing &rout, Modification &modify, string* begin, string* end, vector<string>* dictionary);
//

int main(int argc, char *argv[])
{
	// Uncomment to redirect stdin to take input from file
//	ifstream arq(getenv("MYARQ"));
//	cin.rdbuf(arq.rdbuf());
	//

	//valgrind will report memory leak when sync_with_stdio is false
	ios_base::sync_with_stdio(false);
	//

	string begin;
	string end;
	vector<string>* dictionary;
	Routing rout = noROUT;
	Modification modify = noMOD;
	Output outp = DEFAULTOUTPUT;

	parseCommandLineInput(argc, argv, begin, end, rout, modify, outp);

	dictionary = storeDictionaryInDataStructure(&begin, &end);

	return 0;
}

void parseCommandLineInput(int & argc, char *argv[], string &begin, string &end, Routing &rout, Modification &modify, Output &output)
{
	static struct option longopts[] = {
		{"stack",	no_argument,		nullptr, 's'},
		{"queue",	no_argument,		nullptr, 'q'},
		{"change",	no_argument,		nullptr, 'c'},
		{"length",	no_argument,		nullptr, 'l'},
		{"output",	required_argument,	nullptr, 'o'},
		{"begin",	required_argument,	nullptr, 'b'},
		{"end",		required_argument,	nullptr, 'e'},
		{"help",	no_argument,		nullptr, 'h'},
		{nullptr,	0,					nullptr, 0},
	};
	int idx = 0;
	char c;

	while ((c = getopt_long(argc, argv, "sqclo:b:e:h", longopts, &idx)) != -1) {
		switch (c) {
			case 's':
				if (rout == QUEUE) {
					cerr << "Stack and Queue were both specified. Program exit(1)\n";
					exit(1);
				}
				else if (rout == STACK)
				{
					cerr << "Stack specified twice. Program exit(1)\n";
					exit(1);
				}
				else {
					rout = STACK;
				}
				break;
			case 'q':
				if (rout == STACK) {
					cerr << "Stack and Queue were both specified. Program exit(1)\n";
					exit(1);
				}
				else if (rout == QUEUE)
				{
					cerr << "Queue specified twice. Program exit(1)\n";
					exit(1);
				}
				else {
					rout = QUEUE;
				}
				break;
			case 'c':
				if (modify == LENGTH) {
					modify = BOTH;
				}
				else if(modify != BOTH) {
					modify = CHANGE;
				}
				break;
			case 'l':
				if (modify == CHANGE) {
					modify = BOTH;
				}
				else if(modify != BOTH) {
					modify = LENGTH;
				}
				break;
			case 'o':
				if (*optarg == 'M') {
					output = MOUT;
				}
				else if (*optarg == 'W') {
					output = WOUT;
				}
				else {
					cerr << "Invalid output character. Program exit(1)\n";
					exit(1);
				}
				break;
			case 'b':
				begin = string(optarg);
				break;
			case 'e':
				end = string(optarg);
				break;
			case 'h':
				cerr << "Useful help message here.\n";
				exit(0);
				break;
			default:
				break;
		}
	}

	// Check for necessary conditions:
	if (rout == noROUT) {
		cerr << "Neither Stack nor Queue were specified. Program exit(1)\n";
		exit(1);
	}
	else if (modify == noMOD) {
		cerr << "Neither Change nor Length were specified. Program exit(1)\n";
		exit(1);
	}
	else if ((begin.length() == 0) or (end.length() == 0)) {
		cerr << "Either begin or end wasn't specified. Program exit(1)\n";
		exit(1);
	}
	else if ((modify == CHANGE) and (begin.length() != end.length() )) {
		cerr << "Length was not specified while begin and end do not have the same length.";
		cerr << " Mission Impossible. Program exit(1)\n";
		exit(1);
	}
}

vector<string>* storeDictionaryInDataStructure(string* begin, string* end)
{
	string size_in;
	int size = 0;
	string entry;
	bool beginIsInDictionary = false;
	bool endIsInDictionary = false;

	getline(cin, size_in);

	size = atoi(size_in.c_str());

	vector<string>* dictionary = new vector<string>(size);

	for (int count = 0; getline(cin, entry) and (count < size);) {
		if (entry.length() == 0) {
			break;
		}
		else if (entry.at(0) == ' ') {
			break;
		}
		else if (entry.at(0) == '\n') {
			break;
		}

		entry.pop_back();

		if (entry.at(0) != '/') {
			if (begin->compare(entry) == 0) {
				beginIsInDictionary = true;
			}
			else if (end->compare(entry) == 0){
				endIsInDictionary = true;
			}

			(*dictionary)[count] = entry;

			cout << dictionary->at(count) << endl;

			count++;
		}
	}

	if (!beginIsInDictionary) {
		cerr << "begin does not exist in the dictionary. Program exit(1)\n";
		exit(1);
	}
	else if (!endIsInDictionary) {
		cerr << "end does not exist in the dictionary. Program exit(1)\n";
		exit(1);
	}

	return dictionary;
}

vector<string>* findLettermansPath(Routing &rout, Modification &modify, string* begin, string* end, vector<string>* dictionary)
{
	vector<string>* path = new vector<string>();

	deque<DictionaryEntry*>* deck = new deque<DictionaryEntry*>();

	DictionaryEntry* first_entry = new DictionaryEntry(begin);

	DictionaryEntry* current_entry;

	deck->push_front(first_entry);

	while (dictionary->size() != 0) {
		if (rout == STACK) {
			current_entry = deck->front();
			deck->pop_front();
		}
		else {
			current_entry = deck->back();
			deck->pop_back();
		}

		
	}

	for (int d = 0; d < deck->size(); d++) {
		delete deck->front();
		deck->pop_front();
	}

	delete [] deck; deck = nullptr;

	return path;
}



















