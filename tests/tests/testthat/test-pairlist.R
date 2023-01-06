# This tests the correct saving and loading of atomic vectors.
# library(testthat); library(rds2cpp); source("test-pairlist.R")

test_that("pairlist loading works as expected", {
    tmp <- tempfile(fileext=".rds")

    y <- pairlist(runif(10), runif(20), runif(30))
    saveRDS(y, file=tmp)
    roundtrip <- rds2cpp:::parse(tmp)
    expect_true(attr(roundtrip$value, "pretend-to-be-a-pairlist"))
    expect_identical(roundtrip$value$data, as.list(y))
    expect_true(all(is.na(roundtrip$value$tag)))

    # Now with some nesting. 
    y <- pairlist(sample(letters), pairlist(sample(11), runif(12)))
    saveRDS(y, file=tmp)
    roundtrip <- rds2cpp:::parse(tmp)
    expect_true(attr(roundtrip$value, "pretend-to-be-a-pairlist"))
    expect_identical(roundtrip$value$data[[1]], y[[1]])
    expect_identical(roundtrip$value$data[[2]]$data, as.list(y[[2]]))
    expect_true(all(is.na(roundtrip$value$tag)))

    # Now with some names.
    y <- pairlist(foo=sample(letters), bar=pairlist(whee=sample(11), bum=runif(12))) # with names
    saveRDS(y, file=tmp)
    roundtrip <- rds2cpp:::parse(tmp)
    expect_true(attr(roundtrip$value, "pretend-to-be-a-pairlist"))
    expect_identical(roundtrip$value$data[[1]], y[[1]])
    expect_identical(roundtrip$value$tag, names(y))
    expect_identical(setNames(roundtrip$value$data[[2]]$data, roundtrip$value$data[[2]]$tag), as.list(y[[2]]))
})

test_that("pairlist loading works with attributes", {
    y <- pairlist(aaron=sample(letters), bar=list(sample(11), runif(12)))
    attr(y, "foo") <- "bar"

    tmp <- tempfile(fileext=".rds")
    saveRDS(y, file=tmp)

    roundtrip <- rds2cpp:::parse(tmp)
    expect_true(attr(roundtrip$value, "pretend-to-be-a-pairlist"))

    massaged <- setNames(roundtrip$value$data, roundtrip$value$tag)
    attr(massaged, 'foo') <- attr(roundtrip$value, 'foo')

    expect_identical(massaged, as.list(y))
})

test_that("pairlist writing works as expected", {
    tmp <- tempfile(fileext=".rds")

    y <- list(runif(10), runif(20), runif(30))
    attr(y, "pretend-to-be-a-pairlist") <- TRUE
    rds2cpp:::write(y, tmp)
    roundtrip <- readRDS(tmp)
    expect_true(is.pairlist(roundtrip))
    expect_identical(y, as.list(roundtrip))

    # Now with some nesting. 
    y <- list(sample(letters), list(sample(11), runif(12)))
    attr(y, "pretend-to-be-a-pairlist") <- TRUE
    attr(y[[2]], "pretend-to-be-a-pairlist") <- TRUE
    rds2cpp:::write(y, tmp)
    roundtrip <- readRDS(tmp)
    expect_true(is.pairlist(roundtrip))
    expect_identical(y[[1]], roundtrip[[1]])
    expect_true(is.pairlist(roundtrip[[2]]))
    expect_identical(y[[2]], as.list(roundtrip[[2]]))

    # Now with some names.
    y <- list(foo=sample(letters), bar=list(whee=sample(11), bum=runif(12))) # with names
    attr(y, "pretend-to-be-a-pairlist") <- TRUE
    attr(y[[2]], "pretend-to-be-a-pairlist") <- TRUE
    rds2cpp:::write(y, tmp)
    roundtrip <- readRDS(tmp)
    expect_true(is.pairlist(roundtrip))
    expect_identical(y[[1]], roundtrip[[1]])
    expect_true(is.pairlist(roundtrip[[2]]))
    expect_identical(y[[2]], as.list(roundtrip[[2]]))
})
