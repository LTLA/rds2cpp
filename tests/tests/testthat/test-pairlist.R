# library(testthat); library(rds2cpp); source("setup.R"); source("test-pairlist.R")

test_that("pairlist loading works as expected", {
    tmp <- tempfile(fileext=".rds")

    y <- pairlist(runif(10), runif(20), runif(30))
    saveRDS(y, file=tmp)
    roundtrip <- quick_parse(tmp)
    expect_true(attr(roundtrip$value, "pretend-to-be-a-pairlist"))
    expect_identical(roundtrip$value$data, as.list(y))
    expect_true(all(is.na(roundtrip$value$tag)))

    # Now with some nesting. 
    y <- pairlist(sample(letters), pairlist(sample(11), runif(12)))
    saveRDS(y, file=tmp)
    roundtrip <- quick_parse(tmp)
    expect_true(attr(roundtrip$value, "pretend-to-be-a-pairlist"))
    expect_identical(roundtrip$value$data[[1]], y[[1]])
    expect_identical(roundtrip$value$data[[2]]$data, as.list(y[[2]]))
    expect_true(all(is.na(roundtrip$value$tag)))
})

test_that("pairlist loading works with names", {
    tmp <- tempfile(fileext=".rds")

    y <- pairlist(foo=sample(letters), bar=pairlist(whee=sample(11), bum=runif(12))) # with names
    saveRDS(y, file=tmp)
    roundtrip <- quick_parse(tmp)
    expect_true(attr(roundtrip$value, "pretend-to-be-a-pairlist"))
    expect_identical(roundtrip$value$data[[1]], y[[1]])
    expect_identical(roundtrip$value$tag, names(y))
    expect_identical(setNames(roundtrip$value$data[[2]]$data, roundtrip$value$data[[2]]$tag), as.list(y[[2]]))

    # Only partially named.
    y <- pairlist(sample(letters), bar=TRUE, FALSE, foo=1:5)
    saveRDS(y, file=tmp)
    roundtrip <- quick_parse(tmp)
    expect_true(attr(roundtrip$value, "pretend-to-be-a-pairlist"))
    expect_identical(roundtrip$value$data, unname(as.list(y)))
    expect_identical(roundtrip$value$tag, c(NA, "bar", NA, "foo"))
})

test_that("pairlist loading works with attributes", {
    y <- pairlist(aaron=sample(letters), bar=list(sample(11), runif(12)))
    attr(y, "foo") <- "bar"
    attr(y, "stuff") <- 1:100

    tmp <- tempfile(fileext=".rds")
    saveRDS(y, file=tmp)

    roundtrip <- quick_parse(tmp)
    expect_true(attr(roundtrip$value, "pretend-to-be-a-pairlist"))

    massaged <- setNames(roundtrip$value$data, roundtrip$value$tag)
    attr(massaged, 'foo') <- attr(roundtrip$value, 'foo')
    attr(massaged, 'stuff') <- attr(roundtrip$value, 'stuff')
    expect_identical(massaged, as.list(y))
})

test_that("pairlist writing works as expected", {
    tmp <- tempfile(fileext=".rds")

    y <- list(runif(10), runif(20), runif(30))
    attr(y, "pretend-to-be-a-pairlist") <- TRUE
    quick_write(y, tmp)
    roundtrip <- readRDS(tmp)
    expect_identical(roundtrip, do.call(pairlist, y))

    # Now with some nesting. 
    y <- list(sample(letters), list(sample(11), runif(12)))
    attr(y, "pretend-to-be-a-pairlist") <- TRUE
    attr(y[[2]], "pretend-to-be-a-pairlist") <- TRUE
    quick_write(y, tmp)
    roundtrip <- readRDS(tmp)
    expect_identical(roundtrip, pairlist(y[[1]], do.call(pairlist, y[[2]])))
})

test_that("pairlist writing works with names", {
    tmp <- tempfile(fileext=".rds")

    y <- list(foo=sample(letters), bar=list(whee=sample(11), bum=runif(12))) # with names
    attr(y, "pretend-to-be-a-pairlist") <- TRUE
    attr(y[[2]], "pretend-to-be-a-pairlist") <- TRUE
    quick_write(y, tmp)
    roundtrip <- readRDS(tmp)
    expect_identical(roundtrip, pairlist(foo=y[[1]], bar=pairlist(whee=y$bar$whee, bum=y$bar$bum)))

    # Only partially named.
    y <- list(sample(letters), bar=TRUE, FALSE, foo=1:5)
    attr(y, "pretend-to-be-a-pairlist") <- TRUE
    quick_write(y, file=tmp)
    roundtrip <- readRDS(tmp)
    expect_identical(roundtrip, pairlist(y[[1]], bar=y[[2]], y[[3]], foo=y[[4]]))
})

test_that("pairlist writing works with attributes", {
    y <- list(aaron=sample(letters), bar=list(sample(11), runif(12)))
    attr(y, "pretend-to-be-a-pairlist") <- "bar"
    attr(y, "foo") <- "bar"
    attr(y, "stuff") <- 1:100

    tmp <- tempfile(fileext=".rds")
    quick_write(y, file=tmp)

    roundtrip <- readRDS(tmp)
    expected <- do.call(pairlist, y)
    attr(expected, "foo") <- "bar"
    attr(expected, "stuff") <- 1:100
    expect_identical(roundtrip, expected)
})
