# jobSchedule

A simple program to perform task with dependencies. Tasks defined in files *.task.

Compiled on Linux:
```
g++ -std=c++11  jobScheduler.cpp -o jobScheduler
```

Test set in testSet directory. Program can be run:
```
./jobScheduler A testSet/A.task testSet/B.task testSet/C.task testSet/D.task testSet/E.task testSet/F.task testSet/G.task
```
