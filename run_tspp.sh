#!/bin/bash

g++ annealing.cpp -o tspp.out -O3

for DISTANCE_FILE in distance/*.txt; do
	# Catch distance file basename
	basename="$(basename -- $DISTANCE_FILE)"
	IFS='.' read -a strarr <<< "$basename"
	filename="${strarr[0]}"

	# for each problem, run all tspp's
	for PENALTY_FILE in penalty/$filename*; do
		basename="$(basename -- $PENALTY_FILE)"
		IFS='.' read -a strarr <<< "$basename"
		filename="${strarr[0]}"

		# run tspp
		ans="$(./tspp.out $DISTANCE_FILE $PENALTY_FILE)"
		echo -e "$filename\t$ans"
	done
done
