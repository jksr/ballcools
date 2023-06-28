# BAllCools: Binary AllC File Tools v0.0.2

[![Anaconda-Server Badge](https://anaconda.org/jksr/ballcools/badges/version.svg)](https://anaconda.org/jksr/ballcools)
[![Anaconda-Server Badge](https://anaconda.org/jksr/ballcools/badges/latest_release_date.svg)](https://anaconda.org/jksr/ballcools)

BAllCools is a comprehensive tool designed to handle binary AllC files (BAllC). The tool aims to solve the challenges posed by the large number of single-cell data. The conventional AllC files store methylation data as text, which becomes significantly bulky when dealing with single-cell data. BAllCools provides an efficient way to store and retrieve data by converting these text files into binary files, thereby saving substantial storage space.


<details>
  <summary>Background</summary>
  
### AllC format
AllC format is a tab-separated-value (tsv) text format containing base level methylation and coverage counts, originally defined in [methylpy](https://github.com/yupenghe/methylpy), a python package developed in the [Ecker lab](https://ecker.salk.edu/) for bulk WGBS-seq data analysis.

|index|column name|example|note|
|:----:|:-----:|:-----:|:---:|
|1|chromosome|12|with no "chr"|
|2|position|18283342|1-based|
|3|strand|+|either + or -|
|4|sequence context|CGT|can be more than 3 bases|
|5|mc|18|count of reads supporting methylation|
|6|cov|21|read coverage|
|7|methylated|1|indicator of significant methylation (1 if no test is performed)|

### BAllC format

**TODO...**

</details>
<br/>



## Installation

The most convient way to install BAllCools is using conda
```bash
conda install -c jksr ballcools
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

```bash
ballcools -h
```

This will print a help message with a summary of the subcommands and their functionalities.

### Basic workflow
**TODO...**












    
# TODO
- [x] examine index format and reduce its size
- [x] organize util main files into CLI
- [x] change internal query return to iterator
- [x] cleanup internal mcrecords and external mcrecords
- [x] merge
- [-] add important check; like order violation etc
- [x] prepare conda package
- [x] to allc
- [x] write test scipt
- [x] extensive test
- [ ] port to python
- [ ] add docs
- [ ] merge +/- CGN
- ~~[ ] sort; abandon, useless in most usecase~~
