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

bool checkIfChangeMorph(string* one, string* two);
//

bool checkIfLengthMorph(string* one, string* two);
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
	vector<string>* path;
	Routing rout = noROUT;
	Modification modify = noMOD;
	Output outp = DEFAULTOUTPUT;

	parseCommandLineInput(argc, argv, begin, end, rout, modify, outp);

	dictionary = storeDictionaryInDataStructure(&begin, &end);

	path = findLettermansPath(rout, modify, &begin, &end, dictionary);

	cout << "Words in morph: " << (int)path->size() << "\n";

	for (int i = (int)path->size() - 1; i >= 0; i--) {
		cout << path->at(i) << "\n";
	}

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

		if (entry.at(entry.size() - 1) == '\n') {
			entry.pop_back();
		}

		if (entry.at(0) != '/') {
			if (begin->compare(entry) == 0) {
				beginIsInDictionary = true;
			}
			else if (end->compare(entry) == 0){
				endIsInDictionary = true;
			}

			(*dictionary)[count] = entry;

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
	vector<DictionaryEntry*>* used_entries = new vector<DictionaryEntry*>();

	deque<DictionaryEntry*>* deck = new deque<DictionaryEntry*>();

	DictionaryEntry* first_entry = new DictionaryEntry(begin);

	DictionaryEntry* current_entry;

	for (int i = 0; i < dictionary->size(); i++) {
		if (dictionary->at(i).compare(*begin) == 0) {
			dictionary->erase(dictionary->begin() + i);
		}
	}

	deck->push_front(first_entry);
	first_entry = nullptr;

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

		for (int i = 0; i < dictionary->size(); i++) {
			if ((modify == CHANGE) or (modify == BOTH)) {
				if (checkIfChangeMorph(current_entry->getWord(), &dictionary->at(i))) {
					DictionaryEntry* new_entry = new DictionaryEntry(&dictionary->at(i), current_entry->getWord());

					deck->push_front(new_entry);
					dictionary->erase(dictionary->begin() + i);
					i--;
				}
			}

			if ( ((modify == LENGTH) or (modify == BOTH)) and
				(current_entry->getWord()->compare(*deck->front()->getWord())) ) {
				if (checkIfLengthMorph(current_entry->getWord(), &dictionary->at(i))) {
					DictionaryEntry* new_entry = new DictionaryEntry(&dictionary->at(i));
					
					deck->push_front(new_entry);
					dictionary->erase(dictionary->begin() + i);
					i--;
				}
			}

			if (deck->front()->getWord()->compare(*end) == 0) {
				reachedTheEnd = true;
				break;
			}
		}

		used_entries->push_back(current_entry);
	}

	if (deck->size() != 0) {
		string* backtrack = deck->front()->getPreviousWord();

		path->push_back(*deck->front()->getWord());

		for (int f = 0; f < used_entries->size(); f++) {
			if (backtrack->compare(*used_entries->at(f)->getWord()) == 0) {
				path->push_back(*backtrack);
				backtrack = used_entries->at(f)->getPreviousWord();
				f = 0;
			}

			if (backtrack->compare(*begin) == 0) {
				path->push_back(*backtrack);
				break;
			}
		}
	}

	for (int d = 0; d < deck->size(); d++) {
		delete deck->front();
		deck->pop_front();
	}

	for (int u = 0; u < used_entries->size(); u++) {
		delete used_entries->back();
		used_entries->pop_back();
	}

	delete deck; deck = nullptr;
	delete used_entries; used_entries = nullptr;

	return path;
}

bool checkIfChangeMorph(string* one, string* two)
{
	int discrepancyNumber = 0;

	if (one->size() == two->size()) {
		for (int i = 0; i < one->size(); i++) {
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
		}

		if (discrepancyNumber <= 1) {
			return true;
		}
	}

	return false;
}
