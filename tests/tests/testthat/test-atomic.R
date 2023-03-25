# This tests the correct saving and loading of atomic vectors.
# library(testthat); library(rds2cpp); source("test-atomic.R")

########################################################

integer.scenarios <- list(
    sample(15),
    rpois(112, lambda=8),
    rnorm(990) * 10,
    {
        y <- 0:99
        y[sample(length(y), 10)] <- NA
        y
    }
)

test_that("integer vector loading works as expected", {
    tmp <- tempfile(fileext=".rds")
    for (y in integer.scenarios) {
        y <- as.integer(y)
        saveRDS(y, file=tmp)
        roundtrip <- rds2cpp:::parse(tmp)
        expect_identical(roundtrip$value, y)
    }
})

test_that("integer vector writing works as expected", {
    tmp <- tempfile(fileext=".rds")
    for (y in integer.scenarios) {
        y <- as.integer(y)
        rds2cpp::write(y, file_name=tmp)
        roundtrip <- readRDS(tmp)
        expect_identical(roundtrip, y)
    }
})

########################################################

logical.scenarios <- list(
    rbinom(55, 1, 0.5) == 0,
    {
        y <- rbinom(999, 1, 0.5) == 0
        y[sample(length(y), 10)] <- NA
        y
    }
)

test_that("logical vector loading works as expected", {
    tmp <- tempfile(fileext=".rds")
    for (y in logical.scenarios) {
        saveRDS(y, file=tmp)
        roundtrip <- rds2cpp:::parse(tmp)
        expect_identical(roundtrip$value, y)
    }
})

test_that("logical vector writing works as expected", {
    tmp <- tempfile(fileext=".rds")
    for (y in logical.scenarios) {
        rds2cpp::write(y, file_name=tmp)
        roundtrip <- readRDS(tmp)
        expect_identical(roundtrip, y)
    }
})

########################################################

double.scenarios <- list(
    rnorm(99),
    rgamma(1, 2, 1),
    rexp(1000),
    {
        y <- rnorm(999)
        y[sample(length(y), 10)] <- NA
        y[sample(length(y), 10)] <- NaN
        y[sample(length(y), 10)] <- Inf
        y[sample(length(y), 10)] <- -Inf
        y
    }
)

test_that("double vector loading works as expected", {
    tmp <- tempfile(fileext=".rds")
    for (y in double.scenarios) {
        saveRDS(y, file=tmp)
        roundtrip <- rds2cpp:::parse(tmp)
        expect_identical(roundtrip$value, y)
    }
})

test_that("double vector writing works as expected", {
    tmp <- tempfile(fileext=".rds")
    for (y in double.scenarios) {
        rds2cpp::write(y, file_name=tmp)
        roundtrip <- readRDS(tmp)
        expect_identical(roundtrip, y)
    }
})

########################################################

test_that("raw vector loading works as expected", {
    tmp <- tempfile(fileext=".rds")
    y <- as.raw(sample(256, 99, replace=TRUE) - 1)
    saveRDS(y, file=tmp)
    roundtrip <- rds2cpp:::parse(tmp)
    expect_identical(roundtrip$value, y)
})

test_that("raw vector writing works as expected", {
    tmp <- tempfile(fileext=".rds")
    y <- as.raw(sample(256, 99, replace=TRUE) - 1)
    rds2cpp::write(y, file_name=tmp)
    roundtrip <- readRDS(tmp)
    expect_identical(roundtrip, y)
})

########################################################

complex.scenarios <- list(
    rnorm(99) + rnorm(99) * 1i,
    rgamma(1, 2, 1) * 2i,
    rexp(1000) + 0i,
    {
        y <- rnorm(999) + rnorm(999) * 1i
        y[sample(length(y), 10)] <- NA
        y[sample(length(y), 10)] <- NaN
        y[sample(length(y), 10)] <- Inf
        y[sample(length(y), 10)] <- -Inf
        y
    }
)

test_that("complex vector loading works as expected", {
    tmp <- tempfile(fileext=".rds")
    for (y in complex.scenarios) {
        saveRDS(y, file=tmp)
        roundtrip <- rds2cpp:::parse(tmp)
        expect_identical(roundtrip$value, y)
    }
})

test_that("complex vector loading works as expected", {
    tmp <- tempfile(fileext=".rds")
    for (y in complex.scenarios) {
        rds2cpp::write(y, file_name=tmp)
        roundtrip <- readRDS(tmp)
        expect_identical(roundtrip, y)
    }
})

########################################################

string.scenarios <- list(
    sample(LETTERS),
    c("Aaron", "Lun", "was", "here!"),
    c("Akari", NA, "Alicia", "Alice", NA, "Athena", "Aika", "Akira", NA),
    c(
        paste(sample(LETTERS, 100, replace=TRUE), collapse=""),
        paste(sample(LETTERS, 1000, replace=TRUE), collapse=""),
        paste(sample(LETTERS, 10000, replace=TRUE), collapse="")
    ),
    c("α-globin", "😀😀😀", "fußball", "Hervé Pagès")
)

test_that("character vector loading works as expected", {
    tmp <- tempfile(fileext=".rds")
    for (y in string.scenarios) {
        saveRDS(y, file=tmp)
        roundtrip <- rds2cpp:::parse(tmp)
        expect_identical(roundtrip$value, y)
    }
})

test_that("character vector writing works as expected", {
    tmp <- tempfile(fileext=".rds")
    for (y in string.scenarios) {
        rds2cpp::write(y, file_name=tmp)
        roundtrip <- readRDS(tmp)
        expect_identical(roundtrip, y)
    }
})

########################################################

attr_vals <- sample(.Machine$integer.max, 1000)
names(attr_vals) <- sprintf("GENE_%i", seq_along(attr_vals))
attr(attr_vals, "foo") <- c("BAR", "bar", "Bar")
class(attr_vals) <- "frog"

test_that("attributes for atomic vectors are loaded correctly", {
    tmp <- tempfile(fileext=".rds")
    saveRDS(attr_vals, file=tmp)
    roundtrip <- rds2cpp:::parse(tmp)
    expect_identical(roundtrip$value, attr_vals)

    # Works if the attributes are wiped; this should be the
    # same as not setting them in the first place.
    wiped <- attr_vals
    attributes(wiped) <- NULL
    names(wiped) <- NULL
    saveRDS(wiped, file=tmp)
    roundtrip <- rds2cpp:::parse(tmp)
    expect_identical(roundtrip$value, wiped)
})

test_that("attributes for atomic vectors are written correctly", {
    tmp <- tempfile(fileext=".rds")
    rds2cpp::write(attr_vals, tmp)
    roundtrip <- readRDS(tmp)
    expect_identical(roundtrip, attr_vals)
})
