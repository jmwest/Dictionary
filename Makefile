#Setting gcc 4.8.2
PATH := /usr/um/gcc­4.8.2/bin:$(PATH)
LD_LIBRARY_PATH := /usr/um/gcc­4.8.2/lib64
LD_RUN_PATH := /usr/um/gcc­4.8.2/lib64

FLAGS = -Wall -Wextra -pedantic -O3 -Wvla -std=c++11

letter : letter.cpp
	g++ $(FLAGS) letter.cpp -o letter

test_all : test1 test2 test3 test4 test5 test6
	diff -q test1.out test-1-rat-bin-qcm-correct.txt
	diff -q test2.out test-2-sea-nifty-sclw-correct.txt
	diff -q test3.out test-3-aardvark-aardvark-slm-correct.txt
	diff -q test4.out test-4-Morphing-Morphine-qclm-correct.txt
	diff -q test5.out test-5-mad-Made-qclw-correct.txt
	diff -q test6.out test-6-sea-nifty-sclw-correct.txt

mem_check_all : mem_check1 mem_check2 mem_check3 mem_check4 mem_check5 mem_check6

test1 : letter
	./letter -q -c -b rat -e bin -o M < test-1-rat-bin-qcm.txt > test1.out

test2 : letter
	./letter -s -c -l -b sea -e nifty -o W < test-2-sea-nifty-sclw.txt > test2.out

test3 : letter
	./letter -s -l -b aardvark -e aardvark -o M < test-3-aardvark-aardvark-slm.txt > test3.out

test4 : letter
	./letter -q -c -l -b Morphing -e Morphine -o M < test-4-Morphing-Morphine-qclm.txt > test4.out

test5 : letter
	./letter -q -c -l -b mad -e Made -o W < test-5-mad-Made-qclw.txt > test5.out

test6 : letter
	./letter -s -c -l -b sea -e nifty -o W < test-6-sea-nifty-sclw.txt >test6.out

mem_check1 : letter
	valgrind --leak-check=full --show-reachable=yes --track-origins=yes ./letter -q -c -b rat -e bin -o M < test-1-rat-bin-qcm.txt

mem_check2 : letter
	valgrind --leak-check=full --show-reachable=yes --track-origins=yes ./letter -s -c -l -b sea -e nifty -o W < test-2-sea-nifty-sclw.txt

mem_check3 : letter
	valgrind --leak-check=full --show-reachable=yes --track-origins=yes ./letter -s -l -b aardvark -e aardvark -o M < test-3-aardvark-aardvark-slm.txt

mem_check4 : letter
	valgrind --leak-check=full --show-reachable=yes --track-origins=yes ./letter -q -c -l -b Morphing -e Morphine -o M < test-4-Morphing-Morphine-qclm.txt

mem_check5 : letter
	valgrind --leak-check=full --show-reachable=yes --track-origins=yes ./letter -q -c -l -b mad -e Made -o W < test-5-mad-Made-qclw.txt

mem_check6 : letter
valgrind --leak-check=full --show-reachable=yes --track-origins=yes ./letter -s -c -l -b sea -e nifty -o W < test-6-sea-nifty-sclw.txt

clean :
	rm letter *.o

# Copy letter.cpp through scp to CAEN
# scp /Users/johnwest/Documents/University\ of\ Michigan/Junior/Semester\ 6/EECS\ 281/EECS\ 281\ Project\ 1/letter.cpp johnwest@login-course.engin.umich.edu:/afs/umich.edu/user/j/o/johnwest/eecs281/project1/