# This tests the correct saving and loading of atomic vectors.
# library(testthat); library(rds2cpp); source("test-atomic.R")

test_that("integer vector loading works as expected", {
    tmp <- tempfile(fileext=".rds")
    for (scenario in 1:4) {
        if (scenario == 1) {
            y <- sample(15)
        } else if (scenario == 2) {
            y <- rpois(112, lambda=8)
        } else if (scenario == 3) {
            y <- rnorm(990) * 10
        } else if (scenario == 4) {
            y <- 0:99
            y[sample(length(y), 10)] <- NA
        }
        y <- as.integer(y)

        saveRDS(y, file=tmp)
        roundtrip <- rds2cpp:::parse(tmp)
        expect_identical(roundtrip, y)
    }
})

test_that("double vector loading works as expected", {
    tmp <- tempfile(fileext=".rds")
    for (scenario in 1:3) {
        if (scenario == 1) {
            y <- rnorm(99)
        } else if (scenario == 2) {
            y <- rgamma(1, 2, 1)
        } else if (scenario == 3) {
            y <- rexp(1000)
        } else if (scenario == 1) {
            y <- rnorm(999)
            y[sample(length(y), 10)] <- NA
            y[sample(length(y), 10)] <- NaN
            y[sample(length(y), 10)] <- Inf
            y[sample(length(y), 10)] <- -Inf
        }

        saveRDS(y, file=tmp)
        roundtrip <- rds2cpp:::parse(tmp)
        expect_identical(roundtrip, y)
    }
})

