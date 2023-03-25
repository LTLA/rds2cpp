#' @export
inspect <- function(path, chunk.size=1000) {
    handle <- gzfile(path, open="rb")
    chunks <- list()
    i <- 1L
    while (1) {
        chunks[[i]] <- readBin(handle, what=raw(), n=chunk.size) 
        if (length(chunks[[i]]) < chunk.size) {
            break
        }
        i <- i + 1L
    }
    unlist(chunks)
}
