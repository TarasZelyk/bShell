#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <fstream>
#include <boost/algorithm/string.hpp>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

#include <iostream>
#include <boost/regex.hpp>

void grep(boost::filesystem::path file, std::string regexp,
          std::string search_string, bool ignore_case, bool invert_match) {

    boost::regex re(regexp, (ignore_case) ? boost::regex::icase : NULL);
    if (ignore_case) {
        boost::algorithm::to_lower(search_string);
    }
    std::string line;
    std::istream *in;
    if (!file.empty()) {
        std::ifstream fin(file.c_str());
        in = &fin;
    } else {
        in = &std::cin;
    }
    boost::cmatch what;
    while (std::getline(*in, line)) {
        if (ignore_case) {
            boost::algorithm::to_lower(line);
        }
        bool matches;
        if (regexp != "") {
            matches = boost::regex_match(line.c_str(), what, re);
        } else {
            matches = line.find(search_string) != std::string::npos;
        }
        if ((matches && !invert_match) || (!matches && invert_match)) {
            std::cout << line << std::endl;
        }
    }

}

int main(int ac, char *av[]) {
    try {
        std::string regexp, search_string;
        boost::filesystem::path file;
        po::options_description desc("Usage:  mygrep [OPTION]... PATTERN\n"
                                             "Search for a PATTERN in standart input or a file.\n\n"
                                             "Allowed options");
        desc.add_options()
                ("help,h", "Display help message")
                ("invert-match,v", "Select non matching lines")
                ("ignore-case,i", "ignore case distinctions")
                ("regexp", po::value<std::string>(&regexp), "PATTERN is a regular expression")
                ("string", po::value<std::string>(&search_string), "PATTERN for search")
                ("file", po::value<boost::filesystem::path>(&file), "Input file");

        po::positional_options_description positionalOptions;
        positionalOptions.add("string", -1);

        po::variables_map vm;
        po::store(
                po::command_line_parser(ac, av)
                        .options(desc)
                        .positional(positionalOptions)
                        .run(),
                vm);
        po::notify(vm);

        if (vm.count("help")) {
            std::cout << desc << "\n";
            return 0;
        }
        bool invert_match = false;
        if (vm.count("invert-match")) {
            invert_match = true;
        }

        bool ignore_case = false;
        if (vm.count("ignore-case")) {
            ignore_case = true;
        }
        grep(file, regexp, search_string, ignore_case, invert_match);
    }
    catch (std::exception &e) {
        std::cerr << "error: " << e.what() << "\n";
        return 1;
    }
    catch (...) {
        std::cerr << "Exception of unknown type!\n";
    }
    return 0;
}
