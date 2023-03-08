# Last modified: 27-03-2022
#
# To compile prog:
#    make
# To run all tests:
#    make t
#----------------------------------------------------------------------


bin: *.c
	cc -g -Wall -O3 basic_functions.c ui_functions.c main_functions.c main.c -o cot
