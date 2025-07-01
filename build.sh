#!/bin/bash

#
# Usage: ./build.sh [TARGET]
#
# TARGET:
#     - build         - configure (if not yet done; clean first to force) and build project,
#     - test          - run tests      (if applicable),
#     - run           - run the binary (if applicable),
#     - all (default) - build + test (no warnings) + run (no warnings),
#     - clean         - remove the whole build.
#

# -- Component specific --
# Set to relative executable path, or leave empty/undefined if there is no executable.
#
# BIN_PATH=""
#

function warn() {
    if [ 0 -ne $1 ]; then
        YELLOW='\033[0;33m'
        DEF='\033[0m'
        printf "${YELLOW}$2${DEF}\n"
    fi
}

# Parse options.
if [ 0 -eq "$#" ]; then        BUILD=1; TEST=1; RUN=1; WARN=0;
elif [ "all" == "$1" ]; then   BUILD=1; TEST=1; RUN=1; WARN=1;
elif [ "build" == "$1" ]; then BUILD=1; TEST=0; RUN=0; WARN=1;
elif [ "test" == "$1" ]; then  BUILD=1; TEST=1; RUN=0; WARN=1;
elif [ "run" == "$1" ]; then   BUILD=1; TEST=0; RUN=1; WARN=1;
elif [ "clean" == "$1" ]; then BUILD=0; TEST=0; RUN=0; WARN=0;
fi

# Build.
if [ 0 -ne $BUILD ]; then
    if [ ! -d "build" ]; then # Configure only if not done yet. Clean first to force.
        cmake -B build -DCMAKE_EXPORT_PACKAGE_REGISTRY=ON # Configure.
    fi
    cmake --build build # Build.
else
    rm -rf build # Clean.
fi

# Test.
if [ 0 -ne $TEST ]; then
    if [ -d "build/test" ]; then
        ctest --test-dir build/test # Run tests.
    else
        warn $WARN "No tests to run."
    fi
fi

# Run.
if [ 0 -ne $RUN ]; then
    if [ "" != "$BIN_PATH" ]; then
        if [ -f $BIN_PATH ]; then
            ./$BIN_PATH
        else
            warn 1 "Binary defined, but not built."
        fi
    else
        warn $WARN "No binary defined (see BIN_PATH variable in this script)."
    fi
fi
