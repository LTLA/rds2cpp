# This tests the correct saving and loading of atomic vectors.
# library(testthat); library(rds2cpp); source("test-list.R")

########################################################

list.scenarios <- list(
    list(runif(10), runif(20), runif(30)),
    list(sample(letters), list(sample(11), runif(12))), # nested
    list(list(2, 6), list(5, c("cat", "dog", "bouse"), list(sample(99), runif(20)))), # deeply nested
    list(A=rnorm(5)), # simple named
    list(A=list(B=runif(2), C=5:1), D=list(A=5L, X=c("akari", "aika", "alice"))) # multiple named
)

test_that("list loading works as expected", {
    tmp <- tempfile(fileext=".rds")
    for (y in list.scenarios) {
        saveRDS(y, file=tmp)
        roundtrip <- rds2cpp:::parse(tmp)
        expect_identical(roundtrip$value, y)
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

########################################################

test.df <- data.frame(xxx=runif(19), YYY=sample(letters, 19), ZZZ=rbinom(19, 1, 0.4) == 0)
test.df.with.rownames <- test.df
rownames(test.df.with.rownames) <- paste0("FOO-", LETTERS[1:19])

test_that("data frame loading works as expected", {
    tmp <- tempfile(fileext=".rds")
    saveRDS(test.df, file=tmp)
    roundtrip <- rds2cpp:::parse(tmp)
    expect_identical(roundtrip$value, test.df)

    # Works with row names.
    saveRDS(test.df.with.rownames, file=tmp)
    roundtrip <- rds2cpp:::parse(tmp)
    expect_identical(roundtrip$value, test.df.with.rownames)
})

test_that("data frame loading works as expected", {
    tmp <- tempfile(fileext=".rds")
    rds2cpp::write(test.df, file=tmp)
    roundtrip <- readRDS(tmp)
    expect_identical(roundtrip, test.df)

    # Works with row names.
    rds2cpp::write(test.df.with.rownames, file=tmp)
    roundtrip <- readRDS(tmp)
    expect_identical(roundtrip, test.df.with.rownames)
})
