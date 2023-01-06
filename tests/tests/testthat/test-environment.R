# Test that environment serialization works correctly.
# library(testthat); library(rds2cpp); source("test-environment.R")

test_that("empty environment loading works as expected", {
    # local() is needed to deal with the fact that the parent
    # environment is mangled somewhat by testthat.
    roundtrip <- local({
        y <- new.env()
        tmp <- tempfile(fileext=".rds")
        saveRDS(y, file=tmp)
        rds2cpp:::parse(tmp)
    }, envir=.GlobalEnv)

    expect_identical(roundtrip$value$environment_id, 0L)
    expect_identical(length(roundtrip$environments), 1L)
    expect_identical(roundtrip$environments[[1]]$parent, -1L)
    expect_identical(length(roundtrip$environments[[1]]$variables), 0L)
    expect_false(roundtrip$environments[[1]]$locked)

    saveRDS(.GlobalEnv, file=tmp)
    roundtrip <- rds2cpp:::parse(tmp)
    expect_identical(roundtrip$value$environment_id, -1L)
})

test_that("empty environment writing works as expected", {
    y <- list()
    attr(y, "pretend-to-be-an-environment") <- TRUE
    attr(y, "environment-index") <- 0L
    attr(y, "environment-parent") <- -1L
    attr(y, "environment-locked") <- FALSE
    names(y) <- character(0)

    tmp <- tempfile(fileext=".rds")
    rds2cpp::write(y, file=tmp)
    roundtrip <- readRDS(tmp)

    expect_type(roundtrip, "environment")
    expect_identical(ls(roundtrip), character(0))
    expect_false(environmentIsLocked(roundtrip))
    expect_identical(parent.env(roundtrip), .GlobalEnv)

    # Works for the global environment.
    y <- list()
    attr(y, "pretend-to-be-an-environment") <- TRUE
    attr(y, "environment-index") <- -1L
    names(y) <- character(0)

    rds2cpp::write(y, file=tmp)
    roundtrip <- readRDS(tmp)
    expect_identical(roundtrip, .GlobalEnv)
})

test_that("locked environment loading works as expected", {
    # local() is needed to deal with the fact that the parent
    # environment is mangled somewhat by testthat.
    roundtrip <- local({
        y <- new.env()
        lockEnvironment(y)
        tmp <- tempfile(fileext=".rds")
        saveRDS(y, file=tmp)
        rds2cpp:::parse(tmp)
    }, envir=.GlobalEnv)

    expect_identical(roundtrip$value$environment_id, 0L)
    expect_identical(length(roundtrip$environments), 1L)
    expect_identical(roundtrip$environments[[1]]$parent, -1L)
    expect_identical(length(roundtrip$environments[[1]]$variables), 0L)
    expect_true(roundtrip$environments[[1]]$locked)
})

test_that("locked environment writing works as expected", {
    y <- list()
    attr(y, "pretend-to-be-an-environment") <- TRUE
    attr(y, "environment-index") <- 0L
    attr(y, "environment-parent") <- -1L
    attr(y, "environment-locked") <- TRUE 
    names(y) <- character(0)

    tmp <- tempfile(fileext=".rds")
    rds2cpp::write(y, file=tmp)
    roundtrip <- readRDS(tmp)

    expect_type(roundtrip, "environment")
    expect_true(environmentIsLocked(roundtrip))
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

    expect_identical(roundtrip$value$environment_id, 0L)
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
        list(roundtrip = rds2cpp:::parse(tmp), ref = y)
    }, envir=.GlobalEnv)

    roundtrip <- output$roundtrip
    ref <- output$ref
    expect_identical(length(roundtrip$environments), 1L)
    expect_identical(length(roundtrip$environments[[1]]$variables), 3L)

    expect_identical(roundtrip$value$environment_id, 0L)
    expect_identical(roundtrip$environments[[1]]$parent, -1L)
    expect_identical(roundtrip$environments[[1]]$variables$Aaron, ref$Aaron)
    expect_identical(roundtrip$environments[[1]]$variables$Jayaram, ref$Jayaram)
    expect_identical(roundtrip$environments[[1]]$variables$Michael, ref$Michael)
})

test_that("unhashed environment loading works as expected", {
    output <- local({
        y <- new.env(hash=FALSE)
        y$aoyama <- rnorm(10)
        y$blue <- "mountain"

        tmp <- tempfile(fileext=".rds")
        saveRDS(y, file=tmp)
        list(roundtrip = rds2cpp:::parse(tmp), ref = y)
    }, envir=.GlobalEnv)

    roundtrip <- output$roundtrip
    ref <- output$ref
    expect_identical(length(roundtrip$environments), 1L)
    expect_identical(length(roundtrip$environments[[1]]$variables), 2L)

    expect_identical(roundtrip$value$environment_id, 0L)
    expect_identical(roundtrip$environments[[1]]$parent, -1L)
    expect_identical(roundtrip$environments[[1]]$variables$AAA, ref$AAA)
    expect_identical(roundtrip$environments[[1]]$variables$BBB, ref$BBB)
})

test_that("environment hash chains are correclty loaded", {
    output <- local({
        y <- new.env()

        for (i in seq_len(1000)) {
            name <- basename(tempfile())
            assign(envir=y, name, runif(10))
        }

        tmp <- tempfile(fileext=".rds")
        saveRDS(y, file=tmp)
        list(roundtrip = rds2cpp:::parse(tmp), ref = y)
    }, envir=.GlobalEnv)

    roundtrip <- output$roundtrip
    ref <- output$ref
    expect_identical(sort(names(roundtrip$environments[[1]]$variables)), sort(ls(ref)))

    retrieved <- as.list(ref)
    expect_identical(roundtrip$environments[[1]]$variables[names(retrieved)], retrieved)
})

test_that("non-empty environment writing works as expected", {
    y <- list(aaron=231, jay=runif(5))
    attr(y, "pretend-to-be-an-environment") <- TRUE
    attr(y, "environment-index") <- 0L
    attr(y, "environment-parent") <- -1L
    attr(y, "environment-locked") <- FALSE 

    tmp <- tempfile(fileext=".rds")
    rds2cpp::write(y, file=tmp)
    roundtrip <- readRDS(tmp)

    expect_type(roundtrip, "environment")
    expect_identical(sort(ls(roundtrip)), c("aaron", "jay"))
    expect_identical(roundtrip$aaron, y$aaron)
    expect_identical(roundtrip$jay, y$jay)
})

test_that("parsing environment references works as expected", {
    roundtrip <- local({
        y <- new.env()
        tmp <- tempfile(fileext=".rds")
        saveRDS(list(y, y), file=tmp)
        rds2cpp:::parse(tmp)
    }, envir=.GlobalEnv)

    expect_identical(roundtrip$value[[1]]$environment_id, 0L)
    expect_identical(roundtrip$value[[2]]$environment_id, 0L)

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

    expect_identical(roundtrip$value[[1]]$environment_id, 0L)
    expect_identical(roundtrip$value[[2]]$environment_id, 1L)
    expect_identical(roundtrip$value[[3]]$environment_id, 0L)
    expect_identical(roundtrip$value[[4]]$environment_id, 1L)

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

    expect_identical(roundtrip2$value[[1]]$environment_id, -1L)
    expect_identical(roundtrip2$value[[2]]$environment_id, 0L)
    expect_identical(roundtrip2$value[[3]]$environment_id, 1L)
    expect_identical(roundtrip2$value[[4]]$environment_id, 0L)
    expect_identical(roundtrip2$value[[5]]$environment_id, 1L)
    expect_identical(roundtrip2$value[[6]]$environment_id, -1L)

    expect_identical(roundtrip$environments, roundtrip2$environments)
})

test_that("writing environment references works as expected", {
    z <- list(
        first = {
            y <- list(chino=231, cocoa=runif(5))
            attr(y, "pretend-to-be-an-environment") <- TRUE
            attr(y, "environment-index") <- 0L
            attr(y, "environment-parent") <- -1L
            attr(y, "environment-locked") <- FALSE 
            y
        },
        second = {
            y <- list(rize=TRUE, syaro=FALSE)
            attr(y, "pretend-to-be-an-environment") <- TRUE
            attr(y, "environment-index") <- 1L
            attr(y, "environment-parent") <- -1L
            attr(y, "environment-locked") <- FALSE 
            y
        },
        second_again = {
            y <- list()
            attr(y, "pretend-to-be-an-environment") <- TRUE
            attr(y, "environment-index") <- 1L
            names(y) <- character(0)
            y
        },
        first_again = {
            y <- list()
            attr(y, "pretend-to-be-an-environment") <- TRUE
            attr(y, "environment-index") <- 0L
            names(y) <- character(0)
            y
        }
    )

    tmp <- tempfile(fileext=".rds")
    rds2cpp::write(z, file=tmp)
    roundtrip <- readRDS(tmp)

    expect_type(roundtrip$first, "environment")
    expect_identical(roundtrip$first, roundtrip$first_again)
    expect_identical(sort(ls(roundtrip$first)), c("chino", "cocoa"))
    expect_identical(roundtrip$first$chino, z$first$chino)
    expect_identical(roundtrip$first$cocoa, z$first$cocoa)

    expect_type(roundtrip$second, "environment")
    expect_identical(roundtrip$second, roundtrip$second_again)
    expect_identical(sort(ls(roundtrip$second)), c("rize", "syaro"))
    expect_identical(roundtrip$second$rize, z$second$rize)
    expect_identical(roundtrip$second$syaro, z$second$syaro)
})

test_that("environment parenthood works as expected when parsing", {
    roundtrip <- local({
        y <- new.env()
        y$level <- "A"

        z <- new.env(parent=y)
        z$level <- "B"

        tmp <- tempfile(fileext=".rds")
        saveRDS(z, file=tmp)
        rds2cpp:::parse(tmp)
    }, envir=.GlobalEnv)

    expect_identical(roundtrip$value$environment_id, 0L)
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

    expect_identical(roundtrip$value, list(list(environment_id=0L), list(environment_id=2L)))
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

test_that("environment parenthood works as expected when writing", {
    z <- list(
        first = {
            y <- list(chino=231, cocoa=runif(5))
            attr(y, "pretend-to-be-an-environment") <- TRUE
            attr(y, "environment-index") <- 0L
            attr(y, "environment-parent") <- -1L
            attr(y, "environment-locked") <- FALSE 
            y
        },
        second = {
            y <- list(rize=TRUE, syaro=FALSE)
            attr(y, "pretend-to-be-an-environment") <- TRUE
            attr(y, "environment-index") <- 1L
            attr(y, "environment-parent") <- 0L 
            attr(y, "environment-locked") <- FALSE 
            y
        },
        third = {
            y <- list(chiya="black", megu="red", maya="blue")
            attr(y, "pretend-to-be-an-environment") <- TRUE
            attr(y, "environment-index") <- 2L
            attr(y, "environment-parent") <- 1L 
            attr(y, "environment-locked") <- FALSE 
            y
        }
    )

    tmp <- tempfile(fileext=".rds")
    rds2cpp::write(z, file=tmp)
    roundtrip <- readRDS(tmp)

    expect_type(roundtrip$first, "environment")
    expect_identical(parent.env(roundtrip$first), .GlobalEnv)
    expect_identical(roundtrip$first$chino, z$first$chino)
    expect_identical(roundtrip$first$cocoa, z$first$cocoa)

    expect_type(roundtrip$second, "environment")
    expect_identical(parent.env(roundtrip$second), roundtrip$first)
    expect_identical(roundtrip$second$rize, z$second$rize)
    expect_identical(roundtrip$second$syaro, z$second$syaro)

    expect_type(roundtrip$third, "environment")
    expect_identical(parent.env(roundtrip$third), roundtrip$second)
    expect_identical(roundtrip$third$chiya, z$third$chiya)
    expect_identical(roundtrip$third$megu, z$third$megu)
    expect_identical(roundtrip$third$maya, z$third$maya)
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
    expect_identical(roundtrip$value, list(environment_id=0L))
    expect_identical(length(roundtrip$environments), 1L)

    expect_identical(roundtrip$environments[[1]]$parent, -1L)
    expect_identical(length(roundtrip$environments[[1]]$variables), 3L)

    expect_identical(roundtrip$environments[[1]]$variables$foo, 1:10)
    expect_identical(roundtrip$environments[[1]]$variables$bar, as.character(100:200))
    expect_identical(roundtrip$environments[[1]]$variables$stuff, list(environment_id=0L)) # can refer to itself, no probs.
})

test_that("environment attributes are parsed correctly", {
    roundtrip <- local({
        y <- new.env()
        y$foo <- "BAR"
        attr(y, "name") <- "natalie portman"
        tmp <- tempfile(fileext=".rds")
        saveRDS(y, file=tmp)
        rds2cpp:::parse(tmp)
    }, envir=.GlobalEnv)

    expect_identical(roundtrip$value$environment_id, 0L)
    expect_identical(length(roundtrip$environments), 1L)
    expect_identical(roundtrip$environments[[1]]$parent, -1L)

    expect_identical(length(roundtrip$environments[[1]]$variables), 1L)
    expect_identical(roundtrip$environments[[1]]$variables$foo, "BAR")
    expect_identical(attr(roundtrip$environments[[1]], "name"), "natalie portman")

    # Check that environments are fully pass-by-reference w.r.t. attribute setting.
    roundtrip <- local({
        y <- new.env()
        y$foo <- "BAR"
        z <- y
        attr(z, "name") <- "jessica biel"
        expect_identical(attr(y, "name"), "jessica biel")

        tmp <- tempfile(fileext=".rds")
        saveRDS(list(y, z), file=tmp)
        rds2cpp:::parse(tmp)
    }, envir=.GlobalEnv)

    expect_identical(roundtrip$value, list(list(environment_id=0L), list(environment_id=0L)))
    expect_identical(attr(roundtrip$environments[[1]], "name"), "jessica biel")
})

test_that("environment attributes are saved correctly", {
    y <- list(aaron=231, jay=runif(5))
    attr(y, "pretend-to-be-an-environment") <- TRUE
    attr(y, "environment-index") <- 0L
    attr(y, "environment-parent") <- -1L
    attr(y, "environment-locked") <- FALSE 
    attr(y, "foo") <- "bar"
    
    tmp <- tempfile(fileext=".rds")
    rds2cpp::write(y, file=tmp)
    roundtrip <- readRDS(tmp)

    expect_type(roundtrip, "environment")
    expect_identical(roundtrip$aaron, y$aaron)
    expect_identical(roundtrip$jay, y$jay)
    expect_identical(attr(roundtrip, "foo"), "bar")
})
