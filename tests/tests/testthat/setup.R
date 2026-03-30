quick_parse <- function(x) {
    rds2cpp::parse_rds(x, parallel=FALSE)
}

quick_write <- function(x, file_name) {
    rds2cpp::write_rds(x, file_name=file_name, parallel=FALSE)
}
