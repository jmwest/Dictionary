//
//  letter.cpp
//  EECS 281 Project 1
//
//  Created by John West on 1/13/15.
//  Copyright (c) 2015 John West. All rights reserved.
//

#include "letter.h"

#include <cstdlib>
#include <iostream>
#include <getopt.h>

#include <string>
#include <sstream>

using namespace std;

void parseCommandLineInput(int & argc, char *argv[], )

void runLetterManWithStack();
void runLetterManWithQueue();

enum Routing {noROUT, STACK, QUEUE};
enum Modification {noMOD, CHANGE, LENGTH, BOTH};
enum Output {DEFAULTOUTPUT, WOUT, MOUT};

int main(int argc, char *argv[])
{
	//valgrind will report memory leak when sync_with_stdio is false
	ios_base::sync_with_stdio(false);

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
	string begin;
	string end;
	Routing rout = noROUT;
	Modification modify = noMOD;
	Output outp = DEFAULTOUTPUT;
	
	while ((c = getopt_long(argc, argv, "sqclo:b:e:h", longopts, &idx)) != -1) {
		switch (c) {
			case 's':
				if (rout == QUEUE) {
					cerr << "Stack and Queue were both specified. Program exit(1)\n";
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
					outp = MOUT;
				}
				else if (*optarg == 'W') {
					outp = WOUT;
				}
				else {
					cerr << "Invalid output character. Program exit(1)\n";
					exit(1);
				}
		break;
			case 'b':
				begin = optarg;
		break;
			case 'e':
				end = optarg;
		break;
			case 'h':
				cerr << "Useful help message here.\n";
				exit(0);
		break;
			default:
		break;
		}
	}
	
}