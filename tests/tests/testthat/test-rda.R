# library(testthat); library(rds2cpp); source("setup.R"); source("test-rda.R")

test_that("reading RDA files (simple)", {
    X <- as.integer(c(1,1,2,3,5,8))
    Y <- LETTERS
    Z <- list(A = TRUE, B = FALSE)

    tmp <- tempfile(fileext=".Rda")
    save(file=tmp, list=c("X", "Y", "Z"))

    roundtrip <- rds2cpp::parse_rda(tmp, parallel=FALSE)
    expect_true(attr(roundtrip$value, "pretend-to-be-a-pairlist"))
    expect_identical(roundtrip$value$data, list(X, Y, Z))
    expect_identical(roundtrip$value$tag, c("X", "Y", "Z"))

    expect_identical(roundtrip$environments, list())
    expect_identical(roundtrip$symbols, c("X", "Y", "Z", "names"))
    expect_identical(roundtrip$external_pointers, list())
})

test_that("reading RDA files (complex)", {
    #  Make sure parent is the global env to avoid too much shenanigans.
    roundtrip <- local({
        X <- new.env()
        X$FOO <- as.integer(c(1,1,2,3,5,8))
        Y <- unclass(hashtab(size=5))[[1]]
        Z <- quote(aaron)

        # More than one of each of these reference types.
        A <- new.env() 
        A$BAR <- setNames(FALSE, "stuff")
        B <- unclass(hashtab(size=10))[[1]]
        C <- quote(lun)

        # Multiple references to the same object.
        M <- X
        N <- Y
        O <- Z

        tmp <- tempfile(fileext=".Rda")
        save(file=tmp, list=c("X", "Y", "Z", "A", "B", "C", "M", "N", "O"))

        rds2cpp::parse_rda(tmp, parallel=FALSE)
    }, envir=.GlobalEnv)

    expect_true(attr(roundtrip$value, "pretend-to-be-a-pairlist"))
    expect_identical(roundtrip$value$data[[1]]$environment_id, 0L)
    expect_identical(roundtrip$value$data[[2]]$external_pointer_id, 0L)
    expect_identical(roundtrip$symbols[roundtrip$value$data[[3]]$symbol_id + 1L], "aaron")
    expect_identical(roundtrip$value$data[[4]]$environment_id, 1L)
    expect_identical(roundtrip$value$data[[5]]$external_pointer_id, 1L)
    expect_identical(roundtrip$symbols[roundtrip$value$data[[6]]$symbol_id + 1L], "lun")
    expect_identical(roundtrip$value$data[[7]]$environment_id, 0L)
    expect_identical(roundtrip$value$data[[8]]$external_pointer_id, 0L)
    expect_identical(roundtrip$value$data[[9]]$symbol_id, roundtrip$value$data[[3]]$symbol_id)

    expect_identical(roundtrip$value$tag, c("X", "Y", "Z", "A", "B", "C", "M", "N", "O"))
    expect_identical(sort(roundtrip$symbols), sort(c("X", "Y", "Z", "A", "B", "C", "M", "N", "O", "aaron", "lun", "names", "FOO", "BAR")))

    expect_identical(roundtrip$environments[[1]]$variables, list(FOO=X$FOO))
    expect_identical(roundtrip$environments[[2]]$variables, list(BAR=A$BAR))

    expect_identical(length(roundtrip$external_pointers), 2L)
})

test_that("writing RDA files (simple)", {
    pl <- list(
        X = c(1,1,2,3,5,8),
        Y = letters,
        Z = list(aa = TRUE, bb = FALSE)
    )
    attr(pl, "pretend-to-be-a-pairlist") <- TRUE

    tmp <- tempfile(fileext=".Rda")
    rds2cpp::write_rda(pl, tmp, parallel=FALSE)

    env <- new.env()
    load(tmp, envir=env)
    expect_identical(ls(env), c("X", "Y", "Z"))
    expect_identical(env$X, pl$X)
    expect_identical(env$Y, pl$Y)
    expect_identical(env$Z, pl$Z)

    # Same results when parallelized.
    rds2cpp::write_rda(pl, tmp, parallel=TRUE)
    env2 <- new.env()
    load(tmp, envir=env2)
    expect_equal(env, env2)
})

test_that("writing RDA files (complex)", {
    X <- list(FOO = as.integer(c(1,1,2,3,5,8)))
    attr(X, "pretend-to-be-an-environment") <- TRUE
    attr(X, "environment-index") <- 0L
    attr(X, "environment-parent") <- -1L
    attr(X, "environment-locked") <- FALSE 

    Y <- list(c("akari"), NULL)
    attr(Y, "pretend-to-be-an-external-pointer") <- TRUE
    attr(Y, "external-pointer-index") <- 0L

    Z <- "ai"
    attr(Z, "pretend-to-be-a-symbol") <- TRUE

    A <- list(BAR = setNames(FALSE, "stuff"))
    attr(A, "pretend-to-be-an-environment") <- TRUE
    attr(A, "environment-index") <- 1L
    attr(A, "environment-parent") <- -1L
    attr(A, "environment-locked") <- FALSE 

    B <- list(c("aika"), NULL)
    attr(B, "pretend-to-be-an-external-pointer") <- TRUE
    attr(B, "external-pointer-index") <- 1L

    C <- "alice"
    attr(C, "pretend-to-be-a-symbol") <- TRUE

    pl <- list(
        X = X, Y = Y, Z = Z,
        A = A, B = B, C = C,
        M = X, N = Y, O = Z
    )
    attr(pl, "pretend-to-be-a-pairlist") <- TRUE

    tmp <- tempfile(fileext=".Rda")
    rds2cpp::write_rda(pl, tmp, parallel=FALSE)

    env <- new.env()
    load(tmp, envir=env)
    expect_identical(sort(ls(env)), sort(c("X", "Y", "Z", "A", "B", "C", "M", "N", "O")))

    expect_type(env$X, "environment")
    expect_identical(ls(env$X), "FOO")
    expect_identical(env$X$FOO, X$FOO)

    expect_type(env$Y, "externalptr")
    expect_output(.Internal(inspect(env$Y)), "PROTECTED")
    expect_output(.Internal(inspect(env$Y)), "akari")

    expect_identical(env$Z, quote(ai))

    expect_type(env$A, "environment")
    expect_identical(ls(env$A), "BAR")
    expect_identical(env$X$BAR, X$BAR)

    expect_type(env$B, "externalptr")
    expect_output(.Internal(inspect(env$B)), "PROTECTED")
    expect_output(.Internal(inspect(env$B)), "aika")

    expect_identical(env$Z, quote(ai))

    expect_identical(env$X, env$M)
    expect_identical(env$Y, env$N)
    expect_identical(env$Z, env$O)
})

test_that("RDS preamble extraction works correctly", {
    tmp <- tempfile(fileext=".rds")
    y <- as.integer(1:20)
    save(file=tmp, list="y")

    payload <- rds2cpp::parse_rda(tmp, parallel=FALSE)
    expect_identical(payload$format_version, 3L)
    expect_identical(paste(payload$writer_version, collapse="."), paste0(R.Version()$major, ".", R.Version()$minor))
    expect_identical(paste(payload$reader_version, collapse="."), "3.5.0")

    expect_true(payload$string_encoding %in% c("UTF-8", "latin1", "bytes", "unknown"))
})

set.seed(9999)
test_that("parallelized read/write of RDA files", {
    payload <- runif(100000)
    tmp <- tempfile(fileext=".Rda")
    save(file=tmp, list=c("payload"))

    roundtrip <- rds2cpp::parse_rda(tmp, parallel=TRUE)
    expect_identical(roundtrip$value$data[[1]], payload)
    expect_identical(roundtrip$value$tag, "payload")

    pl <- list(payload = payload)
    attr(pl, "pretend-to-be-a-pairlist") <- TRUE
    rds2cpp::write_rda(pl, tmp, parallel=TRUE)
    env <- new.env()
    load(tmp, envir=env)
    expect_identical(env$payload, payload)
})
