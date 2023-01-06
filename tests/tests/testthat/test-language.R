# Test that language serialization works correctly.
# library(testthat); library(rds2cpp); source("test-language.R")

test_that("function serialization works correctly", {
    tmp <- tempfile(fileext=".rds")
    y <- quote(cbind(12, 'foo'))
    saveRDS(y, file=tmp)

    info <- rds2cpp::parse(tmp)
    expect_true(attr(info$value, "pretend-to-be-a-language"))
    expect_identical(info$value[[1]], "cbind")

    expect_identical(attr(info$value[[2]], "arg-names"), character(2))
    args <- info$value[[2]]
    attr(args, "arg-names") <- NULL
    expect_identical(args, list(12, "foo"))
})

test_that("function writing works correctly", {
    y <- list("cbind", list(12, "foo"))
    attr(y[[1]], "pretend-to-be-a-symbol") <- TRUE
    attr(y, "pretend-to-be-a-language") <- TRUE

    tmp <- tempfile(fileext=".rds")
    rds2cpp::write(y, tmp)
    roundtrip <- readRDS(tmp)
    expect_identical(roundtrip, quote(cbind(12, 'foo')))
})

test_that("function serialization works correctly for named arguments", {
    tmp <- tempfile(fileext=".rds")
    y <- quote(aaron(foo=12, bar='foo'))
    saveRDS(y, file=tmp)

    info <- rds2cpp::parse(tmp)
    expect_true(attr(info$value, "pretend-to-be-a-language"))
    expect_identical(info$value[[1]], "aaron")

    expect_identical(attr(info$value[[2]], "arg-names"), c("foo", "bar"))
    args <- info$value[[2]]
    attr(args, "arg-names") <- NULL
    expect_identical(args, list(12, "foo"))
})

test_that("function writing works correctly with named arguments", {
    y <- list("aaron", list(12, "foo"))
    attr(y[[1]], "pretend-to-be-a-symbol") <- TRUE
    attr(y, "pretend-to-be-a-language") <- TRUE
    names(y[[2]]) <- c("foo", "bar")

    tmp <- tempfile(fileext=".rds")
    rds2cpp::write(y, tmp)
    roundtrip <- readRDS(tmp)
    expect_identical(roundtrip, quote(aaron(foo=12, bar='foo')))
})

test_that("function serialization works correctly for nested function calls", {
    tmp <- tempfile(fileext=".rds")
    y <- quote(x + y + z)
    saveRDS(y, file=tmp)

    info <- rds2cpp::parse(tmp)
    expect_true(attr(info$value, "pretend-to-be-a-language"))
    expect_identical(info$value[[1]], "+")
    expect_identical(info$value[[2]][[1]][[1]], "+")
    expect_identical(info$symbols[info$value[[2]][[1]][[2]][[1]]$symbol_id + 1], "x")
    expect_identical(info$symbols[info$value[[2]][[1]][[2]][[2]]$symbol_id + 1], "y")
    expect_identical(info$symbols[info$value[[2]][[2]]$symbol_id + 1], "z")
})

test_that("function writing works correctly for nested function calls", {
    y <- list("+", list(list("+", list("x", "y")), "z"))
    attr(y[[1]], "pretend-to-be-a-symbol") <- TRUE
    attr(y[[2]][[1]][[1]], "pretend-to-be-a-symbol") <- TRUE
    attr(y[[2]][[1]][[2]][[1]], "pretend-to-be-a-symbol") <- TRUE
    attr(y[[2]][[1]][[2]][[2]], "pretend-to-be-a-symbol") <- TRUE
    attr(y[[2]][[2]], "pretend-to-be-a-symbol") <- TRUE
    attr(y, "pretend-to-be-a-language") <- TRUE
    attr(y[[2]][[1]], "pretend-to-be-a-language") <- TRUE

    tmp <- tempfile(fileext=".rds")
    rds2cpp::write(y, tmp)
    roundtrip <- readRDS(tmp)
    expect_identical(roundtrip, quote(x + y + z))
})

test_that("function serialization works correctly with attributes", {
    tmp <- tempfile(fileext=".rds")
    y <- quote(x %% 1)
    attr(y, "foo") <- "BAR"
    saveRDS(y, file=tmp)

    info <- rds2cpp::parse(tmp)
    expect_true(attr(info$value, "pretend-to-be-a-language"))
    expect_identical(attr(info$value, "foo"), "BAR")

    expect_identical(info$value[[1]], "%%")
    expect_identical(info$symbols[info$value[[2]][[1]]$symbol_id + 1], "x")
    expect_identical(info$value[[2]][[2]], 1)
})

test_that("function writing works correctly with attributes", {
    y <- list("%%", list("x", 1))
    attr(y, "foo") <- "BAR"
    attr(y[[2]][[1]], "pretend-to-be-a-symbol") <- TRUE
    attr(y, "pretend-to-be-a-language") <- TRUE

    tmp <- tempfile(fileext=".rds")
    rds2cpp::write(y, tmp)
    roundtrip <- readRDS(tmp)

    expect_identical(attr(roundtrip, "foo"), "BAR")
    attr(roundtrip, "foo") <- NULL
    expect_identical(roundtrip, quote(x%%1))
})

