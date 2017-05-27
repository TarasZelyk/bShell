
/*
Developed by Taras Zelyk for the beerShell.
Ask zelyk@ucu.edu.ua for all the questions and bugs.
Version 1.0.4
April 2, 2017
*/

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/regex.hpp>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <pwd.h>
#include <grp.h>
#include <sys/stat.h>


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

void ls(fs::path path, std::string sort = "N", bool detailed = false,
        bool reverse = false, bool show_indicators = false) {
    /*
     * Minor bugfixes S.Dubovyk.
     */
    std::vector<fs::path> paths;
    if (!fs::exists(path)) {

        std::cerr << "No such file or directory." << std::endl;
        return;
    }
    if (fs::is_directory(path)) {
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
                std::cerr << "Invalid argument " << sort << " for '--sort'" << std::endl;
                std::cerr << "  -  `S`, `size` - by size" << std::endl;
                std::cerr << "  -  `t`, `time` - by time" << std::endl;
                std::cerr << "  -  `X`, `size` - by extension" << std::endl;
                std::cerr << "  -  `N`, `name` - by name" << std::endl;
                std::cerr << "  -  `U` - no sorting" << std::endl;
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
        /*
        * Bug: wrong output format
        * fixed by Sergey Dubovyk,
        * discovered by Sergey Dubovyk.
        */
        struct stat info;
        stat(p.c_str(), &info);
        if (detailed) {
            try {
                size_t size = get_size(p);
                std::cout << std::setw(4) << std::right << fs::status(p).permissions() << " ";

                struct passwd *pw = getpwuid(info.st_uid);
                struct group *gr = getgrgid(info.st_gid);
                std::cout << std::setw(10) << std::right << pw->pw_name << " ";
                std::cout << std::setw(10) << std::right << gr->gr_name << " ";

                std::cout << std::setw(12) << std::right << size << " ";

                time_t tt = fs::last_write_time(p);
                tm tm = *localtime(&tt);
                std::cout << std::setw(30) << std::right << std::put_time(&tm, "%d/%m/%Y %H:%M:%S");

                std::cout << std::setw(0) << " ";

            } catch (fs::filesystem_error &e) {

                std::cout << p.string() << " " << e.what() << '\n';
            }
        }
        std::string filename = p.filename().string();
        if (fs::is_directory(p)) {
            filename = "/" + filename;
        }
        if (show_indicators) {
            if ((info.st_mode & S_IEXEC) != 0 and !fs::is_directory(p))
                filename += "*";

            if (fs::symlink_status(p).type() == fs::symlink_file)
                filename += "@";
        }
        std::cout << filename << std::endl;

    }

}

int main(int ac, char *av[]) {
    /*
     * Bugfixing by Dubovyk Sergey + added some descs and style updates.
     */
    try {
        po::options_description desc(
                "Usage: ls [OPTION]... [FILE]...\nList information about"
                        " the file(s) (the current directory by default).\n\nAllowed options");
        desc.add_options()
                ("help,h", "Display help message")
                ("path", po::value<std::string>(), "path to the directory/file")
                (",l", "Use a long listing format")
                (",F", "append indicator (one of *=@) to entries")
                (",N", "sort alphabetically by name extension")
                (",r", "reverse order while sorting")
                (",R", "list subdirectories recursively")
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
        bool recursive = false;
        if (vm.count("-R")) {
            recursive = true;
        }

        bool reverse = false;
        if (vm.count("-r")) {
            reverse = true;
        }
        bool detailed = false;
        if (vm.count("-l")) {
            detailed = true;
        }
        bool show_indicators = false;
        if (vm.count("-F")) {
            show_indicators = true;
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
        if (recursive) {
            for (fs::recursive_directory_iterator it(path);
                 it != fs::recursive_directory_iterator(); ++it) {
                if (fs::is_directory(*it)) {
                    std::cerr << it->path().string() << ":" << std::endl;
                    ls(it->path(), sort, detailed, reverse, show_indicators);
                    std::cerr << std::endl;
                }
            }
        } else {
            ls(path, sort, detailed, reverse,show_indicators);

        }

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
