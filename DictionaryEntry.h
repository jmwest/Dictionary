//
//  DictionaryEntry.h
//  EECS 281 Project 1
//
//  Created by John West on 1/15/15.
//  Copyright (c) 2015 John West. All rights reserved.
//
// This is a structure used to store words for use in a dictionary.
// It contains the word entered in the dictionary, and the word that
// this entry came from in the Letterman chain.

#ifndef __EECS_281_Project_1__Dictionary__
#define __EECS_281_Project_1__Dictionary__

#include <stdio.h>

#include <string>

using namespace std;

struct DictionaryEntry {
//private:
//	string word;
//	string previous;

public:
	DictionaryEntry();
	DictionaryEntry(string* word_entry);
	DictionaryEntry(string* word_entry, string* previous_entry);

	string* word;
	string* previous;

//	string* getWord();
//	string* getPreviousWord();
//
//	void setPreviousEntry(string* previous_entry);
};

#endif /* defined(__EECS_281_Project_1__Dictionary__) */
