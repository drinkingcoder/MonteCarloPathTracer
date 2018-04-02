/*
*  Simple monte carlo path tracer in C++
*
*  Compilation:
*      $ mkdir build
*      $ cd build
*      $ cmake ..
*      $ make
*
*  Usage:
*      $ ./pathtracer <number of samples>
*/

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

#include "Type.h"
#include "Timer.h"
#include "Material.h"
#include "Objects.h"
#include "Camera.h"
#include "Scene.h"
#include "Renderer.h"
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

Configuration config;

void ParseConfiguration(Configuration & config) {
    timer config_timer = timer();
    config_timer.start();
    cout << "parsing configuraion.." << endl;
    ifstream ifs;
    ifs.open("config.json");
    assert(ifs.is_open());

    json j;
    ifs >> j;
    config = Configuration {
        j["obj"].get<string>(),
        j["output_name"].get<string>(),
        j["width"].get<int>(),
        j["height"].get<int>(),
        j["samples"].get<int>(),
        j["batch_size"].get<int>(),
        Vector3(
                j["camera_x"].get<double>(),
                j["camera_y"].get<double>(),
                j["camera_z"].get<double>()
               ),
        Vector3(
                j["look_at_x"].get<double>(),
                j["look_at_y"].get<double>(),
                j["look_at_z"].get<double>()
               ),
        Vector3(
                j["ambient_light_r"].get<double>(),
                j["ambient_light_g"].get<double>(),
                j["ambient_light_b"].get<double>()
               ),
        j["light"].get<bool>()
    };
    cout << "obj = " << config.obj << endl;
    cout << "width = " << config.width << endl;
    cout << "height = " << config.height << endl;
    cout << "finished." << endl;
    config_timer.stop();
    cout << "parsing configuration consumes: " << config_timer.get_seconds() << " secs." << endl;
}


int main(int argc, char *argv[]) {

#ifdef TIMER_COUNT
    printf("timer enabled.\n");
    global_timer::initialize();
    global_timer::timers[GLOBAL_TIMER].start();
#endif

    ParseConfiguration(config);

    Renderer renderer = Renderer(config);  // Create renderer with our scene and camera
    renderer.render();                       // Render image to pixel buffer

#ifdef TIMER_COUNT
    global_timer::timers[GLOBAL_TIMER].stop();
#endif

    printf("Global timer counts %lf secs\n\n", global_timer::timers[0].get_seconds());
    return 0;
}
