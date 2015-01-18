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
public:
	DictionaryEntry();
	DictionaryEntry(string* word_entry);
	DictionaryEntry(string* word_entry, string* previous_entry);
	~DictionaryEntry();

	string* word;
	string* previous;
};

DictionaryEntry::DictionaryEntry()
{
	word = new string();
	previous = new string();
}

DictionaryEntry::DictionaryEntry(string* word_entry)
: word(word_entry)
{
	previous = new string();
}

DictionaryEntry::DictionaryEntry(string* word_entry, string* previous_entry)
: word(word_entry), previous(previous_entry)
{}

DictionaryEntry::~DictionaryEntry()
{
	delete word; word = NULL;
	delete previous; previous = NULL;
}

#endif /* defined(__EECS_281_Project_1__Dictionary__) */
