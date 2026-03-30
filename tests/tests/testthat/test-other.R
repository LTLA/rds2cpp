# library(testthat); library(rds2cpp); source("setup.R"); source("test-other.R")

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
    expect_identical(rds2cpp::parse_single_string(as.raw(c(0, 0, 0, 2^7, 9, 0, 0, 0, 5, charToRaw('ABCDE')))), list("ABCDE", "UTF-8", FALSE))
    expect_identical(rds2cpp::parse_single_string(as.raw(c(0, 0, 0, 2^6, 9, 0, 0, 0, 5, charToRaw('ABCDE')))), list("ABCDE", "latin1", FALSE))
    expect_identical(rds2cpp::parse_single_string(as.raw(c(0, 0, 2^2, 0, 9, 0, 0, 0, 5, charToRaw('ABCDE')))), list("ABCDE", "unknown", FALSE))
    expect_identical(rds2cpp::parse_single_string(as.raw(c(0, 0, 0, 2^5, 9, 0, 0, 0, 5, charToRaw('ABCDE')))), list("ABCDE", "bytes", FALSE))

    expect_identical(rds2cpp::parse_single_string(as.raw(c(0, 0, 0, 2^7, 9, 0, 0, 0, 0))), list("", "UTF-8", FALSE))
    expect_identical(rds2cpp::parse_single_string(as.raw(c(0, 0, 0, 2^5, 9, 255, 255, 255, 255))), list("", "bytes", TRUE))
    expect_error(rds2cpp::parse_single_string(as.raw(c(0, 0, 0, 2^5, 9, 255, 0, 0, 0))), "non-negative");
})

test_that("RDS preamble extraction works correctly", {
    tmp <- tempfile(fileext=".rds")
    y <- as.integer(1:20)
    saveRDS(y, file=tmp)

    payload <- quick_parse(tmp)
    expect_identical(payload$format_version, 3L)
    expect_identical(paste(payload$writer_version, collapse="."), paste0(R.Version()$major, ".", R.Version()$minor))
    expect_identical(paste(payload$reader_version, collapse="."), "3.5.0")

    expect_true(payload$string_encoding %in% c("UTF-8", "latin1", "bytes", "unknown"))
})

set.seed(9999)
test_that("parallelized read/write of RDS files", {
    payload <- runif(100000)
    tmp <- tempfile(fileext=".rds")
    saveRDS(file=tmp, payload)

    roundtrip <- rds2cpp::parse_rds(tmp, parallel=TRUE)
    expect_identical(roundtrip$value, payload)

    rds2cpp::write_rds(payload, tmp, parallel=TRUE)
    roundtrip <- readRDS(tmp)
    expect_identical(roundtrip, payload)
})
