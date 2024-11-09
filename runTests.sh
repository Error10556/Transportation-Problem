make -s

for file in inputs/test*.txt; do
    echo "Running $file"
    ./transport.bin < $file
    echo
    echo
done

