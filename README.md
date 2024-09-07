# Parallel Solutions of The Longest Common Subsequence Algorithm With OpenMP

This repository contains two versions of parallel implementations for determining the length of the longest common subsequence (LLCS) given a set of sequences. Both solutions use cache-aware algorithms and scan the matrix anti-diagonally to enable parallel computations of the elements. All speedups are measured relative to the serial version, which took 14.55 seconds.

## Implementations

### V1: Explicit Tasks
- **Algorithm**:
  - Iterate over the anti-diagonals of the matrix.
  - Identify the top and bottom blocks in each anti-diagonal.
  - Create tasks for each block, defining dependencies between them.
  - Process blocks in parallel, synchronizing  shared updates using `#pragma omp atomic` directive.
  
- **Dependencies**:
  - Tasks require synchronization, waiting for blocks to the left and above to complete.
  - Input dependencies are set to ensure the currently running task is dependent on the last elements of the left and top blocks.
  - Output dependencies ensure subsequent tasks rely on the current block’s last row and column.

- **Data Scoping**:
  - `row`, `col`, and `count` variables are defined as `firstprivate` to ensure each task has independent and predefined local copies.

- **Granularity**:
  - The number of tasks depends on the block size.
  
- **Performance**:

  | Threads | Block Size | Seconds | Speedup |
  |---------|------------|---------|---------|
  | 16      | 200        | 1.40    | 10.39   |
  | 16      | 256        | 1.33    | 10.93   |
  | 16      | 512        | 1.28    | 11.36   |
  | 32      | 200        | 1.14    | 12.76   |
  | 32      | 256        | 1.16    | 12.54   |
  | 32      | 512        | 1.19    | 12.22   |

### V2: Taskloop
- **Algorithm**:
  - Iterate over the anti-diagonals of the matrix.
  - Identify the top and bottom blocks in each anti-diagonal.
  - Iterate the blocks in the antidiagonals using `taskloop`.
  - Process blocks in parallel, synchronizing via the `reduction` clause for shared updates.

- **Data Scoping**:
  - `row` and `col` variables are defined as `private` to ensure each task has its local copy.

- **Granularity**:
  - The number of tasks is determined by block size and the `grainsize` clause.
  - Block size of 256 with grainsize of 1 produces 200 tasks.

- **Performance**:

  | Threads | Grain Size | Seconds | Speedup |
  |---------|------------|---------|---------|
  | 16      | 1          | 1.36    | 10.69   |
  | 16      | 2          | 1.41    | 10.31   |
  | 16      | 3          | 1.45    | 10.03   |
  | 32      | 1          | 1.36    | 10.69   |
  | 32      | 2          | 1.41    | 10.31   |
  | 32      | 3          | 1.45    | 10.03   |


## Conclusion
The V1 implementation using explicit tasks outperforms V2, which uses `taskloop`. This is because explicit tasks offer more flexible synchronization by allowing task dependencies. In V2, `taskloop` processes all blocks in an antidiagonal like a `taskgroup`, waiting for all to finish before moving to the next. In contrast, V1 defines dependencies between tasks, allowing parallelism to be utilized more efficiently without waiting for all blocks in an antidiagonal to complete.