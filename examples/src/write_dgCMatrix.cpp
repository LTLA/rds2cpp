#include "rds2cpp/rds2cpp.hpp"

int main () {
    rds2cpp::RdsFile file_info;
    auto ptr = new rds2cpp::S4Object;
    file_info.object.reset(ptr);
    auto& obj = *ptr;

    obj.class_name = "dgCMatrix";
    obj.class_encoding = rds2cpp::StringEncoding::UTF8;
    obj.package_name = "Matrix";
    obj.package_encoding = rds2cpp::StringEncoding::UTF8;

    auto ivec = std::make_unique<rds2cpp::IntegerVector>();
    ivec->data = std::vector<int32_t>{ 6, 8, 0, 3, 5, 6, 0, 1, 3, 7 };
    obj.attributes.emplace_back(rds2cpp::register_symbol("i", rds2cpp::StringEncoding::UTF8, file_info.symbols), std::move(ivec));

    auto pvec = std::make_unique<rds2cpp::IntegerVector>();
    pvec->data = std::vector<int32_t>{ 0, 0, 2, 3, 4, 5, 6, 8, 8, 8, 10 };
    obj.attributes.emplace_back(rds2cpp::register_symbol("p", rds2cpp::StringEncoding::UTF8, file_info.symbols), std::move(pvec));

    auto xvec = std::make_unique<rds2cpp::DoubleVector>();
    xvec->data = std::vector<double>{ 0.96, -0.34, 0.82, -2, -0.072, 0.39, 0.16, 0.36, -1.5, -0.047 };
    obj.attributes.emplace_back(rds2cpp::register_symbol("x", rds2cpp::StringEncoding::UTF8, file_info.symbols), std::move(xvec));

    auto dims = std::make_unique<rds2cpp::IntegerVector>();
    dims->data = std::vector<int32_t>{ 10, 10 };
    obj.attributes.emplace_back(rds2cpp::register_symbol("Dim", rds2cpp::StringEncoding::UTF8, file_info.symbols), std::move(dims));

    auto dimnames = std::make_unique<rds2cpp::GenericVector>();
    dimnames->data.emplace_back(new rds2cpp::Null);
    dimnames->data.emplace_back(new rds2cpp::Null);
    obj.attributes.emplace_back(rds2cpp::register_symbol("Dimnames", rds2cpp::StringEncoding::UTF8, file_info.symbols), std::move(dimnames));

    auto factors = std::make_unique<rds2cpp::GenericVector>();
    obj.attributes.emplace_back(rds2cpp::register_symbol("factors", rds2cpp::StringEncoding::UTF8, file_info.symbols), std::move(factors));

    rds2cpp::write_rds(file_info, "my_matrix.rds", {});
    return 0;
}
