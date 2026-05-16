# Patterns: Fork-join

- fork-join pattern can generate a high-level of parallelism
- serial divide-and-conquer algorithms can be efficiently parallelized with fork-join pattern
  - limits on speedup
  - most of the work should go deep into the recursion
- recursive approach to parallelism
  - need for work schedulers
- fork-join as a directed graph
  - control flow forks (divides) into multiple flows
    - one flow turns into more separate flows
    - each flow is independent and not constrained to do similar computation
  - multiple flows join (combine) the latter
    - after joining, only one flow continues
  - example: two tasks B() and C() are executed in parallel and joined afterward

## Divide-and-conquer

- typical divide-and-conquer pattern
  - sub-problems must be independent

  ```C
  void divideAndConquer(problem P)
    if (P is base case)
      solve P;
    else {
        divide P into b sub-problems
        fork to conquer each sub-problem in parallel
        join
        combine sub-solutions into final solution
    } 
  ```

- $b$ sub-problems on $L$ levels permit up to $b^L$ parallel tasks
- the vast majority of work must be deep in the recursion where the parallelism is high
- gor good performance, it is crucial to select the proper size of the base case
  - the recursion should not go too deep as scheduling overheads will start to dominate
  - the operations before the fork and after the join should be fast, so they do not strangle speedup

## Fork-join Programming Model

- CUDA: patterns with a lot of control do not fit well with GPU concepts
- MPI: dynamic changing of the number of processors makes it possible; not used much as the number of processes on clusters must be determined at the job submission stage
- OpenMP: from version 3.0 onwards, it has explicit support for tasks

### OpenMP support for tasks

- tasks are independent units of work
- tasks can be nested
- tasks present additional level of abstraction
- tasks are sent to a pool of tasks
- OpenMP scheduler picks tasks from the pool and assigns them to software threads
  - tasks may be executed immediately
  - tasks may be deferred (for example waiting for task dependencies to be fulfilled)
  - schedulers are implementation specific, not determined by standard

- OpenMP syntax
  - `#pragma omp task`
    - indicates that subsequent statements can be independently forked as tasks
    - by default, variables are of type `firstprivate`
  - join with #pragma omp `taskwait`
    - waits for all tasks to join
  - tasks can only be used inside a parallel region
    - only one thread (master) starts the execution
  - example: function `divideAndConquer` and its call from the main routine

    ```C
    solution divideAndConquer(problem) {
        subproblem1 = fork(problem, 1);
        subproblem2 = fork(problem, 2);
        #pragma omp task
        solution1 = divideAndConquer(subproblem1)
        solution2 = divideAndConquer(subproblem2)
        #pragma omp task wait
        solution = join(solution1, solution2);
    }

    int main(void) {
        #pragma omp parallel
        #pragma omp master
        divideAndConquer(problem);
        return 0;
    }
    ```

  - programmer has some control on forking
    - `#pragma omp task final(condition)`
      - when condition executes to `true`, new tasks are not generated anymore
      - the computation is performed inside the calling task

### Recursive Implementation of the Map-reduce Pattern

- adaptive quadrature using trapezoidal rule
- compare quadrature on two levels: if difference is grater than allowed, split interval to two halves and repeat quadrature on each halve
- code
  - [adaptquad_ser.c](files/adaptquad/adaptquad_ser.c) and [adaptquad_ser.sh](files/adaptquad/adaptquad_ser.sh): reference serial implementation
  - [adaptquad_par1.c](files/adaptquad/adaptquad_par1.c) and [adaptquad_par1.sh](files/adaptquad/adaptquad_par1.sh)
    - each task creates two sub-tasks
    - slow performance
  - [adaptquad_par2.c](files/adaptquad/adaptquad_par2.c) and [adaptquad_par2.sh](files/adaptquad/adaptquad_par2.sh)
    - added `final`clause to limit creation of parallel tasks deep in the recursion
  - [adaptquad_par3.c](files/adaptquad/adaptquad_par3.c) and [adaptquad_par3.sh](files/adaptquad/adaptquad_par3.sh)
    - each task creates only one sub-task
    - main tasks performs some computation on its own

### Choosing Base Cases

- when recursion goes to deep, scheduling overheads tend to swamp useful work
- two separate base cases at different levels
  - a base case for stopping parallel recursion
    - parallel task scheduling overheads
  - a base case for stopping serial recursion
    - function call overheads
    - less expensive compared to task scheduling overheads
  - serial recursion stops at much smaller problem sizes
- it is tempting to set the number of base cases equal to the number of parallel hardware threads
  - scheduler has no flexibility to balance load; even if problem is well balanced, operating system can cause issues
  - better is to over-decompose the problem and create some parallel slack

### Algorithm Complexity

- majority of problems can be described with relation 

  $t(N) = at({N\over b}) + cN^d\quad, \quad t(1)=\mathrm{e}$

- task on level $l$ has $cN^d$ work itself and task on level $l+1$ has $ac(\frac{N}{b})^d$ work itself
- asymptotic solutions
  - based on proportion $r = \frac{t_{l+1}(N)}{t_l(N)} = \frac{a}{b^d}$
  - case 1: $r > 1: t(N) = O(N^{\log_b A})$
    - the work exponentially increases with depth, bottom levels dominate
  - case 2: $r = 1: t(N) = O(N^d \log_2 N)$
    - the work at each level is about the same
    - the work is proportional to the work at top level times the number of levels
  - case 3: $r < 1: t(N) = O(N^d)$
    - the work exponentially decreases with depth, top levels dominate
- examples with $𝑐 = 𝑑 = 1$
