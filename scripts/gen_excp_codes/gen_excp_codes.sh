#!/bin/bash

# create handler.s
for e in $(grep -v '^#' excp_list.txt | cut -d',' -f4); do
	sed "s/\[name\]/$e/g" handler_template.s
	echo
done >handler.s

# create intr_handler.c
for e in $(grep -v '^#' excp_list.txt | cut -d',' -f4); do
	sed "s/\[name\]/$e/g" intr_handler_template.c | sed "s/\[msg\]/$(echo $e | tr '[:lower:]' '[:upper:]' | tr '_' ' ')/g"
	echo
done > intr_handler.c

# create intr_handler_declaration.c
for e in $(grep -v '^#' excp_list.txt | cut -d',' -f4); do
	echo "void do_${e}(void);"
done > intr_handler_declaration.c

# create intr_init.c
grep -v '^#' excp_list.txt | cut -d',' -f1,4 | awk 'BEGIN{FS=","}{print "\tset_intr_desc("$1", do_"$2");"}' > intr_init.c
