/*
Developed by Nina Bondar for the beerShell.
Ask bondar_n@ucu.edu.ua for all the questions and bugs.

Version 1.0.1
April 1, 2017
*/

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

namespace po = boost::program_options;
namespace fs = boost::filesystem;

#include <iostream>

void copy_file(boost::filesystem::path old_file, boost::filesystem::path new_file, bool force_overwrite = false) {
    /*
     * Bug on removin old file if user selected "skip" option 
     * fixed by Sergey Dubovyk,
     * discovered by Taras Zelyk.
     */
    if (!fs::exists(old_file)) {
        std::cerr << "No such file: " << old_file << std::endl;
        return;
    }
    if (fs::exists(new_file) && fs::is_directory(new_file)) {
        std::cerr << "Target " << new_file << " is a directory" << std::endl;
        return;
    }

    if (fs::exists(new_file)) {
        if (!force_overwrite) {
            std::cerr << "File " << new_file << " already exists. Do you want to overwrite it?(y/n)"
                      << std::endl;
            std::string choice;
            std::cin >> choice;
            if (choice != "y") {
                std::cerr << "Skipping..." << std::endl;
                return;
            }
        }
        fs::remove(new_file);
    }

    fs::copy_file(old_file, new_file);
}

int main(int ac, char *av[]) {
    try {

        po::options_description desc("Usage:  cp [OPTION]... SOURCE... DIRECTORY\n"
                                             "Copy SOURCE to DEST, or multiple SOURCE(s) to DIRECTORY.\n\n"
                                             "Allowed options");
        desc.add_options()
                ("help,h", "Display help message")
                ("force,f", "Do not prompt before overwriting")
                ("files", po::value<std::vector<boost::filesystem::path>>(), "Input file(s) and destination");

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
        /*
         * Bug on multiple files: only 1s file proceeded 
         * fixed by Sergey Dubovyk,
         * discovered by Taras Zelyk.
         */
        if (vm.count("files")) {
            std::vector<boost::filesystem::path> files = vm["files"].as<std::vector<boost::filesystem::path>>();
            if (files.size() == 2) {
                copy_file(files[0], files[1], force_overwrite);
            } else if (files.size() > 2) {
                boost::filesystem::path directory = files[files.size() - 1];
                files.pop_back();
                if (!fs::exists(directory) && !fs::is_directory(directory)) {
                    std::cerr << "target " << directory << " does not exist or is not a directory" << std::endl;
                    return 0;
                }
                for (boost::filesystem::path file:files) {
                    if (!fs::exists(file)) {
                        std::cerr << "No such file: " << file << std::endl;
                        return 0;
                    }
                    copy_file(file, directory / file, force_overwrite);

                }
            } else {
                std::cerr << "Missing destination file operand afrer " << files[0] << std::endl;
            }
        } else {
            std::cerr << "Missing file operand.\n";
        }
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
