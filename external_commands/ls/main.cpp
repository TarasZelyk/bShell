#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <iostream>
#include <chrono>
#include <iomanip>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

size_t get_size(fs::path path) {

    size_t size = 0;
    if (!fs::is_directory(path)) {
        size = fs::file_size(path);
    } else {
        for (fs::recursive_directory_iterator it(path);
             it != fs::recursive_directory_iterator(); ++it) {
            if (!fs::is_directory(*it))
                size += fs::file_size(*it);
        }
    }
    return size;
}

void ls(fs::path path, std::string sort = "N", bool detailed = false, bool reverse = false) {
    if (!fs::exists(path))
        std::cout << "No such file or directory." << std::endl;

    std::vector<fs::path> paths;
    if (fs::is_directory(path)) {
        fs::directory_iterator end_itr;
        copy(fs::directory_iterator(path), fs::directory_iterator(), back_inserter(paths));

        if (sort != "U") {
            std::function<bool(boost::filesystem::path, boost::filesystem::path)> sort_by_name = [](const fs::path &a,
                                                                                                    const fs::path &b) -> bool {
                return a.string() < b.string();
            };
            std::function<bool(boost::filesystem::path, boost::filesystem::path)> sort_by_ext = [](const fs::path &a,
                                                                                                   const fs::path &b) -> bool {
                if (fs::is_directory(a))return true;
                return fs::extension(a) < fs::extension(b);
            };
            std::function<bool(boost::filesystem::path, boost::filesystem::path)> sort_by_time = [](const fs::path &a,
                                                                                                    const fs::path &b) -> bool {
                return fs::last_write_time(a) > fs::last_write_time(b);
            };
            std::function<bool(boost::filesystem::path, boost::filesystem::path)> sort_by_size = [](const fs::path &a,
                                                                                                    const fs::path &b) -> bool {
                return get_size(a) > get_size(b);
            };
            std::function<bool(boost::filesystem::path, boost::filesystem::path)> sort_func;
            if (sort == "S" || sort == "size")
                sort_func = sort_by_size;
            else if (sort == "t" || sort == "time")
                sort_func = sort_by_time;
            else if (sort == "X" || sort == "extension")
                sort_func = sort_by_ext;
            else if (sort == "N" || sort == "name")
                sort_func = sort_by_name;
            else {
                std::cout << "Invalid argument " << sort << " for '--sort'" << std::endl;
                std::cout << "  -  `S`, `size` - by size" << std::endl;
                std::cout << "  -  `t`, `time` - by time" << std::endl;
                std::cout << "  -  `X`, `size` - by extension" << std::endl;
                std::cout << "  -  `N`, `name` - by name" << std::endl;
                std::cout << "  -  `U` - no sorting" << std::endl;
                return;
            }

            std::sort(paths.begin(), paths.end(), sort_func);
            if (reverse)
                std::reverse(paths.begin(), paths.end());
        }
    } else {
        paths.push_back(path);
    }

    for (fs::path p : paths) {
        if (detailed) {
            try {
                size_t size = get_size(p);
                std::cout << std::setw(12) << std::right << size << " ";

                time_t tt = fs::last_write_time(p);
                tm tm = *localtime(&tt);
                std::cout << std::setw(30) << std::right << std::put_time(&tm, "%d/%m/%Y %H:%M:%S");

                std::cout << std::setw(0) << " ";

            } catch (fs::filesystem_error &e) {

                std::cout << p.string() << " " << e.what() << '\n';
            }
        }
        if (p.string().find("./") == 0) {
            if (!fs::is_directory(p)) {
                std::cout << p.string().substr(2) << std::endl;
            } else {
                std::cout << p.string().substr(1) << std::endl;
            }
        } else {
            std::cout << p.string() << std::endl;

        }
    }

}

int main(int ac, char *av[]) {
    try {
        po::options_description desc(
                "Usage: ls [OPTION]... [FILE]...\nList information about"
                        " the file(s) (the current directory by default).\n\nAllowed options");
        desc.add_options()
                ("help,h", "Display help message")
                ("path", po::value<std::string>(), "path to the directory/file")
                (",l", "Use a long listing format")
                (",N", "sort alphabetically by name extension")
                (",r", "reverse order while sorting")
                (",S", "sort by file size, largest first")
                (",t", "sort by modification time, newest first")
                (",U", "do not sort; list entries in directory order")
                (",X", "sort alphabetically by entry extension")
                ("sort", po::value<std::string>(), "");

        po::positional_options_description positionalOptions;
        positionalOptions.add("path", 1);

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
        bool reverse = false;
        if (vm.count("-r")) {
            reverse = true;
        }
        bool detailed = false;
        if (vm.count("-l")) {
            detailed = true;
        }
        std::string sort = "name";
        if (vm.count("-S")) { sort = "S"; }
        if (vm.count("-N")) { sort = "N"; }
        if (vm.count("-X")) { sort = "X"; }
        if (vm.count("-T")) { sort = "t"; }
        if (vm.count("sort")) {
            sort = vm["sort"].as<std::string>();
        }
        fs::path path;
        if (vm.count("path")) {
            path = fs::path(vm["path"].as<std::string>());
        } else {
            path = fs::path(".");
        }

        ls(path, sort, detailed, reverse);

    }
    catch (std::exception &e) {
        std::cerr << "error: " << e.what() << std::endl;
        return 1;
    }
    catch (...) {
        std::cerr << "Exception of unknown type!" << std::endl;
    }

    return 0;
}
