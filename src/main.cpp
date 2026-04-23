#include "Application.hpp"
#include "ConfigParser.hpp"

int main(int argc, char* argv[]) {
    engine::Config config = engine::ConfigParser::parseArguments(argc, argv);

    engine::Application app(config);
    app.run();

    return 0;
}
