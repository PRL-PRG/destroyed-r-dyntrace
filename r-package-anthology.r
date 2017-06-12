# Other things that need to be installed:
# - GSL (GNU Scientific Library)
#   sudo apt install gsl-bin libgsl-dbg libgsl-dev
# - XML2
#   sudo apt install xml2 libxml2-dev

library(dplyr)

packages <- read.csv("r-package-anthology.csv", 
                        header=TRUE, sep=";", 
                        comment.char="#", strip.white=TRUE)

# Install CRAN packages
install.packages((packages %>% filter(source == "cran"))$package)

# Install additional packages needed by vignettes, but not listed as pkg
# dependencies.
install.packages(c(
#    "knitr",
#    "nycflights",
    "chron",
    "tseries",
))

source("https://bioconductor.org/biocLite.R")
biocLite()
for (package in (packages %>% filter(source == "bioc"))$package) {
    biocLite(package)
}




