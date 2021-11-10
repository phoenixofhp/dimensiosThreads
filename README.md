# dimensiosThreads
Basic multithreading program

Calculates amount of points in multidimension space that fits in outer circle layer

constexpr int THREADS- cores amount

constexpr int DIMENSIONS- takes too long to render if DIMENSIONS >30

constexpr int AMOUNT- amount of points

constexpr double RADIUS- radius of sphere, need to be <=1.0 due to the double overflow

constexpr double EPSILON- thickness of sphere outer layer, need to be <=1.0 due to the double overflow

#define THREAD_LOOP- always on

#define DEBUG_ON- sends to console progress info- slow down render, when DIMENSION<25
