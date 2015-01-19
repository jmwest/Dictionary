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

vector<string*>* storeDictionaryInDataStructure(string* begin, string* end);
//

vector<string*>* findLettermansPath(Routing &rout, Modification &modify, string* begin, string* end, vector<string*>* dictionary);
//

bool checkIfChangeMorph(string* one, string* two);
//

bool checkIfLengthMorph(string* one, string* two);
//

void printPath(vector<string*>*);

int main(int argc, char *argv[])
{
	// Uncomment to redirect stdin to take input from file
//	ifstream arq(getenv("MYARQ"));
//	cin.rdbuf(arq.rdbuf());
	// New method coming up with on my own
//	streambuf *ifbuf;
//	ifstream dict;
//	dict.open("dictionary-xlarge-234936.txt");
//	ifbuf = dict.rdbuf();
//	cin.rdbuf(ifbuf);
	//

	//valgrind will report memory leak when sync_with_stdio is false
//	ios_base::sync_with_stdio(false);
	//

	string begin;
	string end;
	vector<string*>* dictionary;
	vector<string*>* path;
	Routing rout = noROUT;
	Modification modify = noMOD;
	Output outp = DEFAULTOUTPUT;

	parseCommandLineInput(argc, argv, begin, end, rout, modify, outp);

	dictionary = storeDictionaryInDataStructure(&begin, &end);

	path = findLettermansPath(rout, modify, &begin, &end, dictionary);

	cout << "Words in morph: " << (int)path->size() << "\n";

	for (int i = (int)path->size() - 1; i >= 0; i--) {
		cout << *path->at(i) << "\n";
	}

	for (unsigned int i = 0; i < dictionary->size(); i++) {
		if (dictionary->at(i) != NULL) {
			delete dictionary->at(i); dictionary->at(i) = NULL;
		}
	}

	for (unsigned int i = 1; i < path->size() - 1; i++) {
		if (path->at(i) != NULL) {
			delete path->at(i); path->at(i) = NULL;
		}
	}

	delete dictionary; dictionary = NULL;
	delete path; path = NULL;

	// Comment this out when not using xcode
//	dict.close();
	//

	return 0;
}

void parseCommandLineInput(int & argc, char *argv[], string &begin, string &end, Routing &rout, Modification &modify, Output &output)
{
	static struct option longopts[] = {
		{"stack",	no_argument,		NULL, 's'},
		{"queue",	no_argument,		NULL, 'q'},
		{"change",	no_argument,		NULL, 'c'},
		{"length",	no_argument,		NULL, 'l'},
		{"output",	required_argument,	NULL, 'o'},
		{"begin",	required_argument,	NULL, 'b'},
		{"end",		required_argument,	NULL, 'e'},
		{"help",	no_argument,		NULL, 'h'},
		{NULL,		0,					NULL, 0},
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
				else if(modify == noMOD) {
					modify = CHANGE;
				}
				break;
			case 'l':
				if (modify == CHANGE) {
					modify = BOTH;
				}
				else if(modify == noMOD) {
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

vector<string*>* storeDictionaryInDataStructure(string* begin, string* end)
{
	string size_in;
	int size = 0;
	string entry;
	bool beginIsInDictionary = false;
	bool endIsInDictionary = false;

	getline(cin, size_in);

	size = atoi(size_in.c_str());

	vector<string*>* dictionary = new vector<string*>(size);

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

		if (entry.at(entry.size() - 1) == '\n') {
//			entry->pop_back()
			entry.erase(entry.begin() + entry.size() - 1);
		}

		if (entry.at(0) != '/') {
			if (begin->compare(entry) == 0) {
				beginIsInDictionary = true;
			}
			else if (end->compare(entry) == 0) {
				endIsInDictionary = true;
			}

			dictionary->at(count) = new string(entry);

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

vector<string*>* findLettermansPath(Routing &rout, Modification &modify, string* begin, string* end, vector<string*>* dictionary)
{
	vector<string*>* path = new vector<string*>();
	vector<DictionaryEntry*>* used_entries = new vector<DictionaryEntry*>();

	deque<DictionaryEntry*>* deck = new deque<DictionaryEntry*>();

	DictionaryEntry* first_entry = new DictionaryEntry();

	DictionaryEntry* current_entry;

	for (unsigned int i = 0; i < dictionary->size(); i++) {
		if (dictionary->at(i)->compare(*begin) == 0) {
			first_entry->word = dictionary->at(i);
			dictionary->at(i) = NULL;
			dictionary->erase(dictionary->begin() + i);
		}
	}

	deck->push_front(first_entry);
	first_entry = NULL;

	bool reachedTheEnd = false;

	while ((deck->size() != 0) and !reachedTheEnd) {
		if (rout == STACK) {
			current_entry = deck->front();
			deck->pop_front();
		}
		else {
			current_entry = deck->back();
			deck->pop_back();
		}

		for (unsigned int i = 0; i < dictionary->size(); i++) {
			if (modify == BOTH) {
				if (checkIfChangeMorph(current_entry->word, dictionary->at(i))) {
					DictionaryEntry* new_entry = new DictionaryEntry(dictionary->at(i), current_entry->word);

					deck->push_front(new_entry);
					dictionary->at(i) = NULL;
					dictionary->erase(dictionary->begin() + i);
					i--;
				}
				else if (checkIfLengthMorph(current_entry->word, dictionary->at(i))) {
					DictionaryEntry* new_entry = new DictionaryEntry(dictionary->at(i), current_entry->word);

					deck->push_front(new_entry);
					dictionary->at(i) = NULL;
					dictionary->erase(dictionary->begin() + i);
					i--;
				}
			}
			else if (modify == LENGTH) {
				if (checkIfLengthMorph(current_entry->word, dictionary->at(i))) {
					DictionaryEntry* new_entry = new DictionaryEntry(dictionary->at(i), current_entry->word);

					deck->push_front(new_entry);
					dictionary->at(i) = NULL;
					dictionary->erase(dictionary->begin() + i);
					i--;
				}
			}
			else if (modify == CHANGE) {
				if (checkIfChangeMorph(current_entry->word, dictionary->at(i))) {
					DictionaryEntry* new_entry = new DictionaryEntry(dictionary->at(i), current_entry->word);

					deck->push_front(new_entry);
					dictionary->at(i) = NULL;
					dictionary->erase(dictionary->begin() + i);
					i--;
				}
			}

			if (deck->size() != 0) {
				if (deck->front()->word->compare(*end) == 0) {
					reachedTheEnd = true;
					break;
				}
			}
		}

		used_entries->push_back(current_entry);
	}

	if (deck->size() != 0) {
		string* backtrack = deck->front()->previous;

		path->push_back(end);

		deck->front()->word = NULL; deck->front()->previous = NULL;
		delete deck->front(); deck->front() = NULL;
		deck->pop_front();

		for (unsigned int f = 0; f < used_entries->size(); f++) {
			if (backtrack->compare(*used_entries->at(f)->word) == 0) {
				path->push_back(backtrack);
				backtrack = used_entries->at(f)->previous;

				used_entries->at(f)->word = NULL; used_entries->at(f)->previous = NULL;
				delete used_entries->at(f); used_entries->at(f) = NULL;
				used_entries->erase(used_entries->begin() + f);

				f = 0;
			}

			if (backtrack->compare(*begin) == 0) {
				path->push_back(begin);
				break;
			}
		}
	}

	for (unsigned int d = 0; d < deck->size(); d++) {
		if (deck->front()->word != NULL) {
			delete deck->front()->word; deck->front()->word = NULL;
		}
//		if (deck->front()->previous != NULL) {
//			delete deck->front()->previous; deck->front()->previous = NULL;
//		}
		delete deck->front(); deck->front() = NULL;
		deck->pop_front();
	}

	for (unsigned int u = 0; u < used_entries->size(); u++) {
		if (used_entries->back()->word != NULL) {
			delete used_entries->back()->word; used_entries->back()->word = NULL;
		}
//		if (used_entries->back()->previous != NULL) {
//			delete used_entries->back()->previous; used_entries->back()->previous = NULL;
//		}
		delete used_entries->back();
		used_entries->pop_back();
	}

	delete deck; deck = NULL;
	delete used_entries; used_entries = NULL;

	return path;
}

bool checkIfChangeMorph(string* one, string* two)
{
	int discrepancyNumber = 0;

	if (one->size() == two->size()) {
		for (unsigned int i = 0; i < one->size(); i++) {
			if (one->at(i) != two->at(i)) {
				discrepancyNumber++;
			}
		}

		if (discrepancyNumber <= 1) {
			return true;
		}
	}

	return false;
}

bool checkIfLengthMorph(string* one, string* two)
{
	if (one->size() > two->size()) {
		return checkIfLengthMorph(two, one);
	}

	int discrepancyNumber = 0;

	if ((two->size() - one->size()) == 1) {
		for (int i = 0; i < ((int)(one->size() + two->size()) / 2); i++) {
			if (one->at(i) != two->at(i + discrepancyNumber)) {
				discrepancyNumber++;
				i--;
			}

			if (discrepancyNumber >= 2) {
				return false;
			}
		}

		// don't need the check?
		if (discrepancyNumber <= 1) {
			return true;
		}
	}

	return false;
}
