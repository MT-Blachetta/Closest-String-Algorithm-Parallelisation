# Closest String Algorithm – Parallelisation

This repository provides Python implementations of the **closest string
problem**, an optimisation task that searches for a string minimising the
maximum Hamming distance to a given set of input strings.  The project
contains a sequential solver and a parallel variant based on
``mpi4py``.

## Requirements

* Python 3.10 or newer
* For the MPI version: ``mpi4py`` and an MPI runtime such as OpenMPI

Install the optional dependency via

```bash
pip install mpi4py
```

## Usage

Both scripts expect the same command line structure.  Parameters are
given as pairs followed by the list of input strings.  Example:

```bash
python closestStringEvoFinalEn.py -g 100 -m 25 -b 5 -n 20 AAAGT ATGAA AAGGT
```

For the MPI variant replace the interpreter call with ``mpiexec``:

```bash
mpiexec -n 4 python closestStringEvoMPI.py -g 100 -m 25 -b 5 -n 20 AAAGT ATGAA AAGGT
```

### Parameters

* ``-g`` – number of generations the evolutionary search runs
* ``-m`` – mutation rate applied to each candidate
* ``-b`` – how many generations a solution must remain best before the
  algorithm stops ("best counter")
* ``-n`` – number of candidate strings per generation
* ``<strings>`` – input strings for which a closest string is sought

The program prints the best solution found and the corresponding
``k`` value, i.e. the maximum Hamming distance to any of the supplied
strings.

## Project Structure

* ``closestStringEvoFinalEn.py`` – sequential implementation capable of
  handling variable string lengths
* ``closestStringEvoMPI.py`` – MPI based parallel solver
* Additional directories contain experimental or brute-force versions
  of the algorithm

## License

This project is provided without a specific licence and is intended for
educational purposes.

