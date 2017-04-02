#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

#include <iostream>

void remove_file(boost::filesystem::path file, bool force_removal = false, bool remove_directories = false) {
    if (!fs::exists(file)) {
        std::cout << "No such file: " << file << std::endl;
        return;
    }
    if (fs::is_directory(file) && !remove_directories) {
        std::cout << "Can not remove " << file << ": It's a directory.\nUse option -R to remove directories. "
                  << std::endl;
        return;
    }

    if (!force_removal) {
        std::cout << "Are you sure you want to remove " << file << "?(y/n)"
                  << std::endl;
        std::string choice;
        std::cin >> choice;
        if (choice != "y") {
            std::cout << "Skipping..." << std::endl;
            return;
        }
    }

    fs::remove_all(file);
}

int main(int ac, char *av[]) {
    try {

        po::options_description desc("Usage: rm [OPTION]... [FILE]...\nRemove the FILE(s).\n"
                                             "\nAllowed options");
        desc.add_options()
                ("help,h", "Display help message")
                ("force,f", "Do not prompt before overwriting")
                ("remove_dir,R", "Remove directories and their contents recursively")
                ("files", po::value<std::vector<boost::filesystem::path>>(), "Input file(s)");

        po::positional_options_description positionalOptions;
        positionalOptions.add("files", -1);

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
        bool force_overwrite = false;
        if (vm.count("force")) {
            force_overwrite = true;
        }
        bool remove_directories = false;
        if (vm.count("remove_dir")) {
            remove_directories = true;
        }
        if (vm.count("files")) {
            std::vector<boost::filesystem::path> files = vm["files"].as<std::vector<boost::filesystem::path>>();
            for (boost::filesystem::path file:files) {
                remove_file(file, force_overwrite, remove_directories);
            }

        } else {
            std::cout << "Missing file operand." << std::endl;
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
