# Graph Project

This was a project from my discrete math II class (due to 2022/08/16)
which it asked us to do a code in C to solve a graph problem, listed
as followings:

 1. Detect all connected components in graph
 2. Check if two vertices are reachable
 3. Find all short paths from a start vertex, short path is defined as
   the lower number of vertices in a path
 4. Find all weak paths from a start vertex, weak path is defined as
   the lower sum of weights in a path

This branch is to keep the sent version to professor as backup.

Note: messages and documentation are in spanish

### Build

It is based on C99 (more precisely GNU99) Standard and it doesn't use
any dependency or external library

- `make all` -- builds everything
- `make build` -- compiles the library
- `make clear` -- erases/clears all build files

and it will be generated in `build/` directory

### Input File Structure

File is given by a sequential data formatted as follwing:
 ```
 vertex_size edge_size
 vertex_1 vertex_2 weight_1 \
 ...      ...      weight_2  |
 .        .        .          >--- size of edges
 .        .        .         |
 vertex_n vertex_m weight_p /
 ```
Where each data is a integer value, and vertex should be a value of
interval (0, vertex_size]
