# Usermods API v2 example usermod

In this usermod file you can find the documentation on how to take advantage of the new version 2 usermods!

## Installation

Copy `usermod_v2_example.h` to the wled00 directory.
Uncomment the corresponding lines in `usermods_list.cpp` and compile!
_(You shouldn't need to actually install this, it does nothing useful)_


curl -X POST -H "Content-Type: application/json" -d {"staircase":{"enabled":true}} http://192.168.1.84/json/state
curl -X POST -H "Content-Type: application/json" -d {"staircase":{"enabled":false}} http://192.168.1.84/json/state
curl -X POST -H "Content-Type: application/json" -d '{"staircase":{"bottom-sensor":true}}' http://192.168.1.84/json/state
curl -X POST -H "Content-Type: application/json" -d '{"staircase":{"top-sensor":true}}' http://192.168.1.84/json/state

