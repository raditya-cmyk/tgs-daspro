#include "bibit.h"
#include "fileio.h"
#include "ui.h"
#include <iostream>
using namespace std;

int main() {
    bool run = true;

    katalog k{};
    load_data(k, "data/bibit.txt");

    while (run) {
        tampilkan_semua(k.data, k.n, "Biji-bijian tanaman");
        command(k, run);
    }
    return 0;
}
