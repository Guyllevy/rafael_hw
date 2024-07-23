run on linux (or wsl)

-before everything create python virtual environment:

    python3 -m venv venv

-activate it:

    source venv/bin/activate

-install nessecery packages: (with activated venv)

    pip install pygame
    pip install numpy

-input wanted parameters and commands in io_files/SimCmds.ini, io_files/SimParams.ini

-run: (without venv activated)
    ./run.sh --compile --run-cpp --run-python --clean-output --target-centered



