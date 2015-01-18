#Setting gcc 4.8.2
PATH := /usr/um/gcc­4.8.2/bin:$(PATH)
LD_LIBRARY_PATH := /usr/um/gcc­4.8.2/lib64
LD_RUN_PATH := /usr/um/gcc­4.8.2/lib64

FLAGS = -Wall -Wextra -pedantic -Wvla -std=c++11





# Copy letter.cpp through scp to CAEN
# scp /Users/johnwest/Documents/University\ of\ Michigan/Junior/Semester\ 6/EECS\ 281/EECS\ 281\ Project\ 1/letter.cpp johnwest@login-course.engin.umich.edu:/afs/umich.edu/user/j/o/johnwest/eecs281/project1/