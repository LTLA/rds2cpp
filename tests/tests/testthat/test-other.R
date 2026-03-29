# library(testthat); library(rds2cpp); source("test-other.R")

test_that("length extraction works correctly", {
    # Remember this advances past the first byte.
    expect_identical(rds2cpp::parse_length(as.raw(c(0, 0, 0, 0, 16))), 16)
    expect_identical(rds2cpp::parse_length(as.raw(c(0, 0, 0, 255, 16))), 255 * 256 + 16)
    expect_identical(rds2cpp::parse_length(as.raw(c(0, 0, 10, 255, 16))), 10 * 256^2 + 255 * 256 + 16)
    expect_identical(rds2cpp::parse_length(as.raw(c(0, 5, 10, 255, 16))), 5 * 256^3 + 10 * 256^2 + 255 * 256 + 16)

    expect_error(rds2cpp::parse_length(as.raw(c(0, 255, 0, 0, 0)), "should be non-negative"))
    expect_identical(rds2cpp::parse_length(as.raw(c(0, 255, 255, 255, 255, 0, 0, 0, 1, 0, 0, 0, 1))), 2^32 + 1)
    expect_identical(rds2cpp::parse_length(as.raw(c(0, 255, 255, 255, 255, 0, 2, 0, 4, 0, 6, 0, 8))), 2 * 256^ 6 + 4 * 256^4 + 6 * 256^2 + 8)
})

test_that("string extraction works correctly", {
    expect_identical(rds2cpp::parse_single_string(as.raw(c(0, 0, 0, 2^7, 9, 0, 0, 0, 5, charToRaw('ABCDE')))), list("ABCDE", "UTF8", FALSE))
    expect_identical(rds2cpp::parse_single_string(as.raw(c(0, 0, 0, 2^6, 9, 0, 0, 0, 5, charToRaw('ABCDE')))), list("ABCDE", "LATIN1", FALSE))
    expect_identical(rds2cpp::parse_single_string(as.raw(c(0, 0, 2^2, 0, 9, 0, 0, 0, 5, charToRaw('ABCDE')))), list("ABCDE", "ASCII", FALSE))
    expect_identical(rds2cpp::parse_single_string(as.raw(c(0, 0, 0, 2^5, 9, 0, 0, 0, 5, charToRaw('ABCDE')))), list("ABCDE", "NONE", FALSE))

    expect_identical(rds2cpp::parse_single_string(as.raw(c(0, 0, 0, 2^7, 9, 0, 0, 0, 0))), list("", "UTF8", FALSE))
    expect_identical(rds2cpp::parse_single_string(as.raw(c(0, 0, 0, 2^5, 9, 255, 255, 255, 255))), list("", "NONE", TRUE))
    expect_error(rds2cpp::parse_single_string(as.raw(c(0, 0, 0, 2^5, 9, 255, 0, 0, 0))), "non-negative");
})
