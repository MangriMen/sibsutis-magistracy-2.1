#!/bin/bash

should_clean="$1"

mkdir -p results

echo "Running benchmarks..." > results/benchmarks_results.txt
echo "====================" >> results/benchmarks_results.txt
echo "" >> results/benchmarks_results.txt

for benchmark in benchmarks/*/; do
    if [ -d "$benchmark" ]; then
        name=$(basename "$benchmark")
        echo "Processing: $name"
        
        echo "Benchmark: $name" >> results/benchmarks_results.txt
        echo "------------------------" >> results/benchmarks_results.txt
        
        (cd "$benchmark" && make -j) > /dev/null 2>&1
        (cd "$benchmark" && ./bin/"$name") >> results/benchmarks_results.txt 2>&1
        if [ -n "$should_clean" ]; then
            (cd "$benchmark" && make clean) > /dev/null 2>&1
        fi

        echo "" >> results/benchmarks_results.txt
    fi
done

echo "All benchmarks completed. Check results/benchmarks_results.txt"