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
    obj.attributes.emplace_back(file_info.symbols.size(), std::move(ivec));
    file_info.symbols.emplace_back("i", rds2cpp::StringEncoding::UTF8);

    auto pvec = std::make_unique<rds2cpp::IntegerVector>();
    pvec->data = std::vector<int32_t>{ 0, 0, 2, 3, 4, 5, 6, 8, 8, 8, 10 };
    obj.attributes.emplace_back(file_info.symbols.size(), std::move(pvec));
    file_info.symbols.emplace_back("p", rds2cpp::StringEncoding::UTF8);

    auto xvec = std::make_unique<rds2cpp::DoubleVector>();
    xvec->data = std::vector<double>{ 0.96, -0.34, 0.82, -2, -0.072, 0.39, 0.16, 0.36, -1.5, -0.047 };
    obj.attributes.emplace_back(file_info.symbols.size(), std::move(xvec));
    file_info.symbols.emplace_back("x", rds2cpp::StringEncoding::UTF8);

    auto dims = std::make_unique<rds2cpp::IntegerVector>();
    dims->data = std::vector<int32_t>{ 10, 10 };
    obj.attributes.emplace_back(file_info.symbols.size(), std::move(dims));
    file_info.symbols.emplace_back("Dim", rds2cpp::StringEncoding::UTF8);

    auto dimnames = std::make_unique<rds2cpp::GenericVector>();
    dimnames->data.emplace_back(new rds2cpp::Null);
    dimnames->data.emplace_back(new rds2cpp::Null);
    obj.attributes.emplace_back(file_info.symbols.size(), std::move(dimnames));
    file_info.symbols.emplace_back("Dimnames", rds2cpp::StringEncoding::UTF8);

    auto factors = std::make_unique<rds2cpp::GenericVector>();
    obj.attributes.emplace_back(file_info.symbols.size(), std::move(factors));
    file_info.symbols.emplace_back("factors", rds2cpp::StringEncoding::UTF8);

    rds2cpp::write_rds(file_info, "my_matrix.rds", {});
    return 0;
}
