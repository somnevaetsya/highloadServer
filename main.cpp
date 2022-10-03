#include "server.h"
#include "conf.h"
#include <iostream>

int main() {
    Conf conf{};
    conf.Parse("../etc/httpd.conf");
    std::cout << "Pool`s size: " << conf.GetCpuLimit() << std::endl;
    std::cout.flush();
    Server server(conf.GetCpuLimit(), conf.GetThreadLimit(), conf.GetDocumentRoot());
    if(!server.init()) {
        return 1;
    }
    server.run();
}
