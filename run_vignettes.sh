#!/bin/bash

#CMD='bin/Rscript rdt-plugins/promises/R/benchmark.R'
#CMD='bin/Rscript compose_testable_vignettes.R'
#CMD='bin/R --slave --no-restore --debugger=gdb --file=compose_testable_vignettes.R --args'
CMD='bin/R --slave --no-restore --file=compose_testable_vignettes.R --args'

export R_COMPILE_PKGS=0
export R_DISABLE_BYTECODE=1
export R_ENABLE_JIT=0
export R_KEEP_PKG_SOURCE=yes

export RDT_COMPILE_VIGNETTE=false

OUTPUT_DIR="/data/kondziu/uncompiled/`date '+%F'`/"
ARGS="--tmp-dir=$OUTPUT_DIR --output-dir=$OUTPUT_DIR"

if $RDT_COMPILE_VIGNETTE
then 
    ARGS="$ARGS --compile"   
fi    

mkdir -p "$OUTPUT_DIR"

PACKAGES=

if [ $# -ge 1 ]
then
    PACKAGES="$@"
else
    #PACKAGES="vcd rpart survival mclust party mvtnorm igraph"
    #ALL_PACKAGES="grid ggplot2 haven readr readxl stringr tibble tidyverse digest colorspace kernlab vcd rpart survival mclust party mvtnorm igraph dplyr"
    PACKAGES=`cat r-package-anthology.csv | grep -v '^#' | grep -v '^$' | tr -s ' ' | tail -n +2 | cut -f 2 -d';' | xargs echo`
fi

echo > packages_done

for i in $PACKAGES
do 
    echo "$CMD $i"
    time $CMD $ARGS $i 2>&1 | tee "$i.log" 
    echo "$i" >> packages_done
done   

