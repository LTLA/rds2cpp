# This tests the correct saving and loading of atomic vectors.
# library(testthat); library(rds2cpp); source("test-list.R")

list.scenarios <- list(
    list(runif(10), runif(20), runif(30)),
    list(sample(letters), list(sample(11), runif(12))), # nested
    list(list(2, 6), list(5, c("cat", "dog", "bouse"), list(sample(99), runif(20)))) # deeply nested
)

test_that("list loading works as expected", {
    tmp <- tempfile(fileext=".rds")
    for (y in list.scenarios) {
        saveRDS(y, file=tmp)
        roundtrip <- rds2cpp:::parse(tmp)
        expect_identical(roundtrip, y)
    }
})

test_that("list writing works as expected", {
    tmp <- tempfile(fileext=".rds")
    for (y in list.scenarios) {
        rds2cpp::write(y, tmp)
        roundtrip <- readRDS(tmp)
        expect_identical(roundtrip, y)
    }
})

test_that("data frame loading works as expected", {
    tmp <- tempfile(fileext=".rds")
    df <- data.frame(xxx=runif(19), YYY=sample(letters, 19), ZZZ=rbinom(19, 1, 0.4) == 0)
    saveRDS(df, file=tmp)
    roundtrip <- rds2cpp:::parse(tmp)
    expect_identical(roundtrip, df)

    # Works with row names.
    rownames(df) <- paste0("FOO-", LETTERS[1:19])
    saveRDS(df, file=tmp)
    roundtrip <- rds2cpp:::parse(tmp)
    expect_identical(roundtrip, df)
})
