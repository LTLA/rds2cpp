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

    obj.attributes.names.push_back("i");
    obj.attributes.encodings.push_back(rds2cpp::StringEncoding::UTF8);
    auto ivec = new rds2cpp::IntegerVector;
    obj.attributes.values.emplace_back(ivec);
    ivec->data = std::vector<int32_t>{ 6, 8, 0, 3, 5, 6, 0, 1, 3, 7 };

    obj.attributes.names.push_back("p");
    obj.attributes.encodings.push_back(rds2cpp::StringEncoding::UTF8);
    auto pvec = new rds2cpp::IntegerVector;
    obj.attributes.values.emplace_back(pvec);
    pvec->data = std::vector<int32_t>{ 0, 0, 2, 3, 4, 5, 6, 8, 8, 8, 10 };

    obj.attributes.names.push_back("x");
    obj.attributes.encodings.push_back(rds2cpp::StringEncoding::UTF8);
    auto xvec = new rds2cpp::DoubleVector;
    obj.attributes.values.emplace_back(xvec);
    xvec->data = std::vector<double>{ 0.96, -0.34, 0.82, -2, -0.072, 0.39, 0.16, 0.36, -1.5, -0.047 };

    obj.attributes.names.push_back("Dim");
    obj.attributes.encodings.push_back(rds2cpp::StringEncoding::UTF8);
    auto dims = new rds2cpp::IntegerVector;
    obj.attributes.values.emplace_back(dims);
    dims->data = std::vector<int32_t>{ 10, 10 };

    obj.attributes.names.push_back("Dimnames");
    obj.attributes.encodings.push_back(rds2cpp::StringEncoding::UTF8);
    auto dimnames = new rds2cpp::GenericVector;
    obj.attributes.values.emplace_back(dimnames);
    dimnames->data.emplace_back(new rds2cpp::Null);
    dimnames->data.emplace_back(new rds2cpp::Null);

    obj.attributes.names.push_back("factors");
    obj.attributes.encodings.push_back(rds2cpp::StringEncoding::UTF8);
    auto factors = new rds2cpp::GenericVector;
    obj.attributes.values.emplace_back(factors);

    rds2cpp::write_rds(file_info, "my_matrix.rds");
    return 0;
}
