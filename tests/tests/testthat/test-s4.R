# This tests the correct saving and loading of S4 objects.
# library(testthat); library(rds2cpp); source("test-s4.R")

setClass("FOO", slots=c(bar="integer"))

s4.scenarios <- list(
    Matrix::rsparsematrix(100, 10, 0.05),
    new("FOO", bar=2L) # custom class
)

test_that("S4 object loading works as expected", {
    tmp <- tempfile(fileext=".rds")
    for (y in s4.scenarios) {
        saveRDS(y, file=tmp)
        roundtrip <- rds2cpp:::parse(tmp)
        expect_identical(roundtrip$value, y)
    }
})

test_that("S4 object writing works as expected", {
    tmp <- tempfile(fileext=".rds")
    for (y in s4.scenarios) {
        rds2cpp:::write(y, tmp)
        roundtrip <- readRDS(tmp)
        expect_identical(roundtrip, y)
    }
})
