# This tests the correct saving and loading of atomic vectors.
# library(testthat); library(rds2cpp); source("test-pairlist.R")

test_that("pairlist loading works as expected", {
    tmp <- tempfile(fileext=".rds")

    y <- pairlist(runif(10), runif(20), runif(30))
    saveRDS(y, file=tmp)
    roundtrip <- rds2cpp:::parse(tmp)
    expect_identical(roundtrip$data, as.list(y))
    expect_true(all(is.na(roundtrip$tag)))

    # Now with some nesting. 
    y <- pairlist(sample(letters), pairlist(sample(11), runif(12)))
    saveRDS(y, file=tmp)
    roundtrip <- rds2cpp:::parse(tmp)
    expect_identical(roundtrip$data[[1]], y[[1]])
    expect_identical(roundtrip$data[[2]]$data, as.list(y[[2]]))
    expect_true(all(is.na(roundtrip$tag)))

    # Now with some names.
    y <- pairlist(foo=sample(letters), bar=pairlist(whee=sample(11), bum=runif(12))) # with names
    saveRDS(y, file=tmp)
    roundtrip <- rds2cpp:::parse(tmp)
    expect_identical(roundtrip$data[[1]], y[[1]])
    expect_identical(roundtrip$tag, names(y))
    expect_identical(setNames(roundtrip$data[[2]]$data, roundtrip$data[[2]]$tag), as.list(y[[2]]))
})

test_that("pairlist loading works with attributes", {
    y <- pairlist(aaron=sample(letters), bar=list(sample(11), runif(12)))
    attr(y, "foo") <- "bar"

    tmp <- tempfile(fileext=".rds")
    saveRDS(y, file=tmp)

    roundtrip <- rds2cpp:::parse(tmp)
    massaged <- setNames(roundtrip$data, roundtrip$tag)
    attr(massaged, 'foo') <- attr(roundtrip, 'foo')

    expect_identical(massaged, as.list(y))
})

