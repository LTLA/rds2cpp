# This tests the correct saving and loading of symbols.
# library(testthat); library(rds2cpp); source("test-symbol.R")

scan_for_references <- function(file) {
    handle <- gzfile(file, open="rb")
    on.exit(close(handle))
    x <- readBin(handle, raw(), 1000)

    candidates <- integer(0)
    for (i in seq_len(length(x) - 3)) {
        if (x[i] == 0 && x[i + 1] == 0 && x[i + 3] == 255) {
            candidates <- c(candidates, x[i + 2])
        }
    }

    candidates
}

########################################################

y <- list("foo", list("bar", "foo", list("bar")))
attr(y[[1]], "pretend-to-be-a-symbol") <- TRUE
attr(y[[2]][[1]], "pretend-to-be-a-symbol") <- TRUE
attr(y[[2]][[2]], "pretend-to-be-a-symbol") <- TRUE
attr(y[[2]][[3]][[1]], "pretend-to-be-a-symbol") <- TRUE

tmp <- tempfile(fileext=".rds")
rds2cpp::write(y, tmp)

test_that("redundant symbols are correctly resolved", {
    roundtrip <- readRDS(tmp)

    expect_type(roundtrip[[1]], "symbol")
    expect_type(roundtrip[[2]][[1]], "symbol")
    expect_type(roundtrip[[2]][[2]], "symbol")
    expect_type(roundtrip[[2]][[3]][[1]], "symbol")

    expect_identical(as.character(roundtrip[[1]]), "foo")
    expect_identical(as.character(roundtrip[[2]][[1]]), "bar")
    expect_identical(as.character(roundtrip[[2]][[2]]), "foo")
    expect_identical(as.character(roundtrip[[2]][[3]][[1]]), "bar")

    refs <- scan_for_references(tmp)
    expect_true(length(unique(refs)) >= 2) # at least 2 refs of the 'foo' and 'bar' symbols.
})

test_that("parser works correctly with redundant symbols", {
    mine <- rds2cpp::parse_details(tmp)
    expect_identical(mine$symbols, c("foo", "bar"))
    expect_identical(mine$value, list(list(id=0L), list(list(id=1L), list(id=0L), list(list(id=1L)))))
})

########################################################

test_that("redundant symbols in the attributes are respected", {
    y <- list(A=list(B=2, C=list(D=4)))
    tmp <- tempfile(fileext=".rds")
    rds2cpp::write(y, tmp)

    expect_identical(y, readRDS(tmp))

    mine <- rds2cpp::parse_details(tmp)
    expect_identical(y, mine$value)
    expect_identical(mine$symbols, "names")

    refs <- scan_for_references(tmp)
    expect_true(length(refs) >= 2) # at least 2 refs of the 'names' symbol.
})

########################################################

y <- list(A=list("names", "foo"), B=123)
attr(y[[1]][[1]], "pretend-to-be-a-symbol") <- TRUE
attr(y[[1]][[2]], "pretend-to-be-a-symbol") <- TRUE
attr(y[[2]], "foo") <- TRUE

tmp <- tempfile(fileext=".rds")
rds2cpp::write(y, tmp)

test_that("redundancy across symbols and attributes are respected", {
    roundtrip <- readRDS(tmp)

    expect_identical(names(roundtrip), c("A", "B"))
    expect_type(roundtrip[[1]][[1]], "symbol")
    expect_type(roundtrip[[1]][[2]], "symbol")
    expect_identical(attr(roundtrip[[2]], "foo"), TRUE)

    refs <- scan_for_references(tmp)
    expect_true(length(unique(refs)) >= 2) # at least 2 refs of the 'names' and 'foo' symbols.
})

test_that("parser works correctly with more redundant symbols", {
    mine <- rds2cpp::parse_details(tmp)
    expect_identical(mine$symbols, c("names", "foo"))
    expect_identical(mine$value, list(A=list(list(id=0L), list(id=1L)), B=y$B))
})

