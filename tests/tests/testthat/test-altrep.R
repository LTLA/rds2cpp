# This tests the correct saving and loading of atomic vectors.
# library(testthat); library(rds2cpp); source("test-altrep.R")

test_that("compact integer loading works as expected", {
    tmp <- tempfile(fileext=".rds")
    scenarios <- list(
        1:15,
        1020:8489, # start from somewhere else
        99:5, # reverse
        100:-5
    )

    for (y in scenarios) {
        # Double-checking we got a compact intseq.
        expect_output(.Internal(inspect(y)), "compact")

        saveRDS(y, file=tmp)
        roundtrip <- rds2cpp:::parse(tmp)
        expect_identical(roundtrip, y)
    }
})

test_that("attribute wrapping for integers works as expected", {
    x <- 1:100
    names(x) <- sprintf("GENE_%s", seq_along(x))
    expect_output(.Internal(inspect(x)), "wrapper")
    expect_output(.Internal(inspect(x)), "compact")

    tmp <- tempfile(fileext=".rds")
    saveRDS(x, file=tmp)
    roundtrip <- rds2cpp:::parse(tmp)
    expect_identical(roundtrip, x)

    # Create a fresh ALTREP because the last one was realized.
    x <- 200:100
    names(x) <- sprintf("GENE_%s", seq_along(x))
    attr(x, "FOO") <- "BAR"
    expect_output(.Internal(inspect(x)), "wrapper")
    expect_output(.Internal(inspect(x)), "compact")

    saveRDS(x, file=tmp)
    roundtrip <- rds2cpp:::parse(tmp)
    expect_identical(roundtrip, x)
})

