#!/bin/bash

# Check if the compile flag is specified
if [ "$1" == "--compile" ]; then
    # Compile the C++ program
    g++ src/sol.cpp -o bin/sol.exe
    
    # Check if compilation was successful
    if [ $? -ne 0 ]; then
        echo "Compilation failed."
        exit 1
    fi
fi

# Run the compiled C++ program
./bin/sol.exe

# Check if the C++ program ran successfully
if [ $? -ne 0 ]; then
    echo "C++ program execution failed."
    exit 1
fi

# Activate the virtual environment
source venv/bin/activate

# Check if activation was successful
if [ $? -ne 0 ]; then
    echo "Failed to activate virtual environment."
    exit 1
fi

# Run the Python script
python3 src/sim.py

# Check if the Python script ran successfully
if [ $? -ne 0 ]; then
    echo "Python script execution failed."
    deactivate
    exit 1
fi

# Deactivate the virtual environment
deactivate
