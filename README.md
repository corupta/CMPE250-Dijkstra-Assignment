I've used dijkstra algorithm to efficiently calculate distances to each town.

To overcome the thief challange, I used bitmasking.

Since, there are at most 13 different thief types, 2^13 = 8192 possible thief / coin configurations exist.

A coin configuration (bitmask) is sufficent for a thief configuration (bitmask) if
all bits in the thief configuration are also present in the coin configuration.
((coin & thief) == thief)

To combine the coin conguration of the towns just use the operator or, 
since the number of coins of each type is irrelevant. a 1 bit in any position means that coin is present.
For example town A has coins 1 2 3, B has coins 3 4 5 and there are at most 8 coins
Their coin bitmasks are A:00000111 and B:00011100 and when they are combined, it results in
A|B:00011111

I decided to define the vertex as (townId, coinConfiguration) 
where townId is between 1 and 2000, coinConfiguration is between 1 and 8192.
Thus, there can be 2000 * 8192 = 16384000 ~ 1.6m vertices at most in one test case.
Similarly since there are at most 3000 roads, there are at most 3000 * 8192 = 2457600 ~ 2.4m 0 edges
(when each coin configuration is considered). So V is 1.6m and E is 2.4m

Since dijkstra runs in O(|E+V|log|V|) in worst case, 
That is around (2.4m + 1.6m)log1.6m = 4m * 20.6 = 82.4m which is fast enough to run in a few seconds.    

#### _Tried Test Cases sent via mail._

- - - -

# CMPE250_2017Fall_Project3

Due date: 7.12.2017 23:59

Please check out Project3.pdf for description of the project.

Due date is a strict due date!

## How to compile

In a terminal, call commands:
```
>cmake CMakeLists.txt

>make

OR

>cmake CMakeLists.txt && make

```
Make sure the executable is produced.

Then you can test the project with the command:
```
>./project3 inputFile outputFile
```
