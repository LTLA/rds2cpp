# This tests the correct saving and loading of atomic vectors.
# library(testthat); library(rds2cpp); source("test-list.R")

test_that("list loading works as expected", {
    tmp <- tempfile(fileext=".rds")
    scenarios <- list(
        list(runif(10), runif(20), runif(30)),
        list(sample(letters), list(sample(11), runif(12))), # nested
        list(list(2, 6), list(5, c("cat", "dog", "bouse"), list(sample(99), runif(20)))) # deeply nested
    )

    for (y in scenarios) {
        saveRDS(y, file=tmp)
        roundtrip <- rds2cpp:::parse(tmp)
        expect_identical(roundtrip, y)
    }
})
