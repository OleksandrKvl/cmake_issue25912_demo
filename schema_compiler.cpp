#include <iostream>
#include <string>
#include <fstream>

int main(int argc, char** argv)
{
    if(argc != 3)
    {
        std::cerr << "Wrong number of arguments\n";
        return 1;
    }

    const auto path = argv[1];
    const auto anchor_path = argv[2];
    std::ifstream file(path);
    if(!file.is_open())
    {
        std::cerr << "Unable to open file: " << path << '\n';
        return 1;
    }

    // create a file for each line
    std::string output_file_name;
    while(std::getline(file, output_file_name))
    {
        std::ofstream output_file(output_file_name);
        if(!output_file.is_open())
        {
            std::cerr << "Unable to create file: " << output_file_name << '\n';
            return 1;
        }

        output_file << "#include \"" << anchor_path << '"';
        output_file.close();
    }

    file.close();

    return 0;
}