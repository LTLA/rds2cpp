# This tests the correct saving and loading of atomic vectors.
# library(testthat); library(rds2cpp); source("test-atomic.R")

test_that("integer vector loading works as expected", {
    tmp <- tempfile(fileext=".rds")
    scenarios <- list(
        sample(15),
        rpois(112, lambda=8),
        rnorm(990) * 10,
        {
            y <- 0:99
            y[sample(length(y), 10)] <- NA
            y
        }
    )

    for (y in scenarios) {
        y <- as.integer(y)
        saveRDS(y, file=tmp)
        roundtrip <- rds2cpp:::parse(tmp)
        expect_identical(roundtrip, y)
    }
})

test_that("logical vector loading works as expected", {
    tmp <- tempfile(fileext=".rds")
    scenarios <- list(
        rbinom(55, 1, 0.5) == 0,
        {
            y <- rbinom(999, 1, 0.5) == 0
            y[sample(length(y), 10)] <- NA
            y
        }
    )

    for (y in scenarios) {
        saveRDS(y, file=tmp)
        roundtrip <- rds2cpp:::parse(tmp)
        expect_identical(roundtrip, y)
    }
})

test_that("double vector loading works as expected", {
    tmp <- tempfile(fileext=".rds")
    scenarios <- list(
        rnorm(99),
        rgamma(1, 2, 1),
        rexp(1000),
        {
            y <- rnorm(999)
            y[sample(length(y), 10)] <- NA
            y[sample(length(y), 10)] <- NaN
            y[sample(length(y), 10)] <- Inf
            y[sample(length(y), 10)] <- -Inf
            y
        }
    )

    for (y in scenarios) {
        saveRDS(y, file=tmp)
        roundtrip <- rds2cpp:::parse(tmp)
        expect_identical(roundtrip, y)
    }
})

test_that("raw vector loading works as expected", {
    tmp <- tempfile(fileext=".rds")
    y <- as.raw(sample(256, 99, replace=TRUE) - 1)
    saveRDS(y, file=tmp)
    roundtrip <- rds2cpp:::parse(tmp)
    expect_identical(roundtrip, y)
})

test_that("complex vector loading works as expected", {
    tmp <- tempfile(fileext=".rds")
    scenarios <- list(
        rnorm(99) + rnorm(99) * 1i,
        rgamma(1, 2, 1) * 2i,
        rexp(1000) + 0i,
        {
            y <- rnorm(999) + rnorm(999) * 1i
            y[sample(length(y), 10)] <- NA
            y[sample(length(y), 10)] <- NaN
            y[sample(length(y), 10)] <- Inf
            y[sample(length(y), 10)] <- -Inf
            y
        }
    )

    for (y in scenarios) {
        saveRDS(y, file=tmp)
        roundtrip <- rds2cpp:::parse(tmp)
        expect_identical(roundtrip, y)
    }
})
