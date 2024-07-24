#!/bin/bash

# Function to compile the C++ program
compile_program() {
    # Check if the bin directory exists, if not create it
    if [ ! -d "bin" ]; then
        mkdir bin
    fi
    
    # Compile the C++ program
    g++ src/*.cpp -I ./include -o bin/sol.exe
    
    # Check if compilation was successful
    if [ $? -ne 0 ]; then
        echo "Compilation failed."
        exit 1
    fi
}

# Function to run the C++ program
run_cpp_program() {
    if [ "$target_centered_flag" = true ]; then
        ./bin/sol.exe --centered
    else
        ./bin/sol.exe
    fi
    
    # Check if the C++ program ran successfully
    if [ $? -ne 0 ]; then
        echo "C++ program execution failed."
        exit 1
    fi
}

# Function to run the Python script
run_python_script() {
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
}

# Function to delete output files
clean_output_files() {
    rm io_files/UAV*.txt
}

# Parse command line arguments
compile_flag=false
target_centered_flag=false
clean_output_flag=false
run_python_flag=false
run_cpp_flag=false

for arg in "$@"; do
    case $arg in
        --compile)
            compile_flag=true
            shift
            ;;
        --target-centered)
            target_centered_flag=true
            shift
            ;;
        --clean-output)
            clean_output_flag=true
            shift
            ;;
        --run-graphics)
            run_python_flag=true
            shift
            ;;
        --run-program)
            run_cpp_flag=true
            shift
            ;;
        *)
            shift
            ;;
    esac
done

# If compile flag is set, compile the program
if [ "$compile_flag" = true ]; then
    compile_program
fi

# If run-cpp flag is set, run the C++ program
if [ "$run_cpp_flag" = true ]; then
    run_cpp_program
fi

# If run-python flag is set, run the Python script
if [ "$run_python_flag" = true ]; then
    run_python_script
fi

# If clean-output flag is set, delete output files
if [ "$clean_output_flag" = true ]; then
    clean_output_files
fi
