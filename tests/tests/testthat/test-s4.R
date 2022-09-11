# This tests the correct saving and loading of S4 objects.
# library(testthat); library(rds2cpp); source("test-s4.R")

test_that("S4 object loading works as expected", {
    tmp <- tempfile(fileext=".rds")
    scenarios <- list(
        Matrix::rsparsematrix(100, 10, 0.05)
        # TODO: add more scenarios.
    )

    for (y in scenarios) {
        saveRDS(y, file=tmp)
        roundtrip <- rds2cpp:::parse(tmp)
        expect_identical(roundtrip, y)
    }
})

test_that("S4 object loading works with custom classes", {
    setClass("FOO", slots=c(bar="integer"))
    A <- new("FOO", bar=2L)

    tmp <- tempfile(fileext=".rds")
    saveRDS(A, file=tmp)
    roundtrip <- rds2cpp:::parse(tmp)
    expect_identical(roundtrip, A)
})
