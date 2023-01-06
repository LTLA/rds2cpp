# This tests the correct saving and loading of symbols.
# library(testthat); library(rds2cpp); source("test-expression.R")

test_that("parsing expression vectors works as expected", {
    y <- expression(1, aaron, lun(aaron, tin, long))
    tmp <- tempfile(fileext=".rds")
    saveRDS(y, file=tmp)

    info <- rds2cpp::parse(tmp)
    expect_true(attr(info$value, "pretend-to-be-an-expression"))
    expect_identical(info$value[[1]], 1) # constant is directly read in as itself.
    expect_identical(info$symbols[info$value[[2]]$symbol_id + 1], "aaron")
    expect_identical(info$value[[3]][[1]], "lun")
    expect_identical(info$symbols[info$value[[3]][[2]][[1]]$symbol_id + 1L], "aaron")
    expect_identical(info$symbols[info$value[[3]][[2]][[2]]$symbol_id + 1L], "tin")
    expect_identical(info$symbols[info$value[[3]][[2]][[3]]$symbol_id + 1L], "long")
})

test_that("writing expression vectors works as expected", {
    y <- list(
        1, 
        "aaron",
        list(
            "lun",
            list("aaron", "tin", "long")
        )
    )
    attr(y, "pretend-to-be-an-expression") <- TRUE
    attr(y[[2]], "pretend-to-be-a-symbol") <- TRUE
    attr(y[[3]], "pretend-to-be-a-language") <- TRUE
    attr(y[[3]][[1]], "pretend-to-be-a-symbol") <- TRUE
    attr(y[[3]][[2]][[1]], "pretend-to-be-a-symbol") <- TRUE
    attr(y[[3]][[2]][[2]], "pretend-to-be-a-symbol") <- TRUE
    attr(y[[3]][[2]][[3]], "pretend-to-be-a-symbol") <- TRUE

    tmp <- tempfile(fileext=".rds")
    rds2cpp::write(y, tmp)

    roundtrip <- readRDS(tmp)
    expect_identical(roundtrip, expression(1, aaron, lun(aaron, tin, long)))
})

test_that("parsing expression vectors works with attributes", {
    y <- c(expression(aaron + 1))
    attr(y, "foo") <- "BAR"
    tmp <- tempfile(fileext=".rds")
    saveRDS(y, file=tmp)

    info <- rds2cpp::parse(tmp)
    expect_true(attr(info$value, "pretend-to-be-an-expression"))
    expect_identical(attr(info$value, "foo"), "BAR")
})

test_that("writing expression vectors works with attributes", {
    y <- list(list("+", list("aaron", 1)))
    attr(y, "pretend-to-be-an-expression") <- TRUE
    attr(y[[1]], "pretend-to-be-a-language") <- TRUE
    attr(y[[1]][[1]], "pretend-to-be-a-symbol") <- TRUE
    attr(y[[1]][[2]][[1]], "pretend-to-be-a-symbol") <- TRUE
    attr(y, "foo") <- "BAR"

    tmp <- tempfile(fileext=".rds")
    rds2cpp::write(y, tmp)

    roundtrip <- readRDS(tmp)
    expect_identical(attr(roundtrip, "foo"), "BAR")
    attr(roundtrip, "foo") <- NULL
    expect_identical(roundtrip, expression(aaron + 1))
})

