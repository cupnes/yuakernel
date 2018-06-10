#!/bin/bash

# create default_handler.s
for n in $(seq 21 255); do
	sed "s/\[n\]/${n}/g" handler_template.s
	echo
done >default_handler.s

# create default_intr.h
echo '#ifndef _DEFAULT_INTR_H_' >default_intr.h
echo '#define _DEFAULT_INTR_H_' >>default_intr.h
echo >>default_intr.h
for n in $(seq 21 255); do
	echo "void default_handler_${n}(void);"
done >>default_intr.h
echo >>default_intr.h
echo '#endif' >>default_intr.h

# create default_intr.c
echo '#include <fb.h>' >default_intr.c
echo '#include <fbcon.h>' >>default_intr.c
echo >>default_intr.c
for n in $(seq 21 255); do
	sed "s/\[n\]/${n}/g" handler_template.c
	echo
done >>default_intr.c

# create intr_init.c
for n in $(seq 21 255); do
	echo -e "\tset_intr_desc(${n}, default_handler_${n});"
done >intr_init.c
