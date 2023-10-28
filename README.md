# BAllCools: Binary AllC File Tools v0.0.4

[![DOI](https://zenodo.org/badge/653446390.svg)](https://zenodo.org/badge/latestdoi/653446390)
[![Anaconda-Server Badge](https://anaconda.org/jksr/ballcools/badges/version.svg)](https://anaconda.org/jksr/ballcools)
[![Anaconda-Server Badge](https://anaconda.org/jksr/ballcools/badges/latest_release_date.svg)](https://anaconda.org/jksr/ballcools)
[![Anaconda-Server Badge](https://anaconda.org/jksr/ballcools/badges/platforms.svg)](https://anaconda.org/jksr/ballcools)


BAllCools is a comprehensive tool designed to handle binary AllC files (BAllC). The tool aims to solve the challenges posed by the large number of single-cell data. The conventional AllC files store methylation data as text, which becomes significantly bulky when dealing with single-cell data. BAllCools provides an efficient way to store and retrieve data by converting these text files into binary files, thereby saving substantial storage space. **The BAllC format save >55% storage compared to AllC the format, and BAllCools accelorates BAllC operation like data merging (`ballcools merge`) ~30x.**

## Background and BAllC format
For background of (B)AllC format and the specification of BAllC format, please check [doc/ballc_spec.pdf](https://github.com/jksr/ballcools/blob/main/doc/ballc_spec.pdf) for details.

## Installation and compilation
The most convient way to install BAllCools is using conda
```bash
conda install -c jksr ballcools
```

Compiling from source code requires a c++ compiler (eg. ```g++```) and ```make```
```bash
git clone https://github.com/jksr/ballcools
cd ballcools & make
```
And then the executable ```ballcools``` will be created in the folder ```bin```.

### Dependency
BAllCools 
```yaml
htslib >=1.10, <2.0
libdeflate >=1.6, <2.0
```

## Features

- **view**: allows you to view the data stored in a BAllC file.
- **index**: indexes a BAllC file to expedite the retrieval of data.
- **a2b**: converts an AllC file (text format) into a BAllC file (binary format), allowing for more efficient storage and faster access.
- **b2a**: converts a BAllC file (binary format) into an AllC file (text format).
- **meta**: extracts and indexes Cytosines from a genome sequence file (fasta format) and stores them in a CMeta file (bed format).
- **query**: allows to retrieve data from a BAllC file according to genome range and context of cytosine.
- **merge**: merges multiple BAllC files into a single file. This is ~30x faster than merge AllC files directly

## Usage

After installation, BAllCools can be run with the following command:

```bash
ballcools [OPTIONS] [SUBCOMMAND]
```

For help with the tool, use the `-h` or `--help` option:

```shell
ballcools -h
```
```text
BAllCools: Binary AllC file tools v(0.0.1)
Usage: ballcools [OPTIONS] [SUBCOMMAND]

Options:
  -h,--help                   Print this help message and exit

Subcommands:
  view                        View data stored in a BAllC file.
  index                       Index a BAllC file.
  a2b                         Convert an AllC file to a BAllc file.
  b2a                         Convert an BAllC file to a Allc file.
  meta                        Extract and index C from a genome sequence file (fasta) and store in a CMeta file (bed format).
  query                       Query info from a BAllC file
  check                       check a BAllC file
  merge                       Merge BAllC files
```

This will print a help message with a summary of the subcommands and their functionalities.

## Basic workflow

### Create the CMeta file
A CMeta file stores the context and the strandness of each cytosine of a given genome.
Although the CMeta file is not required for BAllC files and BAllCools, 
it is highly recommended to have one to accompany the associated BAllC files.
For details of the CMeta file, please check [doc/ballc_spec.pdf](https://github.com/jksr/ballcools/blob/main/doc/ballc_spec.pdf).

For a given genome (eg. a standard one like mm10 or hg38 or an individual-specific genome), ```ballcools meta``` can be used to generate the corresponding CMeta file. This step usually takes around 30min. This only need to be run once for each genome, and the resulting CMeta file can be used for all BAllC files associated with this genome.


### Create BAllC files









### python version
BAllCools has python port as well. See [pyballc](https://github.com/jksr/pyballc) for details.
```
pip install pyballc
```







    
# TODO
- [x] examine index format and reduce its size
- [x] organize util main files into CLI
- [x] change internal query return to iterator
- [x] cleanup internal mcrecords and external mcrecords
- [x] merge
- [ ] add important check; like order violation etc
- [x] prepare conda package
- [x] to allc
- [x] write test scipt
- [x] extensive test
- [x] port to python
- [ ] add docs
- ~~[ ] merge +/- CGN, useless in most usecase, can be done by allcools with allc format~~
- ~~[ ] sort; abandon, useless in most usecase~~

