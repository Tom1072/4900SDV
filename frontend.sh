#!/bin/bash
Help()
{
   # Display Help
   echo "This file provides frontend utility scripts."
   echo "Only the FIRST argument is taken if provided multiple."
   echo
   echo "Dependencies: python3, python-tk, pip"
   echo "Syntax: ./frontend.sh [-h|i|r]"
   echo "Options:"
   echo " -h   Displays help"
   echo " -i   Installs depenencies and build executable"
   echo " -r   Runs the frontend executable"
   echo " -c   Clean up frontend build (including executables)"
   echo
}

# Get the options
while getopts ":hirc" option; do
   case $option in
      h) # display Help
         Help
         exit;;
      i) # install and build executable
         case $OSTYPE in
         "msys")
            echo "Installing for Windows"
            python -m venv ./venv
            source ./venv/Scripts/activate
            pip3 install -r ./src/frontend/requirements.txt
            pushd src/frontend/
            pyinstaller --onefile Display.py
            popd
            deactivate 
            ;;
         *)
            echo "Installing for Linux/MacOS"
            python3 -m venv ./venv
            source ./venv/bin/activate
            pip3 install -r ./src/frontend/requirements.txt
            cd src/frontend/
            pyinstaller --onefile Display.py
            cd ../../
            deactivate 
            ;;
         esac
         exit;;
      r) # run frontend
         ./src/frontend/dist/Display
         exit;;
      c) # clean up frontend build (including executables)
         rm -rf venv
         cd src/frontend
         rm -rf Display.spec dist/ build/
         cd ../../
         exit;;
   esac
done

echo "Error: Invalid argument"