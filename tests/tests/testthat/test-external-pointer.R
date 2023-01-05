# Test that external pointer serialization works correctly.
# library(testthat); library(rds2cpp); source("test-external-pointer.R")

test_that("external pointer parsing works as expected", {
    H <- hashtab()
    ptr <- unclass(H)[[1]]
    tmp <- tempfile(fileext=".rds")
    saveRDS(ptr, file=tmp)

    roundtrip <- rds2cpp:::parse_details(tmp)
    expect_identical(roundtrip$value, list(id=0L))
    expect_identical(length(roundtrip$external_pointers), 1L)
    expect_true(is.list(roundtrip$external_pointers[[1]]$protection))
    expect_true(is.integer(roundtrip$external_pointers[[1]]$tag))
})

test_that("external pointer parsing behaves properly as a reference object", {
    H <- hashtab()
    ptr <- unclass(H)[[1]]

    H2 <- hashtab()
    ptr2 <- unclass(H2)[[1]]

    tmp <- tempfile(fileext=".rds")
    saveRDS(list(ptr, ptr2, ptr2, ptr), file=tmp)

    roundtrip <- rds2cpp:::parse_details(tmp)
    expect_identical(roundtrip$value, list(list(id=0L), list(id=1L), list(id=1L), list(id=0L)))
    expect_identical(length(roundtrip$external_pointers), 2L)
})

test_that("external pointer parsing works with attributes", {
    H <- hashtab()
    ptr <- unclass(H)[[1]]
    attr(ptr, "foo") <- "BAR"

    tmp <- tempfile(fileext=".rds")
    saveRDS(ptr, file=tmp)

    roundtrip <- rds2cpp:::parse_details(tmp)
    expect_identical(attr(roundtrip$external_pointers[[1]], "foo"), "BAR")
})

