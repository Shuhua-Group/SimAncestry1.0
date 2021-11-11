## SimAncestry1.0

SimAncestry is a script to extract archaic introgressed sequences from ms simulation output. The script SimAncestry is only a simple script to analyze the tree structure outputted by the software ms. It cannot be applied to the real data as it was not designed to work with SNP data. The algorithm used in SimAncestry is as follows. For each non-African node, we make a transversal up to the root. If this node coalesces to archaic lineage before any African lineages, we say this node is introgressed. This script is used only to obtain the ground truth for introgressed sequences.

The usage of SimAncestry can be found in Manual for SimAncestry.pdf.

If you use our software in your research, please cite the following paper:

Yuan K, Ni X, Liu C, Pan Y, Deng L, Zhang R, Gao Y, Ge X, Liu J, Ma X, Lou H, Wu T, Xu S. Refining models of archaic admixture in Eurasia with ArchaicSeeker 2.0. Nat Commun. 2021 Oct 29;12(1):6232. doi: 10.1038/s41467-021-26503-5.

https://www.nature.com/articles/s41467-021-26503-5.
