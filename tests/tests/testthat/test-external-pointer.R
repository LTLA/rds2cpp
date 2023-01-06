# Test that external pointer serialization works correctly.
# library(testthat); library(rds2cpp); source("test-external-pointer.R")

test_that("external pointer parsing works as expected", {
    H <- hashtab()
    ptr <- unclass(H)[[1]]
    tmp <- tempfile(fileext=".rds")
    saveRDS(ptr, file=tmp)

    roundtrip <- rds2cpp:::parse(tmp)
    expect_identical(roundtrip$value, list(external_pointer_id=0L))
    expect_identical(length(roundtrip$external_pointers), 1L)
    expect_true(is.list(roundtrip$external_pointers[[1]]$protection))
    expect_true(is.integer(roundtrip$external_pointers[[1]]$tag))
})

test_that("external pointer writing works as expected", {
    {
        mock <- list(c("FOO", "BAR"), NULL)
        attr(mock, "pretend-to-be-an-external-pointer") <- TRUE
        attr(mock, "external-pointer-index") <- 0L

        tmp <- tempfile(fileext=".rds")
        rds2cpp::write(mock, file=tmp)
        roundtrip <- readRDS(tmp)

        expect_type(roundtrip, "externalptr")
        expect_output(.Internal(inspect(roundtrip)), "PROTECTED")
        expect_output(.Internal(inspect(roundtrip)), "FOO")
    }

    {
        mock <- list(NULL, "evangelion")
        attr(mock, "pretend-to-be-an-external-pointer") <- TRUE
        attr(mock, "external-pointer-index") <- 0L

        tmp <- tempfile(fileext=".rds")
        rds2cpp::write(mock, file=tmp)
        roundtrip <- readRDS(tmp)

        expect_type(roundtrip, "externalptr")
        expect_output(.Internal(inspect(roundtrip)), "TAG")
        expect_output(.Internal(inspect(roundtrip)), "evangelion")
    }
})

test_that("external pointer parsing behaves properly as a reference object", {
    H <- hashtab()
    ptr <- unclass(H)[[1]]

    H2 <- hashtab()
    ptr2 <- unclass(H2)[[1]]

    tmp <- tempfile(fileext=".rds")
    saveRDS(list(ptr, ptr2, ptr2, ptr), file=tmp)

    roundtrip <- rds2cpp:::parse(tmp)
    expect_identical(roundtrip$value, list(list(external_pointer_id=0L), list(external_pointer_id=1L), list(external_pointer_id=1L), list(external_pointer_id=0L)))
    expect_identical(length(roundtrip$external_pointers), 2L)
})

test_that("external pointer writing behaves properly as a reference object", {
    collected <- list(
        first = {
            mock <- list(c("asuka"), NULL)
            attr(mock, "pretend-to-be-an-external-pointer") <- TRUE
            attr(mock, "external-pointer-index") <- 0L
            mock
        },
        second = {
            mock <- list(NULL, "rei")
            attr(mock, "pretend-to-be-an-external-pointer") <- TRUE
            attr(mock, "external-pointer-index") <- 1L
            mock
        },
        three = {
            mock <- list()
            attr(mock, "pretend-to-be-an-external-pointer") <- TRUE
            attr(mock, "external-pointer-index") <- 1L
            mock
        },
        four = {
            mock <- list()
            attr(mock, "pretend-to-be-an-external-pointer") <- TRUE
            attr(mock, "external-pointer-index") <- 0L
            mock
        }
    )

    tmp <- tempfile(fileext=".rds")
    rds2cpp::write(collected, file=tmp)
    roundtrip <- readRDS(tmp)

    # Correctly chooses the right external pointer reference.
    for (x in roundtrip) {
        expect_type(x, "externalptr")
    }
    expect_output(.Internal(inspect(roundtrip$first)), "asuka")
    expect_output(.Internal(inspect(roundtrip$second)), "rei")
    expect_output(.Internal(inspect(roundtrip$three)), "rei")
    expect_output(.Internal(inspect(roundtrip$four)), "asuka")
})

test_that("external pointer parsing works with attributes", {
    H <- hashtab()
    ptr <- unclass(H)[[1]]
    attr(ptr, "foo") <- "BAR"

    tmp <- tempfile(fileext=".rds")
    saveRDS(ptr, file=tmp)

    roundtrip <- rds2cpp:::parse(tmp)
    expect_identical(attr(roundtrip$external_pointers[[1]], "foo"), "BAR")
})

test_that("external pointer writing works with attributes", {
    mock <- list("misato", "shinji")
    attr(mock, "pretend-to-be-an-external-pointer") <- TRUE
    attr(mock, "external-pointer-index") <- 0L
    attr(mock, "FOO") <- "BAR"

    tmp <- tempfile(fileext=".rds")
    rds2cpp::write(mock, file=tmp)
    roundtrip <- readRDS(tmp)

    expect_type(roundtrip, "externalptr")
    expect_identical(attr(roundtrip, "FOO"), "BAR")
    expect_output(.Internal(inspect(roundtrip)), "misato")
    expect_output(.Internal(inspect(roundtrip)), "shinji")
})
