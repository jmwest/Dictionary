//
//  DictionaryEntry.cpp
//  EECS 281 Project 1
//
//  Created by John West on 1/15/15.
//  Copyright (c) 2015 John West. All rights reserved.
//

#include "DictionaryEntry.h"

DictionaryEntry::DictionaryEntry()
{
	word = "";
	previous = "";
}

DictionaryEntry::DictionaryEntry(string* word_entry)
: word(*word_entry)
{
	previous = "";
}

DictionaryEntry::DictionaryEntry(string* word_entry, string*previous_entry)
: word(*word_entry), previous(*previous_entry)
{}

string* DictionaryEntry::getWord()
{
	return &word;
}

string* DictionaryEntry::getPreviousWord()
{
	return &previous;
}

void DictionaryEntry::setPreviousEntry(string* previous_entry)
{
	previous = *previous_entry;

	return;
}