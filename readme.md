## Compile

``
mkdir build
``

``
cd ./build
``

``
cmake ..
``

``
make ..
``

## Usage

``
mbcd fp pf=[0,1] sp=[0,1,2] alpha=[10,80]
``

This executable accepts the following parameters:

- "fp": the path of the selected dataset

- "pf": a flag for the advanced pruning techniques

- "sp": "0" for DMBC, "1" for DMBC(SO), "2" for DMBC-H

- "alpha": a parameter for DMBC-H
