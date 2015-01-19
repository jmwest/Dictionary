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
private:
	string* word;
	string* previous;

public:
	DictionaryEntry();
	DictionaryEntry(string* word_entry);
	DictionaryEntry(string* word_entry, string* previous_entry);
	~DictionaryEntry();

	void setWord(string* word_in);
	void setPrevious(string* previous_in);

	string* getWord() const;
	string* getPrevious() const;
};

DictionaryEntry::DictionaryEntry()
{
	word = NULL;
	previous = NULL;
}

DictionaryEntry::DictionaryEntry(string* word_entry)
{
	word = new string(*word_entry);
	previous = NULL;
}

DictionaryEntry::DictionaryEntry(string* word_entry, string* previous_entry)
: word(word_entry), previous(previous_entry)
{
	word = new string(*word_entry);
	previous = new string(*previous_entry);
}

void DictionaryEntry::setWord(string* word_in)
{
	word = new string(*word_in);
}

void DictionaryEntry::setPrevious(string* previous_in)
{
	previous = new string(*previous_in);
}

string* DictionaryEntry::getWord() const
{
	return new string(*word);
}

string* DictionaryEntry::getPrevious() const
{
	return new string(*previous);
}

DictionaryEntry::~DictionaryEntry()
{
	if (word != NULL) {
		delete word; word = NULL;
	}

	if (previous != NULL) {
		delete previous; previous = NULL;
	}
}

#endif /* defined(__EECS_281_Project_1__Dictionary__) */
