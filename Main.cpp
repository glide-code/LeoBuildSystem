#include "BuildSystem.hpp"
#include "Utils.hpp"

int main(int argc, char** argv)
{
    std::cout << "------------[ Leo Build System ]------------\n";

    if(argc < 2)
    {
        std::cout <<
        "Usage: buildsystem <projectfile.xml>\n" <<
        "\n";

        return 0;
    }

    Leo::BuildSystem buildSystem;
    bool success = buildSystem.ReadProjectFile(argv[1]);
    if(success) buildSystem.StartBuild();

    return 0;
}