# Test that environment serialization works correctly.
# library(testthat); library(rds2cpp); source("test-environment.R")

test_that("empty environment loading works as expected", {
    y <- new.env()

    tmp <- tempfile(fileext=".rds")
    saveRDS(y, file=tmp)
    roundtrip <- rds2cpp:::parse(tmp)
    expect_true(length(roundtrip$variables) == 0)
    expect_identical(roundtrip$parent, -1L)

    saveRDS(.GlobalEnv, file=tmp)
    roundtrip <- rds2cpp:::parse(tmp)
    expect_identical(roundtrip$id, -1L)
})

test_that("non-empty environment loading works as expected", {
    y <- new.env()
    y$AAA <- runif(10)
    y$BBB <- rnorm(10)

    tmp <- tempfile(fileext=".rds")
    saveRDS(y, file=tmp)
    roundtrip <- rds2cpp:::parse(tmp)
    expect_true(length(roundtrip$variables) == 2L)

    expect_identical(roundtrip$id, 0L)
    expect_identical(roundtrip$parent, -1L)
    expect_identical(roundtrip$variables$AAA, y$AAA)
    expect_identical(roundtrip$variables$BBB, y$BBB)

    # Something a little more involved.
#    y <- new.env()
#    y$Aaron <- 2:60
#    y$Jayaram <- as.character(5:10)
#    y$Michael <- list(1:5)
#
#    saveRDS(y, file=tmp)
#    roundtrip <- rds2cpp:::parse(tmp)
})

test_that("environment references work as expected", {
    y <- new.env()

    tmp <- tempfile(fileext=".rds")
    saveRDS(list(y, y), file=tmp)
    roundtrip <- rds2cpp:::parse(tmp)

    expect_identical(roundtrip[[1]]$id, 0L)
    expect_identical(roundtrip[[2]]$id, 0L)

    # Links to existing references work correctly.
    z <- new.env()
    saveRDS(list(z, y, z, y), file=tmp)
    roundtrip <- rds2cpp:::parse(tmp)

    expect_identical(roundtrip[[1]]$id, 0L)
    expect_identical(roundtrip[[2]]$id, 1L)
    expect_identical(roundtrip[[3]]$id, 0L)
    expect_identical(roundtrip[[4]]$id, 1L)

    # Mix in some global references.
    saveRDS(list(.GlobalEnv, z, y, z, y, .GlobalEnv), file=tmp)
    roundtrip <- rds2cpp:::parse(tmp)

    expect_identical(roundtrip[[1]]$id, -1L)
    expect_identical(roundtrip[[2]]$id, 0L)
    expect_identical(roundtrip[[3]]$id, 1L)
    expect_identical(roundtrip[[4]]$id, 0L)
    expect_identical(roundtrip[[5]]$id, 1L)
    expect_identical(roundtrip[[5]]$id, -1L)
})

