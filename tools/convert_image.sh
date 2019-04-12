#!/bin/bash

set -ue
# set -uex

if [ $# -ne 4 ]; then
	echo "$0 src_img dst_dir width height" > /dev/stderr
	exit 1
fi

src_img=$1
dst_dir=$2
width=$3
height=$4

img_name=$(basename $src_img | rev | cut -d'.' -f2- | rev)

convert $src_img -resize ${width}x${height}! -depth 8 ${dst_dir}/${img_name}.bgra

rm -f ${dst_dir}/${img_name}_h.dat

width_hex=$(printf '%08x\n' $width)
for i in $(seq 4); do
	echo -en "\x$(echo $width_hex | rev | cut -c-2 | rev)" >> ${dst_dir}/${img_name}_h.dat
	width_hex=$(echo $width_hex | rev | cut -c3- | rev)
done

height_hex=$(printf '%08x\n' $height)
for i in $(seq 4); do
	echo -en "\x$(echo $height_hex | rev | cut -c-2 | rev)" >> ${dst_dir}/${img_name}_h.dat
	height_hex=$(echo $height_hex | rev | cut -c3- | rev)
done

cat ${dst_dir}/${img_name}_h.dat ${dst_dir}/${img_name}.bgra > ${dst_dir}/i.${img_name}

rm ${dst_dir}/${img_name}_h.dat ${dst_dir}/${img_name}.bgra
