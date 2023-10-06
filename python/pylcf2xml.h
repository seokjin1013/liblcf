#pragma once

#include <lcf/ldb/reader.h>
#include <lcf/lmt/reader.h>
#include <lcf/lmu/reader.h>
#include <lcf/lsd/reader.h>
#include <lcf/reader_lcf.h>
#include <lcf/reader_util.h>
#include <sstream>
#include <pybind11/pybind11.h>

pybind11::bytes lmu2xml(const pybind11::bytes& in, std::string engine, std::string encoding);

pybind11::bytes lsd2xml(const pybind11::bytes& in, std::string engine, std::string encoding);

pybind11::bytes ldb2xml(const pybind11::bytes& in, std::string encoding);

pybind11::bytes lmt2xml(const pybind11::bytes& in, std::string engine, std::string encoding);

pybind11::bytes xml2lmu(const pybind11::bytes& in, std::string engine, std::string encoding);

pybind11::bytes xml2lsd(const pybind11::bytes& in, std::string engine, std::string encoding);

pybind11::bytes xml2ldb(const pybind11::bytes& in, std::string encoding);

pybind11::bytes xml2lmt(const pybind11::bytes& in, std::string engine, std::string encoding);