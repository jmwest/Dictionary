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

// REQUIRES:
// MODIFIES: begin, end, rout, modify, and output.
// EFFECTS: uses argc and argv that are passed into the program
//			to give values to begin, end, rout, modify, and output.
void parse_command_line_input(int & argc, char *argv[], string &begin, string &end,
							  Routing &rout, Modification &modify, Output &output);

// REQUIRES: begin and end are non-NULL pointers
// MODIFIES:
// EFFECTS: Returns a pointer to a list of strings that contains
//			all the words in the entered dictionary.
//			If modify == CHANGE, only words of the same length as
//			begin are added to the dictionary
list<string>* store_dictionary_in_data_structure(const string* begin, const string* end,
												 const Modification &modify);

// REQUIRES: begin, end, and dictionary are non-NULL pointers
// MODIFIES: dictionary
// EFFECTS: Returns a pointer to a vector of strings that contains
//		the path from begin to end through the dictionary.
// ->at(0) contains *end, and ->back() contains *begin
vector<string>* find_lettermans_path(const Routing &rout, const Modification &modify,
									 string* begin, string* end,
									 list<string>* dictionary);

// REQUIRES: one and two are non-NULL pointers
// MODIFIES:
// EFFECTS: compares one to two to determine if one and two are a
//			valid change word morph
bool check_if_change_morph(const string* one, const string* two);

// REQUIRES: one and two are non-NULL pointers
// MODIFIES:
// EFFECTS: compares one to two to determine if one and two are a
//			valid length word morph
bool check_if_length_morph(const string* one, const string* two);

// REQUIRES: path is a non-NULL pointer
// MODIFIES:
// EFFECTS: prints out the words in path in the W format
void print_path_words(const vector<string>* path);

// REQUIRES: path is a non-NULL pointer
// MODIFIES:
// EFFECTS: prints out the words in path in the M format
void print_path_modifications(vector<string>* path);

// REQUIRES: current and next are non-NULL pointers
// MODIFIES:
// EFFECTS: returns the index at which the string current differs
//			from the string next
int find_positional_difference(const string* current, const string* next);

// REQUIRES: deck, entry, and current are all non-NULL pointers
// MODIFIES: deck
// EFFECTS: creates a new DictionaryEntry out of entry and current and
//			adds it to the front of deck
void add_word_to_deque_from_dictionary(deque<DictionaryEntry>* deck, string* entry,
									   DictionaryEntry* current);

// REQUIRES:
// MODIFIES:
// EFFECTS: Prints the help message for when the --help or -h flags are input
void print_help_message();

int main(int argc, char *argv[]) {

	//valgrind will report memory leak when sync_with_stdio is false
	ios_base::sync_with_stdio(false);
	////////////////////////////////////////////////////////////////

	string begin;
	string end;
	list<string>* dictionary;
	vector<string>* path;
	Routing rout = noROUT;
	Modification modify = noMOD;
	Output outp = DEFAULTOUTPUT;

	parse_command_line_input(argc, argv, begin, end, rout, modify, outp);

	dictionary = store_dictionary_in_data_structure(&begin, &end, modify);

	// Check for begin and end being the same word
	if (begin == end) {
		path = new vector<string>(1, begin);
	}
	else {
		path = find_lettermans_path(rout, modify, &begin, &end, dictionary);
	}

	if (path->size() == 0) {
		ostringstream ss;

		ss << "No solution.\n";

		cout << ss.str();
	}
	else {
		if (outp == MOUT) {
			print_path_modifications(path);
		}
		else {
			print_path_words(path);
		}
	}

	delete dictionary; dictionary = NULL;
	delete path; path = NULL;

	return 0;
}

void parse_command_line_input(int & argc, char *argv[], string &begin, string &end, Routing &rout, Modification &modify, Output &output) {
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

	// Use getopt to parse through command line input
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
				print_help_message();

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

list<string>* store_dictionary_in_data_structure(const string* begin, const string* end,
												 const Modification &modify) {
	list<string>* dictionary;

	string size_in;
	int size = 0;
	string entry;
	bool beginIsInDictionary = false;
	bool endIsInDictionary = false;

	getline(cin, size_in);

	size = atoi(size_in.c_str());

	// If change is the only modification allowed, all words that are
	//	input that have a different length from that of begin and end
	//	can be ignored. The first block of code in the IF statement
	//	has a varied loop condition from the ELSE, since the dictionary
	//	size can't be pre-determined for change only modification.
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

		for (list<string>::iterator it = dictionary->begin();
			 getline(cin, entry) and (it != dictionary->end()); ) {
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

vector<string>* find_lettermans_path(const Routing &rout, const Modification &modify,
									string* begin, string* end,
									list<string>* dictionary) {
	vector<string>* path = new vector<string>();
	list<DictionaryEntry>* used_entries = new list<DictionaryEntry>();

	deque<DictionaryEntry>* deck = new deque<DictionaryEntry>();

	DictionaryEntry current_entry;

	deck->push_front(DictionaryEntry(begin));

	// Find begin in dictionary and delete it
	for (list<string>::iterator it = dictionary->begin(); it != dictionary->end(); ++it) {
		if (*it == *begin) {
			dictionary->erase(it);

			break;
		}
	}

	bool reachedTheEnd = false;

	// Continue to take the next word out of deck and find all of its
	//	word morphs until end is reached or deck is empty
	while ((deck->size() != 0) and !reachedTheEnd) {
		if (rout == STACK) {
			current_entry = deck->front();
			deck->pop_front();
		}
		else {
			current_entry = deck->back();
			deck->pop_back();
		}

		// Loop through the dictionary and find all the word morphs for
		//	the current word
		for (list<string>::iterator it = dictionary->begin(); it != dictionary->end(); ++it) {
			// Don't need to do any more with a word if it's length is more than 1
			//	different from the current word
			if (((int)(current_entry.getWord()->length() - it->length()) <= 1)
				and ((int)(it->length() - current_entry.getWord()->length()) <= 1)) {

				if (modify == BOTH) {
					if (check_if_change_morph(current_entry.getWord(), &(*it))
						or check_if_length_morph(current_entry.getWord(), &(*it))) {
						add_word_to_deque_from_dictionary(deck, &(*it), &current_entry);

						list<string>::iterator invalidated_it = it;
						it--;

						dictionary->erase(invalidated_it);
					}
				}
				else if (modify == LENGTH) {
					if (check_if_length_morph(current_entry.getWord(), &(*it))) {
						add_word_to_deque_from_dictionary(deck, &(*it), &current_entry);

						list<string>::iterator invalidated_it = it;
						it--;
						
						dictionary->erase(invalidated_it);
					}
				}
				else if (modify == CHANGE) {
					if (check_if_change_morph(current_entry.getWord(), &(*it))) {
						add_word_to_deque_from_dictionary(deck, &(*it), &current_entry);

						list<string>::iterator invalidated_it = it;
						it--;
						
						dictionary->erase(invalidated_it);
					}
				}

				// Check to see if the newest found word is end
				if (deck->size() != 0) {
					if (*deck->front().getWord() == *end) {
						reachedTheEnd = true;
						break;
					}
				}
			}
		}

		// Keep track of all entries found so the path can be found later.
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
			// Search through the used entries to find the way back from
			//	end to begin
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

bool check_if_change_morph(const string* one, const string* two) {
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

bool check_if_length_morph(const string* one, const string* two) {
	if (((two->size() - one->size()) == 1) or ((one->size() - two->size()) == 1)) {
		// Swap one and two then call self since the length of one
		//	must be greater than the length of two
		if (one->size() > two->size()) {
			return check_if_length_morph(two, one);
		}

		int discrepancyNumber = 0;

		// Loop through both string indeces and check if the letters are
		//	the same. If the letters are different the indexes shift
		//	and then the search continues unless a second difference
		//	is zero
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

void print_path_words(const vector<string>* path) {
	ostringstream ss;

	ss << "Words in morph: " << (int)path->size() << "\n";
	
	for (int i = (int)path->size() - 1; i >= 0; i--) {
		ss << path->at(i) << "\n";
	}

	cout << ss.str();

	return;
}

void print_path_modifications(vector<string>* path) {
	ostringstream ss;

	ss << "Words in morph: " << (int)path->size() << "\n";

	ss << path->back() << "\n";

	for (int i = (int)path->size() - 1; i > 0; i--) {
		// The three different ways to change a word; insertion, deletion,
		//	and change; are checked for the appropriate printing option
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

int find_positional_difference(const string* current, const string* next) {
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


void add_word_to_deque_from_dictionary(deque<DictionaryEntry>* deck, string* entry,
									   DictionaryEntry* current) {

	deck->push_front(DictionaryEntry(entry, current->getWord()));

	return;
}

void print_help_message() {
	cerr << "\n///////////////////////////////////////////////////////";
	cerr << "/////////////////////";
	cerr << "\nThis program helps Letterman find his way from one word";
	cerr << " to another!\nHe uses a dictionary provided by the user";
	cerr << " to tackle some tricky word morphs.\n\n";
	
	cerr << "Command Line Input Options:\n";
	cerr << "--------------------------------------------------------";
	cerr << "--------------------\n";
	
	cerr << "--stack (-s)\t: Runs the program using a stack based\n";
	cerr << "\t\trouting scheme for determining the word path.\n\n";
	cerr << "--queue (-q)\t: Runs the program using a queue based routing\n";
	cerr << "\t\tscheme for determining the word path.\n\n";
	
	cerr << "\t\tOnly one of stack and queue may be specified.\n\n";
	
	cerr << "--change (-c)\t: Runs the program allowing Letterman to change";
	cerr << "\n\t\tone letter into another.\n\n";
	cerr << "--length (-l)\t: Runs the program allowing Letterman to modify";
	cerr << "\n\t\tthe length of a word by inserting or deleting a letter.\n";
	cerr << "\n\t\tEither change or length or both may be specified.\n\n";
	
	cerr << "--output <W|M>\t: Requires either W (for word) or M";
	cerr << " (for modification)\n  (-o <W|M>)\tfollow the flag to specify output type.\n\n";
	cerr << "\t\tIf neither W or M are specified, the program defaults to W.\n\n";
	
	cerr << "--begin <word>\t: Specifies the word Letterman ";
	cerr << "starts with.\n  (-b <word>)\n\n";
	cerr << "--end <word>\t: Specifies the word Letterman ";
	cerr << "ends with.\n  (-e <word>)\n\n";
	
	cerr << "--help (-h)\t: Outputs the message that was previously output\n";
	cerr << "///////////////////////////////////////////////////////";
	cerr << "/////////////////////\n\n";

	return;
}

