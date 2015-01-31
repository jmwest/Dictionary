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

#include <list>
#include <vector>
#include <string>
#include <deque>
#include <sstream>

using namespace std;

enum Routing {noROUT, STACK, QUEUE};
enum Modification {noMOD, CHANGE, LENGTH, BOTH};
enum Output {DEFAULTOUTPUT, WOUT, MOUT};

void parseCommandLineInput(int & argc, char *argv[], string &begin, string &end, Routing &rout, Modification &modify, Output &output);
//

list<string>* storeDictionaryInDataStructure(string* begin, string* end, Modification &modify);
//

vector<string>* findLettermansPath(Routing &rout, Modification &modify, string* begin, string* end, list<string>* dictionary);
// Returns a pointer to a vector of string pointers that contains
//		the path from begin to end through the dictionary.
// ->at(0) contains *end, and ->back() contains *begin

bool checkIfChangeMorph(string* one, string* two);
//

bool checkIfLengthMorph(string* one, string* two);
//

void printPathWords(vector<string>* path);
//

void printPathModifications(vector<string>* path);
//

int find_positional_difference(string* current, string* next);
//

void add_word_to_deque_from_dictionary(deque<DictionaryEntry>* deck, string* entry, DictionaryEntry* current);
//

int main(int argc, char *argv[]) {
	// Uncomment to redirect stdin to take input from file
//	ifstream arq(getenv("MYARQ"));
//	cin.rdbuf(arq.rdbuf());
	//

	//valgrind will report memory leak when sync_with_stdio is false
	ios_base::sync_with_stdio(false);
	//

	string begin;
	string end;
	list<string>* dictionary;
	vector<string>* path;
	Routing rout = noROUT;
	Modification modify = noMOD;
	Output outp = DEFAULTOUTPUT;

	parseCommandLineInput(argc, argv, begin, end, rout, modify, outp);

	dictionary = storeDictionaryInDataStructure(&begin, &end, modify);

	if (begin == end) {
		path = new vector<string>(1, begin);
	}
	else {
		path = findLettermansPath(rout, modify, &begin, &end, dictionary);
	}

	if (path->size() == 0) {
		ostringstream ss;

		ss << "No solution.\n";

		cout << ss.str();
	}
	else {
		if (outp == MOUT) {
			printPathModifications(path);
		}
		else {
			printPathWords(path);
		}
	}

	delete dictionary; dictionary = NULL;
	delete path; path = NULL;

	return 0;
}

void parseCommandLineInput(int & argc, char *argv[], string &begin, string &end, Routing &rout, Modification &modify, Output &output) {
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

list<string>* storeDictionaryInDataStructure(string* begin, string* end, Modification &modify) {
	list<string>* dictionary;

	string size_in;
	int size = 0;
	string entry;
	bool beginIsInDictionary = false;
	bool endIsInDictionary = false;

	getline(cin, size_in);

	size = atoi(size_in.c_str());

	if (modify == CHANGE) {
		dictionary = new list<string>();

		while (getline(cin, entry)) {
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
				entry.erase(entry.begin() + entry.size() - 1);
			}
			
			if (entry.at(0) != '/') {
				if (*begin == entry) {
					beginIsInDictionary = true;
				}
				if (*end == entry) {
					endIsInDictionary = true;
				}

				if (entry.length() == begin->length()) {
					dictionary->push_back(entry);
				}
			}
		}
	}
	else {
		dictionary = new list<string>(size);

		for (list<string>::iterator it = dictionary->begin(); getline(cin, entry) and (it != dictionary->end());) {
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
				entry.erase(entry.begin() + entry.size() - 1);
			}

			if (entry.at(0) != '/') {
				if (*begin == entry) {
					beginIsInDictionary = true;
				}
				if (*end == entry) {
					endIsInDictionary = true;
				}

				*it = entry;

				++it;
			}
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

vector<string>* findLettermansPath(Routing &rout, Modification &modify, string* begin, string* end, list<string>* dictionary) {
	vector<string>* path = new vector<string>();
	list<DictionaryEntry>* used_entries = new list<DictionaryEntry>();

	deque<DictionaryEntry>* deck = new deque<DictionaryEntry>();

	DictionaryEntry current_entry;

	deck->push_front(DictionaryEntry(begin));

	for (list<string>::iterator it = dictionary->begin(); it != dictionary->end(); ++it) {
		if (*it == *begin) {
			list<string>::iterator invalidated_it = it;
			it--;
			dictionary->erase(invalidated_it);

			break;
		}
	}

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

		for (list<string>::iterator it = dictionary->begin(); it != dictionary->end(); ++it) {
			if (((int)(current_entry.getWord()->length() - it->length()) <= 1)
				and ((int)(it->length() - current_entry.getWord()->length()) <= 1)) {

				if (modify == BOTH) {
					if (checkIfChangeMorph(current_entry.getWord(), &(*it))
						or checkIfLengthMorph(current_entry.getWord(), &(*it))) {
						add_word_to_deque_from_dictionary(deck, &(*it), &current_entry);

						list<string>::iterator invalidated_it = it;
						it--;

						dictionary->erase(invalidated_it);
					}
				}
				else if (modify == LENGTH) {
					if (checkIfLengthMorph(current_entry.getWord(), &(*it))) {
						add_word_to_deque_from_dictionary(deck, &(*it), &current_entry);

						list<string>::iterator invalidated_it = it;
						it--;
						
						dictionary->erase(invalidated_it);
					}
				}
				else if (modify == CHANGE) {
					if (checkIfChangeMorph(current_entry.getWord(), &(*it))) {
						add_word_to_deque_from_dictionary(deck, &(*it), &current_entry);

						list<string>::iterator invalidated_it = it;
						it--;
						
						dictionary->erase(invalidated_it);
					}
				}

				if (deck->size() != 0) {
					if (*deck->front().getWord() == *end) {
						reachedTheEnd = true;
						break;
					}
				}
			}
		}

		used_entries->push_back(current_entry);
	}

	if ((int)deck->size() != 0) {
		string backtrack = *deck->front().getPrevious();

		path->push_back(*end);

		deck->pop_front();

		if (backtrack == *begin) {
			path->push_back(*begin);
		}
		else {
			for (list<DictionaryEntry>::iterator it = used_entries->begin(); it != used_entries->end(); ++it) {
				if (backtrack == *it->getWord()) {
					path->push_back(backtrack);
					backtrack = *it->getPrevious();

					used_entries->erase(it);

					it = used_entries->begin();
				}
				if (backtrack == *begin) {
					path->push_back(*begin);

					break;
				}
			}
		}
	}

	delete deck; deck = NULL;
	delete used_entries; used_entries = NULL;

	return path;
}

bool checkIfChangeMorph(string* one, string* two) {
	if (one->size() == two->size()) {
		int discrepancyNumber = 0;

		for (unsigned int i = 0; i < one->size(); i++) {
			if (one->at(i) != two->at(i)) {
				discrepancyNumber++;
			}

			if (discrepancyNumber >= 2) {
				return false;
			}
		}

		return (discrepancyNumber <=1);
	}

	return false;
}

bool checkIfLengthMorph(string* one, string* two) {
	if (((two->size() - one->size()) == 1) or ((one->size() - two->size()) == 1)) {

		if (one->size() > two->size()) {
			return checkIfLengthMorph(two, one);
		}

		int discrepancyNumber = 0;

		for (int i = 0; i < ((int)(one->size() + two->size()) / 2); i++) {
			if (one->at(i) != two->at(i + discrepancyNumber)) {
				discrepancyNumber++;
				i--;
			}

			if (discrepancyNumber >= 2) {
				return false;
			}
		}

		return (discrepancyNumber <=1);
	}

	return false;
}

void printPathWords(vector<string>* path) {
	ostringstream ss;

	ss << "Words in morph: " << (int)path->size() << "\n";
	
	for (int i = (int)path->size() - 1; i >= 0; i--) {
		ss << path->at(i) << "\n";
	}

	cout << ss.str();

	return;
}

void printPathModifications(vector<string>* path) {
	ostringstream ss;

	ss << "Words in morph: " << (int)path->size() << "\n";

	ss << path->back() << "\n";

	for (int i = (int)path->size() - 1; i > 0; i--) {
		if (path->at(i).length() == path->at(i - 1).length()) {
			int position = find_positional_difference(&path->at(i), &path->at(i - 1));

			ss << "c," << position << "," << path->at(i - 1).at(position) << "\n";
		}
		else if (path->at(i).length() > path->at(i - 1).length()) {
			ss << "d," << find_positional_difference(&path->at(i), &path->at(i - 1)) << "\n";
		}
		else {
			int position = find_positional_difference(&path->at(i), &path->at(i - 1));

			ss << "i," << position << "," << path->at(i - 1).at(position) << "\n";
		}
	}

	cout << ss.str();

	return;
}

int find_positional_difference(string* current, string* next) {
	int position = -1;

	for (int i = 0; i < (int)current->length(); i++) {
		if (i == (int)next->length()) {
			position = i;
			break;
		}

		if (current->at(i) != next->at(i)) {
			position = i;
			break;
		}
	}

	if (position == -1) {
		position = (int)next->length() - 1;
	}

	return position;
}


void add_word_to_deque_from_dictionary(deque<DictionaryEntry>* deck, string* entry, DictionaryEntry* current) {

	deck->push_front(DictionaryEntry(entry, current->getWord()));

	return;
}
