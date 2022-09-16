# Test that environment serialization works correctly.
# library(testthat); library(rds2cpp); source("test-environment.R")

test_that("empty environment loading works as expected", {
    # This is needed to deal with the fact that the parent
    # environment is mangled somewhat by testthat.
    roundtrip <- local({
        y <- new.env()
        tmp <- tempfile(fileext=".rds")
        saveRDS(y, file=tmp)
        rds2cpp:::parse(tmp)
    }, envir=.GlobalEnv)

    expect_identical(roundtrip$value$id, 0L)
    expect_identical(length(roundtrip$environments), 1L)
    expect_identical(roundtrip$environments[[1]]$parent, -1L)
    expect_identical(length(roundtrip$environments[[1]]$variables), 0L)

    saveRDS(.GlobalEnv, file=tmp)
    roundtrip <- rds2cpp:::parse(tmp)
    expect_identical(roundtrip$id, -1L)
})

test_that("non-empty environment loading works as expected", {
    output <- local({
        y <- new.env()
        y$AAA <- rnorm(10)
        y$BBB <- runif(100)

        tmp <- tempfile(fileext=".rds")
        saveRDS(y, file=tmp)
        list(roundtrip = rds2cpp:::parse(tmp), ref = y)
    }, envir=.GlobalEnv)

    roundtrip <- output$roundtrip
    ref <- output$ref
    expect_identical(length(roundtrip$environments), 1L)
    expect_identical(length(roundtrip$environments[[1]]$variables), 2L)

    expect_identical(roundtrip$value$id, 0L)
    expect_identical(roundtrip$environments[[1]]$parent, -1L)
    expect_identical(roundtrip$environments[[1]]$variables$AAA, ref$AAA)
    expect_identical(roundtrip$environments[[1]]$variables$BBB, ref$BBB)

    # Something a little more involved.
    output <- local({
        y <- new.env()
        y$Aaron <- 2:60
        y$Jayaram <- as.character(5:10)
        y$Michael <- list(1:5)

        saveRDS(y, file=tmp)
        rds2cpp:::parse(tmp)
        list(roundtrip = rds2cpp:::parse(tmp), ref = y)
    }, envir=.GlobalEnv)

    roundtrip <- output$roundtrip
    ref <- output$ref
    expect_identical(length(roundtrip$environments), 1L)
    expect_identical(length(roundtrip$environments[[1]]$variables), 3L)

    expect_identical(roundtrip$value$id, 0L)
    expect_identical(roundtrip$environments[[1]]$parent, -1L)
    expect_identical(roundtrip$environments[[1]]$variables$Aaron, ref$Aaron)
    expect_identical(roundtrip$environments[[1]]$variables$Jayaram, ref$Jayaram)
    expect_identical(roundtrip$environments[[1]]$variables$Michael, ref$Michael)
})

test_that("environment references work as expected", {
    roundtrip <- local({
        y <- new.env()
        tmp <- tempfile(fileext=".rds")
        saveRDS(list(y, y), file=tmp)
        rds2cpp:::parse(tmp)
    }, envir=.GlobalEnv)

    expect_identical(roundtrip$value[[1]]$id, 0L)
    expect_identical(roundtrip$value[[2]]$id, 0L)

    expect_identical(length(roundtrip$environments), 1L)
    expect_identical(roundtrip$environments[[1]]$parent, -1L)
    expect_identical(length(roundtrip$environments[[1]]$variables), 0L)

    # More complicated links to existing references.
    roundtrip <- local({
        y <- new.env()
        z <- new.env()
        z$foo <- "BAR"
        saveRDS(list(z, y, z, y), file=tmp)
        rds2cpp:::parse(tmp)
    }, envir=.GlobalEnv)

    expect_identical(roundtrip$value[[1]]$id, 0L)
    expect_identical(roundtrip$value[[2]]$id, 1L)
    expect_identical(roundtrip$value[[3]]$id, 0L)
    expect_identical(roundtrip$value[[4]]$id, 1L)

    expect_identical(length(roundtrip$environments), 2L)

    expect_identical(length(roundtrip$environments[[1]]$variables), 1L)
    expect_identical(roundtrip$environments[[1]]$variables$foo, "BAR")
    expect_identical(roundtrip$environments[[1]]$parent, -1L)

    expect_identical(length(roundtrip$environments[[2]]$variables), 0L)
    expect_identical(roundtrip$environments[[2]]$parent, -1L)

    # Mix in some global references.
    roundtrip2 <- local({
        y <- new.env()
        z <- new.env()
        z$foo <- "BAR"
        saveRDS(list(.GlobalEnv, z, y, z, y, .GlobalEnv), file=tmp)
        rds2cpp:::parse(tmp)
    }, envir=.GlobalEnv)

    expect_identical(roundtrip2$value[[1]]$id, -1L)
    expect_identical(roundtrip2$value[[2]]$id, 0L)
    expect_identical(roundtrip2$value[[3]]$id, 1L)
    expect_identical(roundtrip2$value[[4]]$id, 0L)
    expect_identical(roundtrip2$value[[5]]$id, 1L)
    expect_identical(roundtrip2$value[[6]]$id, -1L)

    expect_identical(roundtrip$environments, roundtrip2$environments)
})

test_that("environment parenthood works as expected", {
    roundtrip <- local({
        y <- new.env()
        y$level <- "A"

        z <- new.env(parent=y)
        z$level <- "B"

        tmp <- tempfile(fileext=".rds")
        saveRDS(z, file=tmp)
        rds2cpp:::parse(tmp)
    }, envir=.GlobalEnv)

    expect_identical(roundtrip$value$id, 0L)
    expect_identical(length(roundtrip$environments), 2L)

    expect_identical(roundtrip$environments[[1]]$parent, 1L)
    expect_identical(length(roundtrip$environments[[1]]$variables), 1L)
    expect_identical(roundtrip$environments[[1]]$variables$level, "B")

    expect_identical(roundtrip$environments[[2]]$parent, -1L)
    expect_identical(length(roundtrip$environments[[2]]$variables), 1L)
    expect_identical(roundtrip$environments[[2]]$variables$level, "A")

    # Trying a multi parenthood chain.
    roundtrip <- local({
        y <- new.env()
        y$level <- "A"

        z1 <- new.env(parent=y)
        z1$level <- "B1"

        z2 <- new.env(parent=y)
        z2$level <- "B2"

        tmp <- tempfile(fileext=".rds")
        saveRDS(list(z1, z2), file=tmp)
        rds2cpp:::parse(tmp)
    }, envir=.GlobalEnv)

    expect_identical(roundtrip$value, list(list(id=0L), list(id=2L)))
    expect_identical(length(roundtrip$environments), 3L)

    expect_identical(roundtrip$environments[[1]]$parent, 1L)
    expect_identical(length(roundtrip$environments[[1]]$variables), 1L)
    expect_identical(roundtrip$environments[[1]]$variables$level, "B1")

    expect_identical(roundtrip$environments[[2]]$parent, -1L)
    expect_identical(length(roundtrip$environments[[2]]$variables), 1L)
    expect_identical(roundtrip$environments[[2]]$variables$level, "A")

    expect_identical(roundtrip$environments[[3]]$parent, 1L)
    expect_identical(length(roundtrip$environments[[3]]$variables), 1L)
    expect_identical(roundtrip$environments[[3]]$variables$level, "B2")
})

test_that("self-references are properly resolved", {
    output <- local({
        y <- new.env()
        y$foo <- 1:10
        y$bar <- as.character(100:200)

        y$stuff <- y

        tmp <- tempfile(fileext=".rds")
        saveRDS(y, file=tmp)
        list(roundtrip = rds2cpp:::parse(tmp), ref = y)
    }, envir=.GlobalEnv)

    roundtrip <- output$roundtrip
    ref <- output$ref
    expect_identical(roundtrip$value, list(id=0L))
    expect_identical(length(roundtrip$environments), 1L)

    expect_identical(roundtrip$environments[[1]]$parent, -1L)
    expect_identical(length(roundtrip$environments[[1]]$variables), 3L)

    expect_identical(roundtrip$environments[[1]]$variables$foo, 1:10)
    expect_identical(roundtrip$environments[[1]]$variables$bar, as.character(100:200))
    expect_identical(roundtrip$environments[[1]]$variables$stuff, list(id=0L)) # can refer to itself, no probs.
})
