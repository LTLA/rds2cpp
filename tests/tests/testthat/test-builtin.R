# This tests the correct saving and loading of atomic vectors.
# library(testthat); library(rds2cpp); source("test-builtin.R")

builtin.scenarios <- list(min, max, c, is.matrix)

test_that("built-in function loading works as expected", {
    tmp <- tempfile(fileext=".rds")
    for (y in builtin.scenarios) {
        saveRDS(y, file=tmp)
        roundtrip <- rds2cpp:::parse(tmp)
        expect_true(attr(roundtrip$value, "pretend-to-be-a-builtin"))
        expect_identical(get(roundtrip$value[[1]], envir=baseenv()), y)
    }
})

test_that("built-in function writing works as expected", {
    y <- list("min")
    attr(y, "pretend-to-be-a-builtin") <- TRUE

    tmp <- tempfile(fileext=".rds")
    rds2cpp::write(y, tmp)
    roundtrip <- readRDS(tmp)
    expect_identical(min, roundtrip)

    y[[1]] <- "is.matrix"
    rds2cpp::write(y, tmp)
    roundtrip <- readRDS(tmp)
    expect_identical(is.matrix, roundtrip)
})
